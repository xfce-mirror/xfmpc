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
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#ifndef __XFMPC_PLAYLIST_H__
#define __XFMPC_PLAYLIST_H__

G_BEGIN_DECLS

#define XFMPC_TYPE_PLAYLIST             (xfmpc_playlist_get_type())

#define XFMPC_PLAYLIST(obj)             (G_TYPE_CHECK_INSTANCE_CAST ((obj), XFMPC_TYPE_PLAYLIST, XfmpcPlaylist))
#define XFMPC_PLAYLIST_CLASS(klass)     (G_TYPE_CHECK_CLASS_CAST ((klass), XFMPC_TYPE_PLAYLIST, XfmpcPlaylistClass))

#define XFMPC_IS_PLAYLIST(obj)          (G_TYPE_CHECK_INSTANCE_TYPE ((obj), XFMPC_TYPE_PLAYLIST))
#define XFMPC_IS_PLAYLIST_CLASS(klass)  (G_TYPE_CHECK_CLASS_TYPE ((klass), XFMPC_TYPE_PLAYLIST))

#define XFMPC_PLAYLIST_GET_CLASS(obj)   (G_TYPE_INSTANCE_GET_CLASS ((obj), XFMPC_TYPE_PLAYLIST, XfmpcPlaylistClass))

typedef struct _XfmpcPlaylistClass      XfmpcPlaylistClass;
typedef struct _XfmpcPlaylist           XfmpcPlaylist;
typedef struct _XfmpcPlaylistPrivate    XfmpcPlaylistPrivate;

GType                   xfmpc_playlist_get_type                 () G_GNUC_CONST;

GtkWidget *             xfmpc_playlist_new                      ();

void                    xfmpc_playlist_append                   (XfmpcPlaylist *playlist,
                                                                 gint id,
                                                                 gint pos,
                                                                 gchar *filename,
                                                                 gchar *song,
                                                                 gchar *length);
void                    xfmpc_playlist_clear                    (XfmpcPlaylist *playlist);

void                    xfmpc_playlist_select_row               (XfmpcPlaylist *playlist,
                                                                 gint i);
void                    xfmpc_playlist_delete_selection         (XfmpcPlaylist *playlist);

void                    xfmpc_playlist_refresh_current_song     (XfmpcPlaylist *playlist);


G_END_DECLS

#endif

