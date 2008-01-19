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

#include <glib.h>
#include <libmpd/libmpd.h>

typedef struct _XfmpcMpdclient          XfmpcMpdclient;

struct _XfmpcMpdclient
{
  MpdObj               *mi;
  gchar                *host;
  guint                 port;
  gchar                *passwd;

  guint8                volume;
};

XfmpcMpdclient *        xfmpc_mpdclient_new                     ();

void                    xfmpc_mpdclient_initenv                 (XfmpcMpdclient *mpdclient);

gboolean                xfmpc_mpdclient_connect                 (XfmpcMpdclient *mpdclient);

void                    xfmpc_mpdclient_disconnect              (XfmpcMpdclient *mpdclient);

gboolean                xfmpc_mpdclient_is_connected            (XfmpcMpdclient *mpdclient);

void                    xfmpc_mpdclient_free                    (XfmpcMpdclient *mpdclient);

gboolean                xfmpc_mpdclient_previous                (XfmpcMpdclient *mpdclient);

gboolean                xfmpc_mpdclient_pp                      (XfmpcMpdclient *mpdclient);

gboolean                xfmpc_mpdclient_play                    (XfmpcMpdclient *mpdclient);

gboolean                xfmpc_mpdclient_pause                   (XfmpcMpdclient *mpdclient);

gboolean                xfmpc_mpdclient_stop                    (XfmpcMpdclient *mpdclient);

gboolean                xfmpc_mpdclient_next                    (XfmpcMpdclient *mpdclient);

gboolean                xfmpc_mpdclient_set_volume              (XfmpcMpdclient *mpdclient,
                                                                 guint8 volume);
gboolean                xfmpc_mpdclient_set_song_time           (XfmpcMpdclient *mpdclient,
                                                                 guint time);
gboolean                xfmpc_mpdclient_is_playing              (XfmpcMpdclient *mpdclient);

gboolean                xfmpc_mpdclient_is_stopped              (XfmpcMpdclient *mpdclient);

const gchar *           xfmpc_mpdclient_get_artist              (XfmpcMpdclient *mpdclient);

const gchar *           xfmpc_mpdclient_get_title               (XfmpcMpdclient *mpdclient);

const gchar *           xfmpc_mpdclient_get_album               (XfmpcMpdclient *mpdclient);

const gchar *           xfmpc_mpdclient_get_date                (XfmpcMpdclient *mpdclient);

gint                    xfmpc_mpdclient_get_time                (XfmpcMpdclient *mpdclient);

gint                    xfmpc_mpdclient_get_total_time          (XfmpcMpdclient *mpdclient);

guint8                  xfmpc_mpdclient_get_volume              (XfmpcMpdclient *mpdclient);

#endif

