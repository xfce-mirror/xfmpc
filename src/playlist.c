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
#include <gdk/gdkkeysyms.h>
#include <libxfce4util/libxfce4util.h>

#include "playlist.h"
#include "preferences.h"
#include "mpdclient.h"

#define BORDER 4

#define XFMPC_PLAYLIST_GET_PRIVATE(o) \
    (G_TYPE_INSTANCE_GET_PRIVATE ((o), XFMPC_TYPE_PLAYLIST, XfmpcPlaylistPrivate))



static void             xfmpc_playlist_class_init               (XfmpcPlaylistClass *klass);
static void             xfmpc_playlist_init                     (XfmpcPlaylist *playlist);
static void             xfmpc_playlist_dispose                  (GObject *object);
static void             xfmpc_playlist_finalize                 (GObject *object);

static void             cb_playlist_changed                     (XfmpcPlaylist *playlist);

static void             cb_row_activated                        (XfmpcPlaylist *playlist,
                                                                 GtkTreePath *path,
                                                                 GtkTreeViewColumn *column);
static gboolean         cb_key_released                         (XfmpcPlaylist *playlist,
                                                                 GdkEventKey *event);
static void             cb_filter_entry_activated               (XfmpcPlaylist *playlist);

static gboolean         cb_filter_entry_key_released            (XfmpcPlaylist *playlist,
                                                                 GdkEventKey *event);
static void             cb_filter_entry_changed                 (XfmpcPlaylist *playlist);

static gboolean         visible_func_filter_tree                (GtkTreeModel *filter,
                                                                 GtkTreeIter *iter,
                                                                 XfmpcPlaylist *playlist);



/* List store identifiers */
enum
{
  COLUMN_ID,
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
  GtkVBox               parent;
  XfmpcPlaylistPrivate *priv;
  XfmpcPreferences     *preferences;
  XfmpcMpdclient       *mpdclient;
};

struct _XfmpcPlaylistPrivate
{
  GtkWidget            *treeview;
  GtkTreeModelFilter   *filter;
  GtkListStore         *store;
  GtkWidget            *filter_entry;

  gint                  current;
  gboolean              autocenter;
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

  playlist->preferences = xfmpc_preferences_get ();
  playlist->mpdclient = xfmpc_mpdclient_new ();

  g_object_get (G_OBJECT (playlist->preferences),
                "playlist-autocenter", &priv->autocenter,
                NULL);

  /* === Tree model === */
  priv->store = gtk_list_store_new (N_COLUMNS,
                                    G_TYPE_INT,
                                    G_TYPE_STRING,
                                    G_TYPE_STRING,
                                    G_TYPE_INT);

  /* === Filter === */
  priv->filter_entry = gtk_entry_new ();
  priv->filter = GTK_TREE_MODEL_FILTER (gtk_tree_model_filter_new (GTK_TREE_MODEL (priv->store), NULL));
  g_object_unref (priv->store);
  gtk_tree_model_filter_set_visible_func (priv->filter,
                                          (GtkTreeModelFilterVisibleFunc)visible_func_filter_tree,
                                          playlist,
                                          NULL);

  /* === Tree view === */
  priv->treeview = gtk_tree_view_new ();
  gtk_tree_selection_set_mode (gtk_tree_view_get_selection (GTK_TREE_VIEW (priv->treeview)), GTK_SELECTION_MULTIPLE);
  gtk_tree_view_set_enable_search (GTK_TREE_VIEW (priv->treeview), FALSE);
  gtk_tree_view_set_headers_visible (GTK_TREE_VIEW (priv->treeview), FALSE);
  gtk_tree_view_set_rules_hint (GTK_TREE_VIEW (priv->treeview), TRUE);
  gtk_tree_view_set_model (GTK_TREE_VIEW (priv->treeview), GTK_TREE_MODEL (priv->filter));
  g_object_unref (priv->filter);

  /* Column "artist - title" */
  GtkCellRenderer *cell = gtk_cell_renderer_text_new ();
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

  /* === Scrolled window === */
  GtkWidget *scrolled = gtk_scrolled_window_new (NULL, NULL);
  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolled),
                                  GTK_POLICY_AUTOMATIC,
                                  GTK_POLICY_ALWAYS);

  /* === Containers === */
  gtk_container_add (GTK_CONTAINER (scrolled), priv->treeview);
  gtk_box_pack_start (GTK_BOX (playlist), scrolled, TRUE, TRUE, 0);
  gtk_box_pack_start (GTK_BOX (playlist), priv->filter_entry, FALSE, FALSE, 0);

  /* === Signals === */
  g_signal_connect_swapped (playlist->mpdclient, "song-changed",
                            G_CALLBACK (cb_playlist_changed), playlist);
  g_signal_connect_swapped (playlist->mpdclient, "playlist-changed",
                            G_CALLBACK (cb_playlist_changed), playlist);
  /* Tree view */
  g_signal_connect_swapped (priv->treeview, "row-activated",
                            G_CALLBACK (cb_row_activated), playlist);
  g_signal_connect_swapped (priv->treeview, "key-release-event",
                            G_CALLBACK (cb_key_released), playlist);
  /* Filter */
  g_signal_connect_swapped (priv->filter_entry, "activate",
                            G_CALLBACK (cb_filter_entry_activated), playlist);
  g_signal_connect_swapped (priv->filter_entry, "key-release-event",
                            G_CALLBACK (cb_filter_entry_key_released), playlist);
  g_signal_connect_swapped (priv->filter_entry, "changed",
                            G_CALLBACK (cb_filter_entry_changed), playlist);
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
                       gchar *song,
                       gchar *length,
                       gboolean is_current)
{
  XfmpcPlaylistPrivate *priv = XFMPC_PLAYLIST_GET_PRIVATE (playlist);
  GtkTreeIter           iter;

  gtk_list_store_append (priv->store, &iter);
  gtk_list_store_set (priv->store, &iter,
                      COLUMN_ID, id,
                      COLUMN_SONG, song,
                      COLUMN_LENGTH, length,
                      COLUMN_WEIGHT, is_current ? PANGO_WEIGHT_BOLD : PANGO_WEIGHT_NORMAL,
                      -1);
}

void
xfmpc_playlist_clear (XfmpcPlaylist *playlist)
{
  XfmpcPlaylistPrivate *priv = XFMPC_PLAYLIST_GET_PRIVATE (playlist);

  gtk_list_store_clear (priv->store);
}

void
xfmpc_playlist_select_row (XfmpcPlaylist *playlist,
                           gint i)
{
  XfmpcPlaylistPrivate *priv = XFMPC_PLAYLIST_GET_PRIVATE (playlist);

  if (gtk_tree_model_iter_n_children (GTK_TREE_MODEL (priv->filter), NULL) == 0)
    return;

  GtkTreePath *path = gtk_tree_path_new_from_indices (i, -1);
  gtk_tree_view_set_cursor (GTK_TREE_VIEW (priv->treeview), path, NULL, FALSE);
  gtk_tree_view_scroll_to_cell (GTK_TREE_VIEW (priv->treeview), path, NULL, TRUE, 0.42, 0);
  gtk_tree_path_free (path);
}

void
xfmpc_playlist_delete_selection (XfmpcPlaylist *playlist)
{
  XfmpcPlaylistPrivate *priv = XFMPC_PLAYLIST_GET_PRIVATE (playlist);
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



static void
cb_playlist_changed (XfmpcPlaylist *playlist)
{
  XfmpcPlaylistPrivate *priv = XFMPC_PLAYLIST_GET_PRIVATE (playlist);
  gchar                *song, *length;
  gint                  id, current;
  gboolean              count = priv->autocenter;
  gint                  i = 0;

  current = xfmpc_mpdclient_get_id (playlist->mpdclient);

  xfmpc_playlist_clear (playlist);
  while (xfmpc_mpdclient_playlist_read (playlist->mpdclient, &id, &song, &length))
    {
      if (count)
        count = current != id && (i += 1);
      xfmpc_playlist_append (playlist, id, song, length, current == id);
      g_free (song);
      g_free (length);
    }

  priv->current = i;
  /* don't autocenter if a filter is typped in */
  if (gtk_entry_get_text (GTK_ENTRY (priv->filter_entry))[0] == '\0' && i > 0 && priv->autocenter)
    xfmpc_playlist_select_row (playlist, priv->current);
}

static void
cb_row_activated (XfmpcPlaylist *playlist,
                  GtkTreePath *path,
                  GtkTreeViewColumn *column)
{
  XfmpcPlaylistPrivate *priv = XFMPC_PLAYLIST_GET_PRIVATE (playlist);
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

static void
cb_filter_entry_activated (XfmpcPlaylist *playlist)
{
  XfmpcPlaylistPrivate *priv = XFMPC_PLAYLIST_GET_PRIVATE (playlist);
  GtkTreeModel         *model = GTK_TREE_MODEL (priv->filter);
  GList                *list;

  list = gtk_tree_selection_get_selected_rows (gtk_tree_view_get_selection (GTK_TREE_VIEW (priv->treeview)), &model);
  if (G_LIKELY (NULL != list))
    {
      gtk_tree_view_row_activated (GTK_TREE_VIEW (priv->treeview), list->data, NULL);
      gtk_entry_set_text (GTK_ENTRY (priv->filter_entry), "");
      xfmpc_playlist_select_row (playlist, priv->current);
      g_list_foreach (list, (GFunc)gtk_tree_path_free, NULL);
      g_list_free (list);
    }
}

static gboolean
cb_filter_entry_key_released (XfmpcPlaylist *playlist,
                              GdkEventKey *event)
{
  XfmpcPlaylistPrivate *priv = XFMPC_PLAYLIST_GET_PRIVATE (playlist);

  if (event->type != GDK_KEY_RELEASE)
    return FALSE;

  if (event->keyval == GDK_Escape)
    {
      gtk_entry_set_text (GTK_ENTRY (priv->filter_entry), "");
      xfmpc_playlist_select_row (playlist, priv->current);
    }
  else if (gtk_entry_get_text (GTK_ENTRY (priv->filter_entry))[0] != '\0')
    xfmpc_playlist_select_row (playlist, 0);

  return TRUE;
}

static void
cb_filter_entry_changed (XfmpcPlaylist *playlist)
{
  XfmpcPlaylistPrivate *priv = XFMPC_PLAYLIST_GET_PRIVATE (playlist);

  gtk_tree_model_filter_refilter (priv->filter);
}

static gboolean
visible_func_filter_tree (GtkTreeModel *filter,
                          GtkTreeIter *iter,
                          XfmpcPlaylist *playlist)
{
  XfmpcPlaylistPrivate *priv = XFMPC_PLAYLIST_GET_PRIVATE (playlist);
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

