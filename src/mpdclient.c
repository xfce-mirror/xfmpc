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

#include <libxfce4util/libxfce4util.h>
#include <libmpd/libmpd.h>

#include "mpdclient.h"

#define MPD_HOST "localhost"
#define MPD_PORT 6600



static void             xfmpc_mpdclient_class_init              (XfmpcMpdclientClass *klass);
static void             xfmpc_mpdclient_init                    (XfmpcMpdclient *mpdclient);
static void             xfmpc_mpdclient_finalize                (GObject *object);

static void             xfmpc_mpdclient_initenv                 (XfmpcMpdclient *mpdclient);

static void             cb_xfmpc_mpdclient_status_changed       (MpdObj *mi,
                                                                 ChangedStatusType what,
                                                                 gpointer user_data);



struct _XfmpcMpdclientClass
{
  GObjectClass          parent_class;
};

struct _XfmpcMpdclient
{
  GObject               parent;
  XfmpcMpdclientPriv   *priv;
};

struct _XfmpcMpdclientPriv
{
  MpdObj               *mi;
  gchar                *host;
  guint                 port;
  gchar                *passwd;

  StatusField           status;
};



static GObjectClass *parent_class = NULL;



GType
xfmpc_mpdclient_get_type ()
{
  static GType xfmpc_mpdclient_type = G_TYPE_INVALID;

  if (G_UNLIKELY (xfmpc_mpdclient_type == G_TYPE_INVALID))
    {
      static const GTypeInfo xfmpc_mpdclient_info =
        {
          sizeof (XfmpcMpdclientClass),
          (GBaseInitFunc) NULL,
          (GBaseFinalizeFunc) NULL,
          (GClassInitFunc) xfmpc_mpdclient_class_init,
          (GClassFinalizeFunc) NULL,
          NULL,
          sizeof (XfmpcMpdclient),
          0,
          (GInstanceInitFunc) xfmpc_mpdclient_init,
          NULL
        };
      xfmpc_mpdclient_type = g_type_register_static (G_TYPE_OBJECT, "XfmpcMpdclient", &xfmpc_mpdclient_info, 0);
    }

  return xfmpc_mpdclient_type;
}



static void
xfmpc_mpdclient_class_init (XfmpcMpdclientClass *klass)
{
  GObjectClass *gobject_class;

  parent_class = g_type_class_peek_parent (klass);

  gobject_class = G_OBJECT_CLASS (klass);
  gobject_class->finalize = xfmpc_mpdclient_finalize;
}

static void
xfmpc_mpdclient_init (XfmpcMpdclient *mpdclient)
{
  mpdclient->priv = g_slice_new0 (XfmpcMpdclientPriv);
  xfmpc_mpdclient_initenv (mpdclient);
  mpdclient->priv->mi = mpd_new (mpdclient->priv->host, mpdclient->priv->port, mpdclient->priv->passwd);
  mpd_signal_connect_status_changed (mpdclient->priv->mi, (StatusChangedCallback)cb_xfmpc_mpdclient_status_changed, mpdclient);
}

static void
xfmpc_mpdclient_finalize (GObject *object)
{
  XfmpcMpdclient *mpdclient = XFMPC_MPDCLIENT (object);
  mpd_free (mpdclient->priv->mi);
  (*G_OBJECT_CLASS (parent_class)->finalize) (object);
}



XfmpcMpdclient *
xfmpc_mpdclient_new ()
{
  static XfmpcMpdclient *mpdclient = NULL;

  if (G_UNLIKELY (NULL == mpdclient))
    {
      mpdclient = g_object_new (XFMPC_TYPE_MPDCLIENT, NULL);
      g_object_add_weak_pointer (G_OBJECT (mpdclient), (gpointer)&mpdclient);
    }
  else
    g_object_ref (G_OBJECT (mpdclient));

  return mpdclient;
}

static void
xfmpc_mpdclient_initenv (XfmpcMpdclient *mpdclient)
{
  /* Hostname */
  mpdclient->priv->host = (g_getenv ("MPD_HOST") != NULL) ?
      g_strdup (g_getenv ("MPD_HOST")) :
      g_strdup (MPD_HOST);

  /* Port */
  mpdclient->priv->port = (g_getenv ("MPD_PORT") != NULL) ?
      (gint) g_ascii_strtoll (g_getenv ("MPD_PORT"), NULL, 0) :
      MPD_PORT;

  /* Check for password */
  mpdclient->priv->passwd = NULL;
  gchar **split = g_strsplit (mpdclient->priv->host, "@", 2);
  if (g_strv_length (split) == 2)
    {
      g_free (mpdclient->priv->host);
      mpdclient->priv->host = g_strdup (split[0]);
      mpdclient->priv->passwd = g_strdup (split[1]);
    }
  g_strfreev (split);
}

gboolean
xfmpc_mpdclient_connect (XfmpcMpdclient *mpdclient)
{
  if (xfmpc_mpdclient_is_connected (mpdclient))
    return TRUE;

  if (mpd_connect (mpdclient->priv->mi) != MPD_OK)
    return FALSE;

  mpd_send_password (mpdclient->priv->mi);

  return TRUE;
}

void
xfmpc_mpdclient_disconnect (XfmpcMpdclient *mpdclient)
{
  if (xfmpc_mpdclient_is_connected (mpdclient))
    mpd_disconnect (mpdclient->priv->mi);
}

gboolean
xfmpc_mpdclient_is_connected (XfmpcMpdclient *mpdclient)
{
  return mpd_check_connected (mpdclient->priv->mi);
}

gboolean
xfmpc_mpdclient_previous (XfmpcMpdclient *mpdclient)
{
  if (mpd_player_prev (mpdclient->priv->mi) != MPD_OK)
    return FALSE;
  else
    return TRUE;
}

gboolean
xfmpc_mpdclient_pp (XfmpcMpdclient *mpdclient)
{
  if (xfmpc_mpdclient_is_playing (mpdclient))
    return xfmpc_mpdclient_pause (mpdclient);
  else
    return xfmpc_mpdclient_play (mpdclient);
}

gboolean
xfmpc_mpdclient_play (XfmpcMpdclient *mpdclient)
{
  if (mpd_player_play (mpdclient->priv->mi) != MPD_OK)
    return FALSE;
  else
    return TRUE;
}

gboolean
xfmpc_mpdclient_pause (XfmpcMpdclient *mpdclient)
{
  if (mpd_player_pause (mpdclient->priv->mi) != MPD_OK)
    return FALSE;
  else
    return TRUE;
}

gboolean
xfmpc_mpdclient_stop (XfmpcMpdclient *mpdclient)
{
  if (mpd_player_stop (mpdclient->priv->mi) != MPD_OK)
    return FALSE;
  else
    return TRUE;
}

gboolean
xfmpc_mpdclient_next (XfmpcMpdclient *mpdclient)
{
  if (mpd_player_next (mpdclient->priv->mi) != MPD_OK)
    return FALSE;
  else
    return TRUE;
}

gboolean
xfmpc_mpdclient_set_volume (XfmpcMpdclient *mpdclient,
                            guint8 volume)
{
  if (mpd_status_set_volume (mpdclient->priv->mi, volume) < 0)
    return FALSE;
  else
    return TRUE;
}

gboolean
xfmpc_mpdclient_set_song_time (XfmpcMpdclient *mpdclient,
                               guint time)
{
  if (mpd_player_seek (mpdclient->priv->mi, time) != MPD_OK)
    return FALSE;
  else
    return TRUE;
}

const gchar *
xfmpc_mpdclient_get_artist (XfmpcMpdclient *mpdclient)
{
  mpd_Song *song = mpd_playlist_get_current_song (mpdclient->priv->mi);
  if (G_UNLIKELY (NULL == song))
    return NULL;

  if (G_UNLIKELY (NULL == song->artist))
    song->artist = g_strdup (_("n/a"));

  return song->artist;
}

const gchar *
xfmpc_mpdclient_get_title (XfmpcMpdclient *mpdclient)
{
  mpd_Song *song = mpd_playlist_get_current_song (mpdclient->priv->mi);
  if (G_UNLIKELY (NULL == song))
    return NULL;

  if (G_UNLIKELY (NULL == song->title))
    song->title = g_path_get_basename (song->file);

  return song->title;
}

const gchar *
xfmpc_mpdclient_get_album (XfmpcMpdclient *mpdclient)
{
  mpd_Song *song = mpd_playlist_get_current_song (mpdclient->priv->mi);
  if (G_UNLIKELY (NULL == song))
    return NULL;

  if (G_UNLIKELY (NULL == song->album))
    song->album = g_strdup (_("n/a"));

  return song->album;
}

const gchar *
xfmpc_mpdclient_get_date (XfmpcMpdclient *mpdclient)
{
  mpd_Song *song = mpd_playlist_get_current_song (mpdclient->priv->mi);
  if (G_UNLIKELY (NULL == song))
    return NULL;

  if (G_UNLIKELY (NULL == song->date))
    song->date = g_strdup (_("n/a"));

  return song->date;
}

gint
xfmpc_mpdclient_get_time (XfmpcMpdclient *mpdclient)
{
  return mpd_status_get_elapsed_song_time (mpdclient->priv->mi);
}

gint
xfmpc_mpdclient_get_total_time (XfmpcMpdclient *mpdclient)
{
  return mpd_status_get_total_song_time (mpdclient->priv->mi);
}

guint8
xfmpc_mpdclient_get_volume (XfmpcMpdclient *mpdclient)
{
  gint volume = mpd_status_get_volume (mpdclient->priv->mi);
  return (volume < 0) ? 100 : volume;
}

gboolean
xfmpc_mpdclient_is_playing (XfmpcMpdclient *mpdclient)
{
  return mpd_player_get_state (mpdclient->priv->mi) == MPD_PLAYER_PLAY;
}

gboolean
xfmpc_mpdclient_is_stopped (XfmpcMpdclient *mpdclient)
{
  return mpd_player_get_state (mpdclient->priv->mi) == MPD_PLAYER_STOP;
}

void
xfmpc_mpdclient_update_status (XfmpcMpdclient *mpdclient)
{
  mpd_status_update (mpdclient->priv->mi);
}

gboolean
xfmpc_mpdclient_status (XfmpcMpdclient *mpdclient,
                        gint bits)
{
  if (mpdclient->priv->status & bits)
    {
      mpdclient->priv->status &= ~bits;
      return TRUE;
    }

  return FALSE;
}

static void
cb_xfmpc_mpdclient_status_changed (MpdObj *mi,
                                   ChangedStatusType what,
                                   gpointer user_data)
{
  XfmpcMpdclient *mpdclient = user_data;
  g_return_if_fail (G_LIKELY (NULL != user_data));

  if (what & MPD_CST_STATE)
    {
      switch (mpd_player_get_state (mi))
        {
        case MPD_PLAYER_STOP:
          mpdclient->priv->status |= STOP_CHANGED;
          mpdclient->priv->status |= SONG_CHANGED; /* as to say that the next time
                                                      the song plays again, it needs
                                                      to update its title in the
                                                      interface */
          break;

        case MPD_PLAYER_PLAY:
        case MPD_PLAYER_PAUSE:
          mpdclient->priv->status |= PP_CHANGED;
          break;

        default:
          break;
        }
    }

  if (what & MPD_CST_SONGID)
    mpdclient->priv->status |= SONG_CHANGED;

  if (what & MPD_CST_VOLUME)
    mpdclient->priv->status |= VOLUME_CHANGED;

  if (what & (MPD_CST_ELAPSED_TIME|MPD_CST_TOTAL_TIME))
    mpdclient->priv->status |= TIME_CHANGED;
}

