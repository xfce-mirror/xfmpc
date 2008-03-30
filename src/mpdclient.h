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

#ifndef __XFMPC_MPDCLIENT_H__
#define __XFMPC_MPDCLIENT_H__

G_BEGIN_DECLS

#define XFMPC_TYPE_MPDCLIENT                (xfmpc_mpdclient_get_type())

#define XFMPC_MPDCLIENT(obj)                (G_TYPE_CHECK_INSTANCE_CAST ((obj), XFMPC_TYPE_MPDCLIENT, XfmpcMpdclient))
#define XFMPC_MPDCLIENT_CLASS(klass)        (G_TYPE_CHECK_CLASS_CAST ((klass), XFMPC_TYPE_MPDCLIENT, XfmpcMpdclientClass))

#define XFMPC_IS_MPDCLIENT(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), XFMPC_TYPE_MPDCLIENT))
#define XFMPC_IS_MPDCLIENT_CLASS(klass)     (G_TYPE_CHECK_CLASS_TYPE ((klass), XFMPC_TYPE_MPDCLIENT))

#define XFMPC_MPDCLIENT_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS ((obj), XFMPC_TYPE_MPDCLIENT, XfmpcMpdclientClass))

typedef struct _XfmpcMpdclientClass         XfmpcMpdclientClass;
typedef struct _XfmpcMpdclient              XfmpcMpdclient;
typedef struct _XfmpcMpdclientPrivate       XfmpcMpdclientPrivate;

GType                   xfmpc_mpdclient_get_type                () G_GNUC_CONST;

XfmpcMpdclient *        xfmpc_mpdclient_new                     ();

gboolean                xfmpc_mpdclient_connect                 (XfmpcMpdclient *mpdclient);

void                    xfmpc_mpdclient_disconnect              (XfmpcMpdclient *mpdclient);

gboolean                xfmpc_mpdclient_is_connected            (XfmpcMpdclient *mpdclient);

gboolean                xfmpc_mpdclient_previous                (XfmpcMpdclient *mpdclient);

gboolean                xfmpc_mpdclient_pp                      (XfmpcMpdclient *mpdclient);

gboolean                xfmpc_mpdclient_play                    (XfmpcMpdclient *mpdclient);

gboolean                xfmpc_mpdclient_pause                   (XfmpcMpdclient *mpdclient);

gboolean                xfmpc_mpdclient_stop                    (XfmpcMpdclient *mpdclient);

gboolean                xfmpc_mpdclient_next                    (XfmpcMpdclient *mpdclient);

gboolean                xfmpc_mpdclient_set_id                  (XfmpcMpdclient *mpdclient,
                                                                 gint id);
gboolean                xfmpc_mpdclient_set_volume              (XfmpcMpdclient *mpdclient,
                                                                 guint8 volume);
gboolean                xfmpc_mpdclient_set_song_time           (XfmpcMpdclient *mpdclient,
                                                                 guint time);
gint                    xfmpc_mpdclient_get_pos                 (XfmpcMpdclient *mpdclient);

gint                    xfmpc_mpdclient_get_id                  (XfmpcMpdclient *mpdclient);

const gchar *           xfmpc_mpdclient_get_artist              (XfmpcMpdclient *mpdclient);

const gchar *           xfmpc_mpdclient_get_title               (XfmpcMpdclient *mpdclient);

const gchar *           xfmpc_mpdclient_get_album               (XfmpcMpdclient *mpdclient);

const gchar *           xfmpc_mpdclient_get_date                (XfmpcMpdclient *mpdclient);

gint                    xfmpc_mpdclient_get_time                (XfmpcMpdclient *mpdclient);

gint                    xfmpc_mpdclient_get_total_time          (XfmpcMpdclient *mpdclient);

guint8                  xfmpc_mpdclient_get_volume              (XfmpcMpdclient *mpdclient);

gboolean                xfmpc_mpdclient_is_playing              (XfmpcMpdclient *mpdclient);

gboolean                xfmpc_mpdclient_is_stopped              (XfmpcMpdclient *mpdclient);

void                    xfmpc_mpdclient_update_status           (XfmpcMpdclient *mpdclient);

gboolean                xfmpc_mpdclient_queue_commit            (XfmpcMpdclient *mpdclient);

gboolean                xfmpc_mpdclient_queue_add               (XfmpcMpdclient *mpdclient,
                                                                 const gchar *path);
gboolean                xfmpc_mpdclient_queue_remove_id         (XfmpcMpdclient *mpdclient,
                                                                 gint id);
gboolean                xfmpc_mpdclient_playlist_read           (XfmpcMpdclient *mpdclient,
                                                                 gint *id,
                                                                 gchar **song,
                                                                 gchar **length);
gboolean                xfmpc_mpdclient_playlist_clear          (XfmpcMpdclient *mpdclient);

gboolean                xfmpc_mpdclient_database_read           (XfmpcMpdclient *mpdclient,
                                                                 const gchar *dir,
                                                                 gchar **filename,
                                                                 gchar **basename,
                                                                 gboolean *is_dir);
gboolean                xfmpc_mpdclient_database_refresh        (XfmpcMpdclient *mpdclient);


G_END_DECLS

#endif

