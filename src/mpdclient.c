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



XfmpcMpdclient *
xfmpc_mpdclient_new ()
{
  XfmpcMpdclient *mpdclient = g_slice_new0 (XfmpcMpdclient);
  xfmpc_mpdclient_initenv (mpdclient);
  mpdclient->mi = mpd_new (mpdclient->host, mpdclient->port, mpdclient->passwd);

  return mpdclient;
}

void
xfmpc_mpdclient_initenv (XfmpcMpdclient *mpdclient)
{
  /* Hostname */
  mpdclient->host = (g_getenv ("MPD_HOST") != NULL) ?
      g_strdup (g_getenv ("MPD_HOST")) :
      g_strdup (MPD_HOST);

  /* Port */
  mpdclient->port = (g_getenv ("MPD_PORT") != NULL) ?
      (gint) g_ascii_strtoll (g_getenv ("MPD_PORT"), NULL, 0) :
      MPD_PORT;

  /* Check for password */
  mpdclient->passwd = NULL;
  gchar **split = g_strsplit (mpdclient->host, "@", 2);
  if (g_strv_length (split) == 2)
    {
      g_free (mpdclient->host);
      mpdclient->host = g_strdup (split[0]);
      mpdclient->passwd = g_strdup (split[1]);
    }
  g_strfreev (split);
}

gboolean
xfmpc_mpdclient_connect (XfmpcMpdclient *mpdclient)
{
  if (xfmpc_mpdclient_is_connected (mpdclient))
    return TRUE;

  if (mpd_connect (mpdclient->mi) != MPD_OK)
    return FALSE;

  return TRUE;
}

void
xfmpc_mpdclient_disconnect (XfmpcMpdclient *mpdclient)
{
  if (xfmpc_mpdclient_is_connected (mpdclient))
    mpd_disconnect (mpdclient->mi);
}

gboolean
xfmpc_mpdclient_is_connected (XfmpcMpdclient *mpdclient)
{
  return mpd_check_connected (mpdclient->mi);
}

void
xfmpc_mpdclient_free (XfmpcMpdclient *mpdclient)
{
  mpd_free (mpdclient->mi);
}

gboolean
xfmpc_mpdclient_previous (XfmpcMpdclient *mpdclient)
{
  xfmpc_mpdclient_connect (mpdclient);
  if (mpd_player_prev (mpdclient->mi) != MPD_OK)
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
  xfmpc_mpdclient_connect (mpdclient);
  if (mpd_player_play (mpdclient->mi) != MPD_OK)
    return FALSE;
  else
    return TRUE;
}

gboolean
xfmpc_mpdclient_pause (XfmpcMpdclient *mpdclient)
{
  xfmpc_mpdclient_connect (mpdclient);
  if (mpd_player_pause (mpdclient->mi) != MPD_OK)
    return FALSE;
  else
    return TRUE;
}

gboolean
xfmpc_mpdclient_stop (XfmpcMpdclient *mpdclient)
{
  xfmpc_mpdclient_connect (mpdclient);
  if (mpd_player_stop (mpdclient->mi) != MPD_OK)
    return FALSE;
  else
    return TRUE;
}

gboolean
xfmpc_mpdclient_next (XfmpcMpdclient *mpdclient)
{
  xfmpc_mpdclient_connect (mpdclient);
  if (mpd_player_next (mpdclient->mi) != MPD_OK)
    return FALSE;
  else
    return TRUE;
}

gboolean
xfmpc_mpdclient_set_volume (XfmpcMpdclient *mpdclient,
                            guint8 volume)
{
  xfmpc_mpdclient_connect (mpdclient);
  if (mpd_status_set_volume (mpdclient->mi, volume) < 0)
    return FALSE;
  else
    return TRUE;
}

gboolean
xfmpc_mpdclient_set_song_time (XfmpcMpdclient *mpdclient,
                               guint time)
{
  xfmpc_mpdclient_connect (mpdclient);
  if (mpd_player_seek (mpdclient->mi, time) != MPD_OK)
    return FALSE;
  else
    return TRUE;
}

gboolean
xfmpc_mpdclient_is_playing (XfmpcMpdclient *mpdclient)
{
  xfmpc_mpdclient_connect (mpdclient);
  return mpd_player_get_state (mpdclient->mi) == MPD_PLAYER_PLAY;
}

gboolean
xfmpc_mpdclient_is_stopped (XfmpcMpdclient *mpdclient)
{
  return mpd_player_get_state (mpdclient->mi) == MPD_PLAYER_STOP;
}

const gchar *
xfmpc_mpdclient_get_artist (XfmpcMpdclient *mpdclient)
{
  mpd_Song *song = mpd_playlist_get_current_song (mpdclient->mi);
  if (G_UNLIKELY (NULL == song))
    return NULL;

  if (G_UNLIKELY (NULL == song->artist))
    song->artist = g_strdup (_("n/a"));

  return song->artist;
}

const gchar *
xfmpc_mpdclient_get_title (XfmpcMpdclient *mpdclient)
{
  mpd_Song *song = mpd_playlist_get_current_song (mpdclient->mi);
  if (G_UNLIKELY (NULL == song))
    return NULL;

  if (G_UNLIKELY (NULL == song->title))
    song->title = g_path_get_basename (song->file);

  return song->title;
}

const gchar *
xfmpc_mpdclient_get_album (XfmpcMpdclient *mpdclient)
{
  mpd_Song *song = mpd_playlist_get_current_song (mpdclient->mi);
  if (G_UNLIKELY (NULL == song))
    return NULL;

  if (G_UNLIKELY (NULL == song->album))
    song->album = g_strdup (_("n/a"));

  return song->album;
}

const gchar *
xfmpc_mpdclient_get_date (XfmpcMpdclient *mpdclient)
{
  mpd_Song *song = mpd_playlist_get_current_song (mpdclient->mi);
  if (G_UNLIKELY (NULL == song))
    return NULL;

  if (G_UNLIKELY (NULL == song->date))
    song->date = g_strdup (_("n/a"));

  return song->date;
}

gint
xfmpc_mpdclient_get_time (XfmpcMpdclient *mpdclient)
{
  return mpd_status_get_elapsed_song_time (mpdclient->mi);
}

gint
xfmpc_mpdclient_get_total_time (XfmpcMpdclient *mpdclient)
{
  xfmpc_mpdclient_connect (mpdclient);
  return mpd_status_get_total_song_time (mpdclient->mi);
}

guint8
xfmpc_mpdclient_get_volume (XfmpcMpdclient *mpdclient)
{
  gint volume = mpd_status_get_volume (mpdclient->mi);
  return (volume < 0) ? 100 : volume;
}

