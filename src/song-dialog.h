/*
 *  Copyright (c) 2009 Mike Massonnet <mmassonnet@xfce.org>
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

#ifndef __XFMPC_SONG_DIALOG_H__
#define __XFMPC_SONG_DIALOG_H__

G_BEGIN_DECLS;

#define XFMPC_TYPE_SONG_DIALOG                  (xfmpc_song_dialog_get_type ())

#define XFMPC_SONG_DIALOG(obj)                  (G_TYPE_CHECK_INSTANCE_CAST ((obj), XFMPC_TYPE_SONG_DIALOG, XfmpcSongDialog))
#define XFMPC_SONG_DIALOG_CLASS(klass)          (G_TYPE_CHECK_CLASS_CAST ((klass), XFMPC_TYPE_SONG_DIALOG, XfmpcSongDialogClass))

#define XFMPC_IS_SONG_DIALOG(obj)               (G_TYPE_CHECK_INSTANCE_TYPE ((obj), XFMPC_TYPE_SONG_DIALOG))
#define XFMPC_IS_SONG_DIALOG_CLASS(klass)       (G_TYPE_CHECK_CLASS_TYPE ((klass), XFMPC_TYPE_SONG_DIALOG))

#define XFMPC_SONG_DIALOG_GET_CLASS(obj)        (G_TYPE_INSTANCE_GET_CLASS ((obj), XFMPC_TYPE_SONG_DIALOG, XfmpcSongDialogClass))

typedef struct          _XfmpcSongDialogClass           XfmpcSongDialogClass;
typedef struct          _XfmpcSongDialog                XfmpcSongDialog;
typedef struct          _XfmpcSongDialogPrivate         XfmpcSongDialogPrivate;

GType                   xfmpc_song_dialog_get_type      () G_GNUC_CONST;

GtkWidget *             xfmpc_song_dialog_new           (gint id);


G_END_DECLS;

#endif


