/*
 *  Copyright (c) 2008 Mike Massonnet <mmassonnet@xfce.org>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Library General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <gtk/gtk.h>
#include <libxfce4util/libxfce4util.h>

#include "dbbrowser.h"
#include "preferences.h"
#include "mpdclient.h"

#define BORDER 4

#define XFMPC_DBBROWSER_GET_PRIVATE(o) \
    (G_TYPE_INSTANCE_GET_PRIVATE ((o), XFMPC_TYPE_DBBROWSER, XfmpcDbbrowserPrivate))



static void             xfmpc_dbbrowser_class_init             (XfmpcDbbrowserClass *klass);
static void             xfmpc_dbbrowser_init                   (XfmpcDbbrowser *dbbrowser);
static void             xfmpc_dbbrowser_dispose                (GObject *object);
static void             xfmpc_dbbrowser_finalize               (GObject *object);

static void             cb_row_activated                       (XfmpcDbbrowser *dbbrowser,
                                                                GtkTreePath *path,
                                                                GtkTreeViewColumn *column);



/* List store identifiers */
enum
{
  COLUMN_ID,
  COLUMN_PIXBUF,
  COLUMN_FILENAME,
  COLUMN_BASENAME,
  COLUMN_IS_DIR,
  N_COLUMNS,
};



struct _XfmpcDbbrowserClass
{
  GtkVBoxClass              parent_class;
};

struct _XfmpcDbbrowser
{
  GtkVBox                   parent;
  XfmpcDbbrowserPrivate    *priv;
  XfmpcPreferences         *preferences;
  XfmpcMpdclient           *mpdclient;
};

struct _XfmpcDbbrowserPrivate
{
  GtkWidget                *treeview;
  GtkListStore             *store;
  GtkWidget                *search_entry;

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
  XfmpcDbbrowserPrivate *priv = XFMPC_DBBROWSER_GET_PRIVATE (dbbrowser);

  dbbrowser->preferences = xfmpc_preferences_get ();
  dbbrowser->mpdclient = xfmpc_mpdclient_new ();

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
                                    G_TYPE_BOOLEAN);

  /* === Tree view === */
  priv->treeview = gtk_tree_view_new ();
  gtk_tree_selection_set_mode (gtk_tree_view_get_selection (GTK_TREE_VIEW (priv->treeview)), GTK_SELECTION_MULTIPLE);
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
                                               NULL);

  /* === Scrolled window === */
  GtkWidget *scrolled = gtk_scrolled_window_new (NULL, NULL);
  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolled),
                                  GTK_POLICY_AUTOMATIC,
                                  GTK_POLICY_ALWAYS);

  /* === Containers === */
  gtk_container_add (GTK_CONTAINER (scrolled), priv->treeview);
  gtk_container_add (GTK_CONTAINER (dbbrowser), scrolled);

  /* === Signals === */
  g_signal_connect_swapped (dbbrowser->mpdclient, "connected",
                            G_CALLBACK (xfmpc_dbbrowser_reload), dbbrowser);
#if 0
  g_signal_connect_swapped (dbbrowser->mpdclient, "database-changed",
                            G_CALLBACK (cb_database_changed), dbbrowser);
#endif
  /* Tree view */
  g_signal_connect_swapped (priv->treeview, "row-activated",
                            G_CALLBACK (cb_row_activated), dbbrowser);
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
  XfmpcDbbrowserPrivate *priv = XFMPC_DBBROWSER_GET_PRIVATE (dbbrowser);

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
  XfmpcDbbrowserPrivate    *priv = XFMPC_DBBROWSER_GET_PRIVATE (dbbrowser);

  gtk_list_store_clear (priv->store);
}

void
xfmpc_dbbrowser_append (XfmpcDbbrowser *dbbrowser,
                        gchar *filename,
                        gchar *basename,
                        gboolean is_dir)
{
  XfmpcDbbrowserPrivate    *priv = XFMPC_DBBROWSER_GET_PRIVATE (dbbrowser);
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
                      -1);
}

void
xfmpc_dbbrowser_reload (XfmpcDbbrowser *dbbrowser)
{
  XfmpcDbbrowserPrivate    *priv = XFMPC_DBBROWSER_GET_PRIVATE (dbbrowser);
  gchar                    *filename;
  gchar                    *basename;
  gboolean                  is_dir;
  gint                      i = 0;

  xfmpc_dbbrowser_clear (dbbrowser);

  if (!xfmpc_dbbrowser_wdir_is_root (dbbrowser))
    {
      filename = xfmpc_dbbrowser_get_parent_wdir (dbbrowser);
      xfmpc_dbbrowser_append (dbbrowser, filename, "..", TRUE);
      g_free (filename);
      i++;
    }

  while (xfmpc_mpdclient_database_read (dbbrowser->mpdclient, priv->wdir,
                                        &filename, &basename, &is_dir))
    {
      xfmpc_dbbrowser_append (dbbrowser, filename, basename, is_dir);

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
          i++;
        }

      g_free (filename);
      g_free (basename);
    }
}



void
xfmpc_dbbrowser_set_wdir (XfmpcDbbrowser *dbbrowser,
                          const gchar *dir)
{
  XfmpcDbbrowserPrivate    *priv = XFMPC_DBBROWSER_GET_PRIVATE (dbbrowser);

  g_free (priv->last_wdir);
  priv->last_wdir = priv->wdir;
  priv->wdir = g_strdup (dir);
}

gboolean
xfmpc_dbbrowser_wdir_is_root (XfmpcDbbrowser *dbbrowser)
{
  XfmpcDbbrowserPrivate    *priv = XFMPC_DBBROWSER_GET_PRIVATE (dbbrowser);

  return priv->wdir[0] == '\0';
}

gchar *
xfmpc_dbbrowser_get_parent_wdir (XfmpcDbbrowser *dbbrowser)
{
  XfmpcDbbrowserPrivate    *priv = XFMPC_DBBROWSER_GET_PRIVATE (dbbrowser);
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
  XfmpcDbbrowserPrivate    *priv = XFMPC_DBBROWSER_GET_PRIVATE (dbbrowser);
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

