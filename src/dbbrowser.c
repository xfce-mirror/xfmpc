/*
 *  Copyright (c) 2008-2009 Mike Massonnet <mmassonnet@xfce.org>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <gtk/gtk.h>
#include <gdk/gdkkeysyms.h>
#include <libxfce4util/libxfce4util.h>

#include "dbbrowser.h"
#include "preferences.h"
#include "mpdclient.h"

#define BORDER 4

#define GET_PRIVATE(o) \
    (G_TYPE_INSTANCE_GET_PRIVATE ((o), XFMPC_TYPE_DBBROWSER, XfmpcDbbrowserPrivate))



static void             xfmpc_dbbrowser_class_init             (XfmpcDbbrowserClass *klass);
static void             xfmpc_dbbrowser_init                   (XfmpcDbbrowser *dbbrowser);
static void             xfmpc_dbbrowser_dispose                (GObject *object);
static void             xfmpc_dbbrowser_finalize               (GObject *object);

static void             cb_row_activated                       (XfmpcDbbrowser *dbbrowser,
                                                                GtkTreePath *path,
                                                                GtkTreeViewColumn *column);
static gboolean         cb_key_pressed                         (XfmpcDbbrowser *dbbrowser,
                                                                GdkEventKey *event);
static gboolean         cb_button_pressed                      (XfmpcDbbrowser *dbbrowser,
                                                                GdkEventButton *event);
static gboolean         cb_popup_menu                          (XfmpcDbbrowser *dbbrowser);
static void             popup_menu                             (XfmpcDbbrowser *dbbrowser);
static void             cb_browse                              (XfmpcDbbrowser *dbbrowser);

static void             cb_search_entry_activated              (XfmpcDbbrowser *dbbrowser);
static gboolean         cb_search_entry_key_released           (XfmpcDbbrowser *dbbrowser,
                                                                GdkEventKey *event);
static void             cb_search_entry_changed                (XfmpcDbbrowser *dbbrowser);
static gboolean         timeout_search                         (XfmpcDbbrowser *dbbrowser);
static void             timeout_search_destroy                 (XfmpcDbbrowser *dbbrowser);

static void             cb_playlist_changed                    (XfmpcDbbrowser *dbbrowser);



/* List store identifiers */
enum
{
  COLUMN_ID,
  COLUMN_PIXBUF,
  COLUMN_FILENAME,
  COLUMN_BASENAME,
  COLUMN_IS_DIR,
  COLUMN_WEIGHT,
  N_COLUMNS,
};



struct _XfmpcDbbrowserClass
{
  GtkVBoxClass              parent_class;
};

struct _XfmpcDbbrowser
{
  GtkVBox                   parent;
  XfmpcPreferences         *preferences;
  XfmpcMpdclient           *mpdclient;
  /*<private>*/
  XfmpcDbbrowserPrivate    *priv;
};

struct _XfmpcDbbrowserPrivate
{
  GtkWidget                *treeview;
  GtkListStore             *store;
  GtkWidget                *search_entry;
  GtkWidget                *menu;
  GtkWidget                *mi_browse;

  guint                     search_timeout;
  gboolean                  is_searching;

  gchar                    *wdir;
  gchar                    *last_wdir;
};



static GObjectClass *parent_class = NULL;



GType
xfmpc_dbbrowser_get_type ()
{
  static GType xfmpc_dbbrowser_type = G_TYPE_INVALID;

  if (G_UNLIKELY (xfmpc_dbbrowser_type == G_TYPE_INVALID))
    {
      static const GTypeInfo xfmpc_dbbrowser_info =
        {
          sizeof (XfmpcDbbrowserClass),
          (GBaseInitFunc) NULL,
          (GBaseFinalizeFunc) NULL,
          (GClassInitFunc) xfmpc_dbbrowser_class_init,
          (GClassFinalizeFunc) NULL,
          NULL,
          sizeof (XfmpcDbbrowser),
          0,
          (GInstanceInitFunc) xfmpc_dbbrowser_init,
          NULL
        };
      xfmpc_dbbrowser_type = g_type_register_static (GTK_TYPE_VBOX, "XfmpcDbbrowser", &xfmpc_dbbrowser_info, 0);
    }

  return xfmpc_dbbrowser_type;
}



static void
xfmpc_dbbrowser_class_init (XfmpcDbbrowserClass *klass)
{
  GObjectClass *gobject_class;

  g_type_class_add_private (klass, sizeof (XfmpcDbbrowserPrivate));

  parent_class = g_type_class_peek_parent (klass);

  gobject_class = G_OBJECT_CLASS (klass);
  gobject_class->dispose = xfmpc_dbbrowser_dispose;
  gobject_class->finalize = xfmpc_dbbrowser_finalize;
}

static void
xfmpc_dbbrowser_init (XfmpcDbbrowser *dbbrowser)
{
  XfmpcDbbrowserPrivate *priv = dbbrowser->priv = GET_PRIVATE (dbbrowser);

  dbbrowser->preferences = xfmpc_preferences_get ();
  dbbrowser->mpdclient = xfmpc_mpdclient_get ();

  g_object_get (G_OBJECT (dbbrowser->preferences),
                "dbbrowser-last-path", &priv->wdir,
                NULL);
  priv->last_wdir = g_strdup (priv->wdir);

  /* === Tree model === */
  priv->store = gtk_list_store_new (N_COLUMNS,
                                    G_TYPE_INT,
                                    GDK_TYPE_PIXBUF,
                                    G_TYPE_STRING,
                                    G_TYPE_STRING,
                                    G_TYPE_BOOLEAN,
                                    G_TYPE_INT);

  /* === Tree view === */
  priv->treeview = gtk_tree_view_new ();
  gtk_tree_selection_set_mode (gtk_tree_view_get_selection (GTK_TREE_VIEW (priv->treeview)), GTK_SELECTION_MULTIPLE);
  gtk_tree_view_set_rubber_banding (GTK_TREE_VIEW (priv->treeview), TRUE);
  gtk_tree_view_set_enable_search (GTK_TREE_VIEW (priv->treeview), TRUE);
  gtk_tree_view_set_search_column (GTK_TREE_VIEW (priv->treeview), COLUMN_BASENAME);
  gtk_tree_view_set_headers_visible (GTK_TREE_VIEW (priv->treeview), FALSE);
  gtk_tree_view_set_rules_hint (GTK_TREE_VIEW (priv->treeview), TRUE);
  gtk_tree_view_set_model (GTK_TREE_VIEW (priv->treeview), GTK_TREE_MODEL (priv->store));
  g_object_unref (priv->store);

  /* Columns */
  GtkCellRenderer *cell = gtk_cell_renderer_pixbuf_new ();
  gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (priv->treeview),
                                               -1, "", cell,
                                               "pixbuf", COLUMN_PIXBUF,
                                               NULL);

  cell = gtk_cell_renderer_text_new ();
  g_object_set (G_OBJECT (cell),
                "ellipsize", PANGO_ELLIPSIZE_END,
                NULL);
  gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (priv->treeview),
                                               -1, "Filename", cell,
                                               "text", COLUMN_BASENAME,
                                               "weight", COLUMN_WEIGHT,
                                               NULL);

  /* Scrolled window */
  GtkWidget *scrolled = gtk_scrolled_window_new (NULL, NULL);
  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolled),
                                  GTK_POLICY_AUTOMATIC,
                                  GTK_POLICY_ALWAYS);

  /* Menu */
  priv->menu = gtk_menu_new ();

  /* Menu -> Add */
  GtkWidget *mi = gtk_image_menu_item_new_from_stock (GTK_STOCK_ADD, NULL);
  gtk_menu_shell_append (GTK_MENU_SHELL (priv->menu), mi);
  g_signal_connect_swapped (mi, "activate",
                            G_CALLBACK (xfmpc_dbbrowser_add_selected_rows), dbbrowser);

  /* Menu -> Replace */
  mi = gtk_image_menu_item_new_with_mnemonic (_("_Replace"));
  gtk_menu_shell_append (GTK_MENU_SHELL (priv->menu), mi);
  g_signal_connect_swapped (mi, "activate",
                            G_CALLBACK (xfmpc_dbbrowser_replace_with_selected_rows), dbbrowser);
  GtkWidget *image = gtk_image_new_from_stock (GTK_STOCK_CUT, GTK_ICON_SIZE_MENU);
  gtk_image_menu_item_set_image (GTK_IMAGE_MENU_ITEM (mi), image);

  /* Menu -> Browse (only shown on a search) */
  mi = priv->mi_browse = gtk_image_menu_item_new_with_mnemonic (_("_Browse"));
  gtk_menu_shell_append (GTK_MENU_SHELL (priv->menu), mi);
  g_signal_connect_swapped (mi, "activate",
                            G_CALLBACK (cb_browse), dbbrowser);
  image = gtk_image_new_from_stock (GTK_STOCK_OPEN, GTK_ICON_SIZE_MENU);
  gtk_image_menu_item_set_image (GTK_IMAGE_MENU_ITEM (mi), image);

  gtk_widget_show_all (priv->menu);

  /* === Search entry === */
  priv->search_entry = gtk_entry_new ();

  /* === Containers === */
  gtk_container_add (GTK_CONTAINER (scrolled), priv->treeview);
  gtk_box_pack_start (GTK_BOX (dbbrowser), scrolled, TRUE, TRUE, 0);
  gtk_box_pack_start (GTK_BOX (dbbrowser), priv->search_entry, FALSE, FALSE, 0);

  /* === Signals === */
  g_signal_connect_swapped (dbbrowser->mpdclient, "connected",
                            G_CALLBACK (xfmpc_dbbrowser_reload), dbbrowser);
  g_signal_connect_swapped (dbbrowser->mpdclient, "database-changed",
                            G_CALLBACK (xfmpc_dbbrowser_reload), dbbrowser);
  g_signal_connect_swapped (dbbrowser->mpdclient, "playlist-changed",
                            G_CALLBACK (cb_playlist_changed), dbbrowser);
  /* Tree view */
  g_signal_connect_swapped (priv->treeview, "row-activated",
                            G_CALLBACK (cb_row_activated), dbbrowser);
  g_signal_connect_swapped (priv->treeview, "key-press-event",
                            G_CALLBACK (cb_key_pressed), dbbrowser);
  g_signal_connect_swapped (priv->treeview, "button-press-event",
                            G_CALLBACK (cb_button_pressed), dbbrowser);
  g_signal_connect_swapped (priv->treeview, "popup-menu",
                            G_CALLBACK (cb_popup_menu), dbbrowser);
  /* Search entry */
  g_signal_connect_swapped (priv->search_entry, "activate",
                            G_CALLBACK (cb_search_entry_activated), dbbrowser);
  g_signal_connect_swapped (priv->search_entry, "key-release-event",
                            G_CALLBACK (cb_search_entry_key_released), dbbrowser);
  g_signal_connect_swapped (priv->search_entry, "changed",
                            G_CALLBACK (cb_search_entry_changed), dbbrowser);
  /* Preferences */
  g_signal_connect_swapped (dbbrowser->preferences, "notify::song-format",
                            G_CALLBACK (xfmpc_dbbrowser_reload), dbbrowser);
  g_signal_connect_swapped (dbbrowser->preferences, "notify::song-format-custom",
                            G_CALLBACK (xfmpc_dbbrowser_reload), dbbrowser);
}

static void
xfmpc_dbbrowser_dispose (GObject *object)
{
  (*G_OBJECT_CLASS (parent_class)->dispose) (object);
}

static void
xfmpc_dbbrowser_finalize (GObject *object)
{
  XfmpcDbbrowser *dbbrowser = XFMPC_DBBROWSER (object);
  XfmpcDbbrowserPrivate *priv = XFMPC_DBBROWSER (dbbrowser)->priv;

  g_object_set (G_OBJECT (dbbrowser->preferences),
                "dbbrowser-last-path", priv->wdir,
                NULL);

  g_object_unref (G_OBJECT (dbbrowser->preferences));
  g_object_unref (G_OBJECT (dbbrowser->mpdclient));
  (*G_OBJECT_CLASS (parent_class)->finalize) (object);
}



GtkWidget*
xfmpc_dbbrowser_new ()
{
  return g_object_new (XFMPC_TYPE_DBBROWSER, NULL);
}

void
xfmpc_dbbrowser_clear (XfmpcDbbrowser *dbbrowser)
{
  XfmpcDbbrowserPrivate *priv = XFMPC_DBBROWSER (dbbrowser)->priv;
  gtk_list_store_clear (priv->store);
}

void
xfmpc_dbbrowser_append (XfmpcDbbrowser *dbbrowser,
                        gchar *filename,
                        gchar *basename,
                        gboolean is_dir,
                        gboolean is_bold)
{
  XfmpcDbbrowserPrivate    *priv = XFMPC_DBBROWSER (dbbrowser)->priv;
  GdkPixbuf                *pixbuf;
  GtkTreeIter               iter;

  pixbuf = gtk_widget_render_icon (priv->treeview,
                                   is_dir ? GTK_STOCK_DIRECTORY : GTK_STOCK_FILE,
                                   GTK_ICON_SIZE_MENU,
                                   NULL);

  gtk_list_store_append (priv->store, &iter);
  gtk_list_store_set (priv->store, &iter,
                      COLUMN_PIXBUF, pixbuf,
                      COLUMN_FILENAME, filename,
                      COLUMN_BASENAME, basename,
                      COLUMN_IS_DIR, is_dir,
                      COLUMN_WEIGHT, is_bold ? PANGO_WEIGHT_BOLD : PANGO_WEIGHT_NORMAL,
                      -1);
}

void
xfmpc_dbbrowser_add_selected_rows (XfmpcDbbrowser *dbbrowser)
{
  XfmpcDbbrowserPrivate *priv = XFMPC_DBBROWSER (dbbrowser)->priv;
  GtkTreeModel         *store = GTK_TREE_MODEL (priv->store);
  GtkTreeIter           iter;
  GList                *l, *list;
  gchar                *filename;

  list = gtk_tree_selection_get_selected_rows (gtk_tree_view_get_selection (GTK_TREE_VIEW (priv->treeview)), &store);

  for (l = list; l != NULL; l = l->next)
    {
      if (gtk_tree_model_get_iter (store, &iter, l->data))
        {       
          gtk_tree_model_get (store, &iter,
                              COLUMN_FILENAME, &filename,
                              -1);
          xfmpc_mpdclient_queue_add (dbbrowser->mpdclient, filename);
          g_free (filename);
        }
    }

  xfmpc_mpdclient_queue_commit (dbbrowser->mpdclient);

  g_list_foreach (list, (GFunc)gtk_tree_path_free, NULL);
  g_list_free (list);
}

void
xfmpc_dbbrowser_replace_with_selected_rows (XfmpcDbbrowser *dbbrowser)
{
  xfmpc_mpdclient_queue_clear (dbbrowser->mpdclient);
  xfmpc_dbbrowser_add_selected_rows (dbbrowser);
}

void
xfmpc_dbbrowser_reload (XfmpcDbbrowser *dbbrowser)
{
  XfmpcDbbrowserPrivate    *priv = XFMPC_DBBROWSER (dbbrowser)->priv;
  gchar                    *filename;
  gchar                    *basename;
  gboolean                  is_dir;
  gboolean                  is_bold;
  gint                      i = 0;

  if (G_UNLIKELY (!xfmpc_mpdclient_is_connected (dbbrowser->mpdclient)))
    return;
  else if (priv->is_searching)
    return;

  xfmpc_dbbrowser_clear (dbbrowser);

  if (!xfmpc_dbbrowser_wdir_is_root (dbbrowser))
    {
      filename = xfmpc_dbbrowser_get_parent_wdir (dbbrowser);
      xfmpc_dbbrowser_append (dbbrowser, filename, "..", TRUE, FALSE);
      g_free (filename);
      i++;
    }

  while (xfmpc_mpdclient_database_read (dbbrowser->mpdclient, priv->wdir,
                                        &filename, &basename, &is_dir))
    {
      is_bold = xfmpc_mpdclient_playlist_has_filename (dbbrowser->mpdclient, filename, is_dir);
      xfmpc_dbbrowser_append (dbbrowser, filename, basename, is_dir, is_bold);

      if (i >= 0)
        {
          if (0 == g_ascii_strcasecmp (filename, priv->last_wdir))
            {
              GtkTreePath *path = gtk_tree_path_new_from_indices (i, -1);
              gtk_tree_view_set_cursor (GTK_TREE_VIEW (priv->treeview), path, NULL, FALSE);
              gtk_tree_view_scroll_to_cell (GTK_TREE_VIEW (priv->treeview), path, NULL, TRUE, 0.10, 0);
              gtk_tree_path_free (path);

              i = -1;
            }
          else
            i++;
        }

      g_free (filename);
      g_free (basename);
    }
}

void
xfmpc_dbbrowser_search (XfmpcDbbrowser *dbbrowser,
                        const gchar *query)
{
  XfmpcDbbrowserPrivate    *priv = XFMPC_DBBROWSER (dbbrowser)->priv;
  gchar                    *filename;
  gchar                    *basename;
  gboolean                  is_bold;
  gint                      i = 0;
  static gboolean           no_result, no_result_buf;
  GdkColor                  color = {0, 0xFFFF, 0x6666, 0x6666};

  if (G_UNLIKELY (!xfmpc_mpdclient_is_connected (dbbrowser->mpdclient)))
    return;

  priv->is_searching = TRUE;
  xfmpc_dbbrowser_clear (dbbrowser);

  while (xfmpc_mpdclient_database_search (dbbrowser->mpdclient, query, &filename, &basename))
    {
      is_bold = xfmpc_mpdclient_playlist_has_filename (dbbrowser->mpdclient, filename, 0);
      xfmpc_dbbrowser_append (dbbrowser, filename, basename, FALSE, is_bold);
      g_free (filename);
      g_free (basename);
      i++;
    }

  /* check the rightness of the query and then play with the colors */
  no_result_buf = no_result;
  if (i == 0)
    no_result = TRUE;
  else if (no_result)
    no_result = FALSE;

  if (no_result != no_result_buf)
    {
#ifdef MORE_FUNKY_COLOR_ON_SEARCH_ENTRY
      gtk_widget_modify_base (priv->search_entry, GTK_STATE_NORMAL, no_result ? &color : NULL);
#endif
      gtk_widget_modify_bg (priv->search_entry, GTK_STATE_NORMAL, no_result ? &color : NULL);
      gtk_widget_modify_bg (priv->search_entry, GTK_STATE_SELECTED, no_result ? &color : NULL);
    }
  if (i == 0)
    {
#ifdef MORE_FUNKY_COLOR_ON_SEARCH_ENTRY
      gtk_widget_modify_base (priv->search_entry, GTK_STATE_NORMAL, &color);
#endif
      gtk_widget_modify_bg (priv->search_entry, GTK_STATE_NORMAL, &color);
      gtk_widget_modify_bg (priv->search_entry, GTK_STATE_SELECTED, &color);
    }
  else if (no_result)
    {
#ifdef MORE_FUNKY_COLOR_ON_SEARCH_ENTRY
      gtk_widget_modify_base (priv->search_entry, GTK_STATE_NORMAL, NULL);
#endif
      gtk_widget_modify_bg (priv->search_entry, GTK_STATE_NORMAL, NULL);
      gtk_widget_modify_bg (priv->search_entry, GTK_STATE_SELECTED, NULL);
    }
}



void
xfmpc_dbbrowser_set_wdir (XfmpcDbbrowser *dbbrowser,
                          const gchar *dir)
{
  XfmpcDbbrowserPrivate *priv = XFMPC_DBBROWSER (dbbrowser)->priv;

  g_free (priv->last_wdir);
  priv->last_wdir = priv->wdir;
  priv->wdir = g_strdup (dir);
}

gboolean
xfmpc_dbbrowser_wdir_is_root (XfmpcDbbrowser *dbbrowser)
{
  XfmpcDbbrowserPrivate *priv = XFMPC_DBBROWSER (dbbrowser)->priv;
  return priv->wdir[0] == '\0';
}

gchar *
xfmpc_dbbrowser_get_parent_wdir (XfmpcDbbrowser *dbbrowser)
{
  XfmpcDbbrowserPrivate    *priv = XFMPC_DBBROWSER (dbbrowser)->priv;
  gchar                    *filename;

  filename = g_strrstr (priv->wdir, "/");
  if (NULL == filename)
    filename = g_strdup ("");
  else
    filename = g_strndup (priv->wdir, filename - priv->wdir);

  return filename;
}



static void
cb_row_activated (XfmpcDbbrowser *dbbrowser,
                  GtkTreePath *path,
                  GtkTreeViewColumn *column)
{
  XfmpcDbbrowserPrivate    *priv = XFMPC_DBBROWSER (dbbrowser)->priv;
  GtkTreeIter               iter;
  gchar                    *filename;
  gboolean                  is_dir;

  if (!gtk_tree_model_get_iter (GTK_TREE_MODEL (priv->store), &iter, path))
    return;

  gtk_tree_model_get (GTK_TREE_MODEL (priv->store), &iter,
                      COLUMN_FILENAME, &filename,
                      COLUMN_IS_DIR, &is_dir,
                      -1);

  if (is_dir)
    {
      xfmpc_dbbrowser_set_wdir (dbbrowser, filename);
      xfmpc_dbbrowser_reload (dbbrowser);
    }
  else
    {
      xfmpc_mpdclient_queue_add (dbbrowser->mpdclient, filename);
      xfmpc_mpdclient_queue_commit (dbbrowser->mpdclient);
    }

  g_free (filename);
}

static gboolean
cb_key_pressed (XfmpcDbbrowser *dbbrowser,
                GdkEventKey *event)
{
  XfmpcDbbrowserPrivate    *priv = XFMPC_DBBROWSER (dbbrowser)->priv;
  GtkTreeSelection         *selection;
  gchar                    *filename;

  if (event->type != GDK_KEY_PRESS)
    return FALSE;

  switch (event->keyval)
    {
    case GDK_Return:
      selection = gtk_tree_view_get_selection (GTK_TREE_VIEW (priv->treeview));
      if (gtk_tree_selection_count_selected_rows (selection) > 1)
        xfmpc_dbbrowser_add_selected_rows (dbbrowser);
      else
        return FALSE;
      break;

    case GDK_BackSpace:
      filename = xfmpc_dbbrowser_get_parent_wdir (dbbrowser);
      xfmpc_dbbrowser_set_wdir (dbbrowser, filename);
      g_free (filename);
      xfmpc_dbbrowser_reload (dbbrowser);
      break;

    default:
      return FALSE;
    }

  return TRUE;
}

static gboolean
cb_button_pressed (XfmpcDbbrowser *dbbrowser,
                   GdkEventButton *event)
{
  XfmpcDbbrowserPrivate    *priv = XFMPC_DBBROWSER (dbbrowser)->priv;
  GtkTreeSelection         *selection;
  GtkTreePath              *path;
  
  if (event->type != GDK_BUTTON_PRESS || event->button != 3)
    return FALSE;

  selection = gtk_tree_view_get_selection (GTK_TREE_VIEW (priv->treeview));
  if (gtk_tree_view_get_path_at_pos (GTK_TREE_VIEW (priv->treeview),
                                     event->x, event->y,
                                     &path, NULL, NULL, NULL))
    {
      if (!gtk_tree_selection_path_is_selected (selection, path))
        {
          gtk_tree_selection_unselect_all (selection);
          gtk_tree_selection_select_path (selection, path);
        }
      gtk_tree_path_free (path);
    }

  popup_menu (dbbrowser);

  return TRUE;
}

static gboolean
cb_popup_menu (XfmpcDbbrowser *dbbrowser)
{
  popup_menu (dbbrowser);
  return TRUE;
}

static void
popup_menu (XfmpcDbbrowser *dbbrowser)
{
  XfmpcDbbrowserPrivate *priv = XFMPC_DBBROWSER (dbbrowser)->priv;
  GtkTreeSelection      *selection;
  gint                   count;

  selection = gtk_tree_view_get_selection (GTK_TREE_VIEW (priv->treeview));
  count = gtk_tree_selection_count_selected_rows (selection);

  if (priv->is_searching)
    {
      gtk_widget_show (priv->mi_browse);
      gtk_widget_set_sensitive (priv->mi_browse, count == 1 ? TRUE : FALSE);
    }
  else
    gtk_widget_hide (priv->mi_browse);

  gtk_menu_popup (GTK_MENU (priv->menu),
                  NULL, NULL,
                  NULL, NULL,
                  0,
                  gtk_get_current_event_time ());
}

static void
cb_browse (XfmpcDbbrowser *dbbrowser)
{
  XfmpcDbbrowserPrivate *priv = XFMPC_DBBROWSER (dbbrowser)->priv;
  GtkTreeSelection      *selection;
  GtkTreeModel          *store = GTK_TREE_MODEL (priv->store);
  GtkTreeIter            iter;
  GList                 *list;
  gchar                 *dir, *filename;

  selection = gtk_tree_view_get_selection (GTK_TREE_VIEW (priv->treeview));
  if (gtk_tree_selection_count_selected_rows (selection) > 1)
    return;

  list = gtk_tree_selection_get_selected_rows (selection, &store);
  if (gtk_tree_model_get_iter (store, &iter, list->data))
    {
      gtk_tree_model_get (store, &iter,
                          COLUMN_FILENAME, &filename,
                          -1);

      priv->is_searching = FALSE;
      dir = g_path_get_dirname (filename);
      xfmpc_dbbrowser_set_wdir (dbbrowser, dir);
      xfmpc_dbbrowser_reload (dbbrowser);

      g_free (filename);
      g_free (dir);
    }

  g_list_foreach (list, (GFunc)gtk_tree_path_free, NULL);
  g_list_free (list);
}



static void
cb_search_entry_activated (XfmpcDbbrowser *dbbrowser)
{
  XfmpcDbbrowserPrivate    *priv = XFMPC_DBBROWSER (dbbrowser)->priv;
  const gchar              *entry_text = gtk_entry_get_text (GTK_ENTRY (priv->search_entry));

  if (entry_text[0] == '\0')
    {
      priv->is_searching = FALSE;
      xfmpc_dbbrowser_reload (dbbrowser);

      /* revert possible previous applied color */
#ifdef MORE_FUNKY_COLOR_ON_SEARCH_ENTRY
      gtk_widget_modify_base (priv->search_entry, GTK_STATE_NORMAL, NULL);
#endif
      gtk_widget_modify_bg (priv->search_entry, GTK_STATE_NORMAL, NULL);
      gtk_widget_modify_bg (priv->search_entry, GTK_STATE_SELECTED, NULL);
      return;
    }

  xfmpc_dbbrowser_search (dbbrowser, entry_text);
}

static gboolean
cb_search_entry_key_released (XfmpcDbbrowser *dbbrowser,
                              GdkEventKey *event)
{
  XfmpcDbbrowserPrivate *priv = XFMPC_DBBROWSER (dbbrowser)->priv;

  if (event->type != GDK_KEY_RELEASE)
    return FALSE;

  if (event->keyval == GDK_Escape)
    {
      gtk_entry_set_text (GTK_ENTRY (priv->search_entry), "");
    }

  return TRUE;
}

static void
cb_search_entry_changed (XfmpcDbbrowser *dbbrowser)
{
  XfmpcDbbrowserPrivate *priv = XFMPC_DBBROWSER (dbbrowser)->priv;

  if (priv->search_timeout > 0)
    g_source_remove (priv->search_timeout);

  priv->search_timeout = g_timeout_add_full (G_PRIORITY_DEFAULT, 642,
                                             (GSourceFunc)timeout_search, dbbrowser,
                                             (GDestroyNotify)timeout_search_destroy);
}

static gboolean
timeout_search (XfmpcDbbrowser *dbbrowser)
{
  cb_search_entry_activated (dbbrowser);
  return FALSE;
}

static void
timeout_search_destroy (XfmpcDbbrowser *dbbrowser)
{
  XfmpcDbbrowserPrivate *priv = XFMPC_DBBROWSER (dbbrowser)->priv;
  priv->search_timeout = 0;
}



static void
cb_playlist_changed (XfmpcDbbrowser *dbbrowser)
{
  XfmpcDbbrowserPrivate *priv = XFMPC_DBBROWSER (dbbrowser)->priv;
  GtkTreeModel         *store = GTK_TREE_MODEL (priv->store);
  GtkTreeIter           iter;
  gchar                *filename;
  gboolean              is_bold;
  gboolean              is_dir;

  if (!gtk_tree_model_get_iter_first (store, &iter))
    return;

  do
    {
      gtk_tree_model_get (store, &iter,
                          COLUMN_FILENAME, &filename,
                          COLUMN_IS_DIR, &is_dir,
                          -1);

      is_bold = xfmpc_mpdclient_playlist_has_filename (dbbrowser->mpdclient, filename, is_dir);
      gtk_list_store_set (GTK_LIST_STORE (store), &iter,
                          COLUMN_WEIGHT, is_bold ? PANGO_WEIGHT_BOLD : PANGO_WEIGHT_NORMAL,
                          -1);

      g_free (filename);
    }
  while (gtk_tree_model_iter_next (store, &iter));
}

