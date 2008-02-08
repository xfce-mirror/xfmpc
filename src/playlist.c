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

#include "playlist.h"

#define BORDER 4

#define XFMPC_PLAYLIST_GET_PRIVATE(o) \
    (G_TYPE_INSTANCE_GET_PRIVATE ((o), XFMPC_TYPE_PLAYLIST, XfmpcPlaylistPrivate))



static void             xfmpc_playlist_class_init               (XfmpcPlaylistClass *klass);
static void             xfmpc_playlist_init                     (XfmpcPlaylist *playlist);
static void             xfmpc_playlist_dispose                  (GObject *object);
static void             xfmpc_playlist_finalize                 (GObject *object);



/* List store identifiers */
enum
{
  COLUMN_SONG,
  COLUMN_LENGTH,
  N_COLUMNS,
};



struct _XfmpcPlaylistClass
{
  GtkVBoxClass          parent_class;
};

struct _XfmpcPlaylist
{
  GtkVBox               parent;
  XfmpcPlaylistPrivate *priv;
};

struct _XfmpcPlaylistPrivate
{
  GtkWidget            *treeview;
  GtkListStore         *store;
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
  XfmpcPlaylistPrivate *priv = XFMPC_PLAYLIST_GET_PRIVATE (playlist);

  priv->store = gtk_list_store_new (N_COLUMNS,
                                         G_TYPE_STRING,
                                         G_TYPE_STRING);

  priv->treeview = gtk_tree_view_new ();
  gtk_tree_view_set_headers_visible (GTK_TREE_VIEW (priv->treeview), FALSE);
  gtk_tree_view_set_rules_hint (GTK_TREE_VIEW (priv->treeview), TRUE);
  gtk_tree_view_set_model (GTK_TREE_VIEW (priv->treeview), GTK_TREE_MODEL (priv->store));
  g_object_unref (priv->store);

  GtkCellRenderer *cell = gtk_cell_renderer_text_new ();
  gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (priv->treeview),
                                               -1, "Song", cell,
                                               "text", COLUMN_SONG,
                                               NULL);

  cell = gtk_cell_renderer_text_new ();
  g_object_set (G_OBJECT (cell), "xalign", 1.0, NULL);
  gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (priv->treeview),
                                               -1, "Length", cell,
                                               "text", COLUMN_LENGTH,
                                               NULL);

#if 1
  xfmpc_playlist_append (playlist, "Hello - World!", "0:00");
  xfmpc_playlist_append (playlist, "Good bye - World!", "0:00");
#endif

  GtkWidget *scrolled = gtk_scrolled_window_new (NULL, NULL);
  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolled),
                                  GTK_POLICY_AUTOMATIC,
                                  GTK_POLICY_ALWAYS);

  gtk_container_add (GTK_CONTAINER (scrolled), priv->treeview);
  gtk_box_pack_start (GTK_BOX (playlist), scrolled, TRUE, TRUE, 0);
}

static void
xfmpc_playlist_dispose (GObject *object)
{
  (*G_OBJECT_CLASS (parent_class)->dispose) (object);
}

static void
xfmpc_playlist_finalize (GObject *object)
{
  (*G_OBJECT_CLASS (parent_class)->finalize) (object);
}



GtkWidget*
xfmpc_playlist_new ()
{
  return g_object_new (XFMPC_TYPE_PLAYLIST, NULL);
}

void
xfmpc_playlist_append (XfmpcPlaylist *playlist,
                       gchar *song,
                       gchar *length)
{
  XfmpcPlaylistPrivate *priv = XFMPC_PLAYLIST_GET_PRIVATE (playlist);
  GtkTreeIter iter;

  gtk_list_store_append (priv->store, &iter);
  gtk_list_store_set (priv->store, &iter,
                      COLUMN_SONG, song,
                      COLUMN_LENGTH, length,
                      -1);
}

void
xfmpc_playlist_clear (XfmpcPlaylist *playlist)
{
  XfmpcPlaylistPrivate *priv = XFMPC_PLAYLIST_GET_PRIVATE (playlist);

  gtk_list_store_clear (priv->store);
}

