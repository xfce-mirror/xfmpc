/*
 *  Copyright (c) 2008-2009 Mike Massonnet <mmassonnet@xfce.org>
 *  Copyright (c) 2009 Vincent Legout <vincent@xfce.org>
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

#include "playlist.h"
#include "preferences.h"
#include "mpdclient.h"
#include "dbbrowser.h"
#include "extended-interface.h"
#include "song-dialog.h"

#define BORDER 4

#define GET_PRIVATE(o) \
    (G_TYPE_INSTANCE_GET_PRIVATE ((o), XFMPC_TYPE_PLAYLIST, XfmpcPlaylistPrivate))



static void             xfmpc_playlist_class_init               (XfmpcPlaylistClass *klass);
static void             xfmpc_playlist_init                     (XfmpcPlaylist *playlist);
static void             xfmpc_playlist_dispose                  (GObject *object);
static void             xfmpc_playlist_finalize                 (GObject *object);

static void             cb_song_changed                         (XfmpcPlaylist *playlist);
static void             cb_playlist_changed                     (XfmpcPlaylist *playlist);
static void             cb_row_activated                        (XfmpcPlaylist *playlist,
                                                                 GtkTreePath *path,
                                                                 GtkTreeViewColumn *column);
static gboolean         cb_key_released                         (XfmpcPlaylist *playlist,
                                                                 GdkEventKey *event);
static gboolean         cb_button_pressed                       (XfmpcPlaylist *playlist,
                                                                 GdkEventButton *event);
static gboolean         cb_button_released                      (XfmpcPlaylist *playlist,
                                                                 GdkEventButton *event);
static gboolean         cb_popup_menu                           (XfmpcPlaylist *playlist);
static void             popup_menu                              (XfmpcPlaylist *playlist);

static void             cb_filter_entry_activated               (XfmpcPlaylist *playlist);
static gboolean         cb_filter_entry_key_released            (XfmpcPlaylist *playlist,
                                                                 GdkEventKey *event);
static void             cb_filter_entry_changed                 (XfmpcPlaylist *playlist);
static gboolean         visible_func_filter_tree                (GtkTreeModel *filter,
                                                                 GtkTreeIter *iter,
                                                                 XfmpcPlaylist *playlist);

void                    cb_browse_selection                     (XfmpcPlaylist *playlist);
void                    cb_info_selection                       (XfmpcPlaylist *playlist);



/* List store identifiers */
enum
{
  COLUMN_ID,
  COLUMN_FILENAME,
  COLUMN_POSITION,
  COLUMN_SONG,
  COLUMN_LENGTH,
  COLUMN_WEIGHT,
  N_COLUMNS,
};



struct _XfmpcPlaylistClass
{
  GtkVBoxClass          parent_class;
};

struct _XfmpcPlaylist
{
  GtkVBox                   parent;
  XfmpcPreferences         *preferences;
  XfmpcMpdclient           *mpdclient;
  /*<private>*/
  XfmpcPlaylistPrivate     *priv;
};

struct _XfmpcPlaylistPrivate
{
  GtkWidget                *treeview;
  GtkTreeModelFilter       *filter;
  GtkListStore             *store;
  GtkWidget                *filter_entry;
  GtkWidget                *menu;
  GtkWidget                *mi_browse;
  GtkWidget                *mi_information;

  gint                      current;
  gboolean                  autocenter;
};



static GObjectClass *parent_class = NULL;



GType
xfmpc_playlist_get_type ()
{
  static GType xfmpc_playlist_type = G_TYPE_INVALID;

  if (G_UNLIKELY (xfmpc_playlist_type == G_TYPE_INVALID))
    {
      static const GTypeInfo xfmpc_playlist_info =
        {
          sizeof (XfmpcPlaylistClass),
          (GBaseInitFunc) NULL,
          (GBaseFinalizeFunc) NULL,
          (GClassInitFunc) xfmpc_playlist_class_init,
          (GClassFinalizeFunc) NULL,
          NULL,
          sizeof (XfmpcPlaylist),
          0,
          (GInstanceInitFunc) xfmpc_playlist_init,
          NULL
        };
      xfmpc_playlist_type = g_type_register_static (GTK_TYPE_VBOX, "XfmpcPlaylist", &xfmpc_playlist_info, 0);
    }

  return xfmpc_playlist_type;
}



static void
xfmpc_playlist_class_init (XfmpcPlaylistClass *klass)
{
  GObjectClass *gobject_class;

  g_type_class_add_private (klass, sizeof (XfmpcPlaylistPrivate));

  parent_class = g_type_class_peek_parent (klass);

  gobject_class = G_OBJECT_CLASS (klass);
  gobject_class->dispose = xfmpc_playlist_dispose;
  gobject_class->finalize = xfmpc_playlist_finalize;
}

static void
xfmpc_playlist_init (XfmpcPlaylist *playlist)
{
  XfmpcPlaylistPrivate *priv = playlist->priv = GET_PRIVATE (playlist);

  playlist->preferences = xfmpc_preferences_get ();
  playlist->mpdclient = xfmpc_mpdclient_get ();

  g_object_get (G_OBJECT (playlist->preferences),
                "playlist-autocenter", &priv->autocenter,
                NULL);

  /* === Tree model === */
  priv->store = gtk_list_store_new (N_COLUMNS,
                                    G_TYPE_INT,
                                    G_TYPE_STRING,
                                    G_TYPE_INT,
                                    G_TYPE_STRING,
                                    G_TYPE_STRING,
                                    G_TYPE_INT);

  /* === Filter === */
  priv->filter = GTK_TREE_MODEL_FILTER (gtk_tree_model_filter_new (GTK_TREE_MODEL (priv->store), NULL));
  g_object_unref (priv->store);
  gtk_tree_model_filter_set_visible_func (priv->filter,
                                          (GtkTreeModelFilterVisibleFunc)visible_func_filter_tree,
                                          playlist,
                                          NULL);

  /* === Tree view === */
  priv->treeview = gtk_tree_view_new ();
  gtk_tree_selection_set_mode (gtk_tree_view_get_selection (GTK_TREE_VIEW (priv->treeview)), GTK_SELECTION_MULTIPLE);
  gtk_tree_view_set_rubber_banding (GTK_TREE_VIEW (priv->treeview), TRUE);
  gtk_tree_view_set_enable_search (GTK_TREE_VIEW (priv->treeview), FALSE);
  gtk_tree_view_set_headers_visible (GTK_TREE_VIEW (priv->treeview), FALSE);
  gtk_tree_view_set_rules_hint (GTK_TREE_VIEW (priv->treeview), TRUE);
  gtk_tree_view_set_model (GTK_TREE_VIEW (priv->treeview), GTK_TREE_MODEL (priv->filter));
  g_object_unref (priv->filter);

  /* Column "position" */
  GtkCellRenderer *cell = gtk_cell_renderer_text_new ();
  g_object_set (G_OBJECT (cell),
                "xalign", 1.0,
                NULL);
  gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (priv->treeview),
                                               -1, "Length", cell,
                                               "text", COLUMN_POSITION,
                                               "weight", COLUMN_WEIGHT,
                                               NULL);

  /* Column "artist - title" */
  cell = gtk_cell_renderer_text_new ();
  g_object_set (G_OBJECT (cell),
                "ellipsize", PANGO_ELLIPSIZE_END,
                NULL);
  GtkTreeViewColumn *column =
    gtk_tree_view_column_new_with_attributes ("Song", cell,
                                              "text", COLUMN_SONG,
                                              "weight", COLUMN_WEIGHT,
                                              NULL);
  g_object_set (G_OBJECT (column),
                "expand", TRUE,
                NULL);
  gtk_tree_view_append_column (GTK_TREE_VIEW (priv->treeview), column);

  /* Column "length" */
  cell = gtk_cell_renderer_text_new ();
  g_object_set (G_OBJECT (cell),
                "xalign", 1.0,
                NULL);
  gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (priv->treeview),
                                               -1, "Length", cell,
                                               "text", COLUMN_LENGTH,
                                               "weight", COLUMN_WEIGHT,
                                               NULL);

  /* Scrolled window */
  GtkWidget *scrolled = gtk_scrolled_window_new (NULL, NULL);
  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolled),
                                  GTK_POLICY_AUTOMATIC,
                                  GTK_POLICY_ALWAYS);

  /* Menu */
  priv->menu = gtk_menu_new ();

  GtkWidget *mi = gtk_image_menu_item_new_from_stock (GTK_STOCK_REMOVE, NULL);
  gtk_menu_shell_append (GTK_MENU_SHELL (priv->menu), mi);
  g_signal_connect_swapped (mi, "activate",
                            G_CALLBACK (xfmpc_playlist_delete_selection), playlist);

  mi = priv->mi_browse = gtk_image_menu_item_new_with_mnemonic (_("_Browse"));
  gtk_menu_shell_append (GTK_MENU_SHELL (priv->menu), mi);
  g_signal_connect_swapped (mi, "activate",
                            G_CALLBACK (cb_browse_selection), playlist);
  GtkWidget *image = gtk_image_new_from_stock (GTK_STOCK_OPEN, GTK_ICON_SIZE_MENU);
  gtk_image_menu_item_set_image (GTK_IMAGE_MENU_ITEM (mi), image);

  mi = priv->mi_information = gtk_image_menu_item_new_from_stock (GTK_STOCK_INFO, NULL);
  gtk_menu_shell_append (GTK_MENU_SHELL (priv->menu), mi);
  g_signal_connect_swapped (mi, "activate",
                            G_CALLBACK (cb_info_selection), playlist);

  gtk_widget_show_all (priv->menu);

  /* === Filter entry === */
  priv->filter_entry = gtk_entry_new ();

  /* === Containers === */
  gtk_container_add (GTK_CONTAINER (scrolled), priv->treeview);
  gtk_box_pack_start (GTK_BOX (playlist), scrolled, TRUE, TRUE, 0);
  gtk_box_pack_start (GTK_BOX (playlist), priv->filter_entry, FALSE, FALSE, 0);

  /* === Signals === */
  g_signal_connect_swapped (playlist->mpdclient, "song-changed",
                            G_CALLBACK (cb_song_changed), playlist);
  g_signal_connect_swapped (playlist->mpdclient, "playlist-changed",
                            G_CALLBACK (cb_playlist_changed), playlist);
  /* Tree view */
  g_signal_connect_swapped (priv->treeview, "row-activated",
                            G_CALLBACK (cb_row_activated), playlist);
  g_signal_connect_swapped (priv->treeview, "key-release-event",
                            G_CALLBACK (cb_key_released), playlist);
  g_signal_connect_swapped (priv->treeview, "button-press-event",
                            G_CALLBACK (cb_button_pressed), playlist);
  g_signal_connect_swapped (priv->treeview, "button-release-event",
                            G_CALLBACK (cb_button_released), playlist);
  g_signal_connect_swapped (priv->treeview, "popup-menu",
                            G_CALLBACK (cb_popup_menu), playlist);
  /* Filter entry */
  g_signal_connect_swapped (priv->filter_entry, "activate",
                            G_CALLBACK (cb_filter_entry_activated), playlist);
  g_signal_connect_swapped (priv->filter_entry, "key-release-event",
                            G_CALLBACK (cb_filter_entry_key_released), playlist);
  g_signal_connect_swapped (priv->filter_entry, "changed",
                            G_CALLBACK (cb_filter_entry_changed), playlist);

  /* Preferences */
  g_signal_connect_swapped (playlist->preferences, "notify::song-format",
                            G_CALLBACK (cb_playlist_changed), playlist);
  g_signal_connect_swapped (playlist->preferences, "notify::song-format-custom",
                            G_CALLBACK (cb_playlist_changed), playlist);
}

static void
xfmpc_playlist_dispose (GObject *object)
{
  (*G_OBJECT_CLASS (parent_class)->dispose) (object);
}

static void
xfmpc_playlist_finalize (GObject *object)
{
  XfmpcPlaylist *playlist = XFMPC_PLAYLIST (object);
  g_object_unref (G_OBJECT (playlist->mpdclient));
  (*G_OBJECT_CLASS (parent_class)->finalize) (object);
}



GtkWidget*
xfmpc_playlist_new ()
{
  return g_object_new (XFMPC_TYPE_PLAYLIST, NULL);
}

void
xfmpc_playlist_append (XfmpcPlaylist *playlist,
                       gint id,
                       gint pos,
                       gchar *filename,
                       gchar *song,
                       gchar *length)
{
  XfmpcPlaylistPrivate *priv = XFMPC_PLAYLIST (playlist)->priv;
  GtkTreeIter           iter;

  gtk_list_store_append (priv->store, &iter);
  gtk_list_store_set (priv->store, &iter,
                      COLUMN_ID, id,
                      COLUMN_FILENAME, filename,
                      COLUMN_POSITION, pos + 1,
                      COLUMN_SONG, song,
                      COLUMN_LENGTH, length,
                      COLUMN_WEIGHT, PANGO_WEIGHT_NORMAL,
                      -1);
}

void
xfmpc_playlist_clear (XfmpcPlaylist *playlist)
{
  XfmpcPlaylistPrivate *priv = XFMPC_PLAYLIST (playlist)->priv;
  gtk_list_store_clear (priv->store);
}

void
xfmpc_playlist_delete_selection (XfmpcPlaylist *playlist)
{
  XfmpcPlaylistPrivate *priv = XFMPC_PLAYLIST (playlist)->priv;
  GtkTreeModel         *store = GTK_TREE_MODEL (priv->store);
  GtkTreeIter           iter;
  GList                *list;
  gint                  id;

  list = gtk_tree_selection_get_selected_rows (gtk_tree_view_get_selection (GTK_TREE_VIEW (priv->treeview)),
                                               &store);
  while (NULL != list)
    {
      if (gtk_tree_model_get_iter (store, &iter, list->data))
        {       
          gtk_tree_model_get (store, &iter,
                              COLUMN_ID, &id,
                              -1);
          xfmpc_mpdclient_queue_remove_id (playlist->mpdclient, id);
        }
      list = g_list_next (list);
    }

  xfmpc_mpdclient_queue_commit (playlist->mpdclient);

  g_list_foreach (list, (GFunc)gtk_tree_path_free, NULL);
  g_list_free (list);
}

void
xfmpc_playlist_select_row (XfmpcPlaylist *playlist,
                           gint i)
{
  XfmpcPlaylistPrivate *priv = XFMPC_PLAYLIST (playlist)->priv;

  if (gtk_tree_model_iter_n_children (GTK_TREE_MODEL (priv->filter), NULL) == 0)
    return;

  GtkTreePath *path = gtk_tree_path_new_from_indices (i, -1);
  gtk_tree_view_set_cursor (GTK_TREE_VIEW (priv->treeview), path, NULL, FALSE);
  gtk_tree_view_scroll_to_cell (GTK_TREE_VIEW (priv->treeview), path, NULL, TRUE, 0.42, 0);
  gtk_tree_path_free (path);
}

void
xfmpc_playlist_refresh_current_song (XfmpcPlaylist *playlist)
{
  XfmpcPlaylistPrivate *priv = XFMPC_PLAYLIST (playlist)->priv;

  /* Remove the bold from the "last" current song */
  GtkTreeIter iter;
  GtkTreePath *path = gtk_tree_path_new_from_indices (priv->current, -1);
  if (gtk_tree_model_get_iter (GTK_TREE_MODEL (priv->store), &iter, path))
    gtk_list_store_set (priv->store, &iter,
                        COLUMN_WEIGHT, PANGO_WEIGHT_NORMAL,
                        -1);

  /* Set the current song bold */
  priv->current = xfmpc_mpdclient_get_pos (playlist->mpdclient);
  path = gtk_tree_path_new_from_indices (priv->current, -1);
  if (gtk_tree_model_get_iter (GTK_TREE_MODEL (priv->store), &iter, path))
    gtk_list_store_set (priv->store, &iter,
                        COLUMN_WEIGHT, PANGO_WEIGHT_BOLD,
                        -1);
}



static void
cb_song_changed (XfmpcPlaylist *playlist)
{
  XfmpcPlaylistPrivate *priv = XFMPC_PLAYLIST (playlist)->priv;

  xfmpc_playlist_refresh_current_song (playlist);

  /* don't autocenter if a filter is typped in */
  if (gtk_entry_get_text (GTK_ENTRY (priv->filter_entry))[0] == '\0' && priv->autocenter)
    xfmpc_playlist_select_row (playlist, priv->current);
}

static void
cb_playlist_changed (XfmpcPlaylist *playlist)
{
  XfmpcPlaylistPrivate *priv = XFMPC_PLAYLIST (playlist)->priv;
  gchar                *filename, *song, *length;
  gint                  id, pos, current;

  current = xfmpc_mpdclient_get_id (playlist->mpdclient);

  xfmpc_playlist_clear (playlist);
  while (xfmpc_mpdclient_playlist_read (playlist->mpdclient, &id, &pos, &filename, &song, &length))
    {
      xfmpc_playlist_append (playlist, id, pos, filename, song, length);
      g_free (filename);
      g_free (song);
      g_free (length);
    }

  xfmpc_playlist_refresh_current_song (playlist);

  /* don't autocenter if a filter is typped in */
  if (gtk_entry_get_text (GTK_ENTRY (priv->filter_entry))[0] == '\0' && priv->autocenter)
    xfmpc_playlist_select_row (playlist, priv->current);
}

static void
cb_row_activated (XfmpcPlaylist *playlist,
                  GtkTreePath *path,
                  GtkTreeViewColumn *column)
{
  XfmpcPlaylistPrivate *priv = XFMPC_PLAYLIST (playlist)->priv;
  GtkTreeIter           iter;
  gint                  id;

  if (!gtk_tree_model_get_iter (GTK_TREE_MODEL (priv->filter), &iter, path))
    return;

  gtk_tree_model_get (GTK_TREE_MODEL (priv->filter), &iter,
                      COLUMN_ID, &id,
                      -1);
  xfmpc_mpdclient_set_id (playlist->mpdclient, id);
}

static gboolean
cb_key_released (XfmpcPlaylist *playlist,
                 GdkEventKey *event)
{
  if (event->type != GDK_KEY_RELEASE)
    return FALSE;

  switch (event->keyval)
    {
    case GDK_Delete:
      xfmpc_playlist_delete_selection (playlist);
      break;

    default:
      return FALSE;
    }

  return TRUE;
}

static gboolean
cb_button_pressed (XfmpcPlaylist *playlist,
                   GdkEventButton *event)
{
  /* Avoid multiple selections from getting lost for button 3 */
  return event->button == 3 ? TRUE : FALSE;
}

static gboolean
cb_button_released (XfmpcPlaylist *playlist,
                    GdkEventButton *event)
{
  XfmpcPlaylistPrivate *priv = XFMPC_PLAYLIST (playlist)->priv;
  GtkTreeSelection     *selection;
  GtkTreePath          *path;
  gboolean              sensitive;
  
  if (event->type != GDK_BUTTON_RELEASE || event->button != 3)
    return FALSE;

  selection = gtk_tree_view_get_selection (GTK_TREE_VIEW (priv->treeview));
  if (gtk_tree_selection_count_selected_rows (selection) < 1)
    return TRUE;

  sensitive = gtk_tree_selection_count_selected_rows (selection) == 1;
  gtk_widget_set_sensitive (priv->mi_browse, sensitive);
  gtk_widget_set_sensitive (priv->mi_information, sensitive);

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

  popup_menu (playlist);
  return TRUE;
}

static gboolean
cb_popup_menu (XfmpcPlaylist *playlist)
{
  popup_menu (playlist);
  return TRUE;
}

static void
popup_menu (XfmpcPlaylist *playlist)
{
  XfmpcPlaylistPrivate *priv = XFMPC_PLAYLIST (playlist)->priv;
  gtk_menu_popup (GTK_MENU (priv->menu),
                  NULL, NULL,
                  NULL, NULL,
                  0,
                  gtk_get_current_event_time ());
}



static void
cb_filter_entry_activated (XfmpcPlaylist *playlist)
{
  XfmpcPlaylistPrivate *priv = XFMPC_PLAYLIST (playlist)->priv;
  GtkTreeModel         *model = GTK_TREE_MODEL (priv->filter);
  GList                *list;

  list = gtk_tree_selection_get_selected_rows (gtk_tree_view_get_selection (GTK_TREE_VIEW (priv->treeview)), &model);
  if (G_LIKELY (NULL != list))
    {
      gtk_tree_view_row_activated (GTK_TREE_VIEW (priv->treeview), list->data, NULL);
      gtk_entry_set_text (GTK_ENTRY (priv->filter_entry), "");
      xfmpc_playlist_select_row (playlist, priv->current);
      gtk_widget_grab_focus (priv->treeview);
      g_list_foreach (list, (GFunc)gtk_tree_path_free, NULL);
      g_list_free (list);
    }
}

static gboolean
cb_filter_entry_key_released (XfmpcPlaylist *playlist,
                              GdkEventKey *event)
{
  XfmpcPlaylistPrivate *priv = XFMPC_PLAYLIST (playlist)->priv;

  if (event->type != GDK_KEY_RELEASE)
    return FALSE;

  if (event->keyval == GDK_Escape)
    {
      gtk_entry_set_text (GTK_ENTRY (priv->filter_entry), "");
      xfmpc_playlist_select_row (playlist, priv->current);
      gtk_widget_grab_focus (priv->treeview);
    }
  else 
    {
      if (gtk_entry_get_text (GTK_ENTRY (priv->filter_entry))[0] != '\0')
        xfmpc_playlist_select_row (playlist, 0);
      else
        xfmpc_playlist_select_row (playlist, priv->current);
    }

  return TRUE;
}

static void
cb_filter_entry_changed (XfmpcPlaylist *playlist)
{
  XfmpcPlaylistPrivate *priv = XFMPC_PLAYLIST (playlist)->priv;
  gtk_tree_model_filter_refilter (priv->filter);
}

static gboolean
visible_func_filter_tree (GtkTreeModel *filter,
                          GtkTreeIter *iter,
                          XfmpcPlaylist *playlist)
{
  XfmpcPlaylistPrivate *priv = XFMPC_PLAYLIST (playlist)->priv;
  gchar                *song, *song_tmp;
  gchar                *search;
  gboolean              result = TRUE;

  gtk_tree_model_get (GTK_TREE_MODEL (filter), iter,
                      COLUMN_SONG, &song,
                      -1);
  if (G_UNLIKELY (NULL == song))
    return TRUE;

  search = g_utf8_casefold (gtk_entry_get_text (GTK_ENTRY (priv->filter_entry)), -1);
  g_return_val_if_fail (G_LIKELY (NULL != search), TRUE);

  if (G_UNLIKELY (search[0] != '\0'))
    {
      song_tmp = g_utf8_casefold (song, -1);
      g_free (song);
      song = song_tmp;

      if (NULL == g_strrstr (song, search))
        result = FALSE;
    }

  g_free (song);
  g_free (search);

  return result;
}

void
cb_browse_selection (XfmpcPlaylist *playlist)
{
  XfmpcPlaylistPrivate   *priv = XFMPC_PLAYLIST (playlist)->priv;
  XfmpcDbbrowser         *dbbrowser;
  XfmpcExtendedInterface *extended_interface;
  GtkTreeModel           *store = GTK_TREE_MODEL (priv->store);
  GtkTreeSelection       *selection;
  GtkTreeIter             iter;
  GList                  *list;
  gchar                  *filename, *dir;

  dbbrowser = g_object_get_data (G_OBJECT (playlist), "XfmpcDbbrowser");
  extended_interface = g_object_get_data (G_OBJECT (playlist), "XfmpcExtendedInterface");

  selection = gtk_tree_view_get_selection (GTK_TREE_VIEW (priv->treeview));
  if (gtk_tree_selection_count_selected_rows (selection) > 1)
    return;

  list = gtk_tree_selection_get_selected_rows (selection, &store);
  if (list == NULL)
    return;

  if (gtk_tree_model_get_iter (store, &iter, list->data))
    {
      gtk_tree_model_get (store, &iter,
                          COLUMN_FILENAME, &filename,
                          -1);

      dir = g_path_get_dirname (filename);
      xfmpc_dbbrowser_set_wdir (dbbrowser, dir);
      xfmpc_dbbrowser_reload (dbbrowser);
      xfmpc_extended_interface_set_active (extended_interface, XFMPC_EXTENDED_INTERFACE_DBBROWSER);

      g_free (filename);
      g_free (dir);
    }

  g_list_foreach (list, (GFunc)gtk_tree_path_free, NULL);
  g_list_free (list);
}

void
cb_info_selection (XfmpcPlaylist *playlist)
{
  XfmpcPlaylistPrivate   *priv = XFMPC_PLAYLIST (playlist)->priv;

  GtkTreeModel           *store = GTK_TREE_MODEL (priv->store);
  GtkTreeSelection       *selection;
  GtkTreeIter             iter;
  GList                  *list;
  gint                    id;

  selection = gtk_tree_view_get_selection (GTK_TREE_VIEW (priv->treeview));
  if (gtk_tree_selection_count_selected_rows (selection) > 1)
    return;

  list = gtk_tree_selection_get_selected_rows (selection, &store);
  if (list == NULL)
    return;

  if (gtk_tree_model_get_iter (store, &iter, list->data))
    {
      gtk_tree_model_get (store, &iter,
                          COLUMN_ID, &id,
                          -1);

      GtkWidget *dialog = xfmpc_song_dialog_new (id);
      gtk_widget_show (dialog);
    }

  g_list_foreach (list, (GFunc)gtk_tree_path_free, NULL);
  g_list_free (list);
}

