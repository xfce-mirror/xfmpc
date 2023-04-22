/*
 *  Copyright (c) 2008-2010 Mike Massonnet <mmassonnet@xfce.org>
 *  Copyright (c) 2009-2010 Vincent Legout <vincent@xfce.org>
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

#include <libxfce4util/libxfce4util.h>
#include <libmpd/libmpd.h>

#include "mpdclient.h"
#include "src/xfmpc.h"



enum
{
  SIG_CONNECTED,
  SIG_SONG_CHANGED,
  SIG_PP_CHANGED,
  SIG_TIME_CHANGED,
  SIG_TOTAL_TIME_CHANGED,
  SIG_VOLUME_CHANGED,
  SIG_STOPPED,
  SIG_DATABASE_CHANGED,
  SIG_PLAYLIST_CHANGED,
  SIG_REPEAT,
  SIG_RANDOM,
  SIG_SINGLE,
  SIG_CONSUME,
  LAST_SIGNAL
};

static guint            signals[LAST_SIGNAL] = { 0 };



static void             xfmpc_mpdclient_finalize                (GObject *object);

static void             xfmpc_mpdclient_initenv                 (XfmpcMpdclient *mpdclient);

static void             cb_status_changed                       (MpdObj *mi,
                                                                 ChangedStatusType what,
                                                                 gpointer user_data);
static gchar *          _get_formatted_name                     (mpd_Song *song);
static gchar *          _get_formatted_name_predefined          (mpd_Song *song,
                                                                 XfmpcPreferencesSongFormat song_format);
static gchar *          _get_formatted_name_custom              (mpd_Song *song,
                                                                 const gchar *format);



struct _XfmpcMpdclientClass
{
  GObjectClass              parent_class;

  void (*connected)         (XfmpcMpdclient *mpdclient, gpointer user_data);
  void (*song_changed)      (XfmpcMpdclient *mpdclient, gpointer user_data);
  void (*pp_changed)        (XfmpcMpdclient *mpdclient, gboolean is_playing, gpointer user_data);
  void (*time_changed)      (XfmpcMpdclient *mpdclient, gint time, gpointer user_data);
  void (*total_time_changed)(XfmpcMpdclient *mpdclient, gint total_time, gpointer user_data);
  void (*volume_changed)    (XfmpcMpdclient *mpdclient, gint volume, gpointer user_data);
  void (*stopped)           (XfmpcMpdclient *mpdclient, gpointer user_data);
  void (*database_changed)  (XfmpcMpdclient *mpdclient, gpointer user_data);
  void (*playlist_changed)  (XfmpcMpdclient *mpdclient, gpointer user_data);
  void (*repeat)            (XfmpcMpdclient *mpdclient, gboolean repeat, gpointer user_data);
  void (*random)            (XfmpcMpdclient *mpdclient, gboolean random, gpointer user_data);
  void (*single)            (XfmpcMpdclient *mpdclient, gboolean single, gpointer user_data);
  void (*consume)           (XfmpcMpdclient *mpdclient, gboolean consume, gpointer user_data);
};

struct _XfmpcMpdclient
{
  GObject                   parent;
  /*<private>*/
  XfmpcMpdclientPrivate    *priv;
};

struct _XfmpcMpdclientPrivate
{
  MpdObj                   *mi;
  gchar                    *host;
  guint                     port;
  gchar                    *passwd;
  gboolean                  env_cached;
  gboolean                  connecting;
  guint                     connection_count;
  GMutex                    mutex;
};



static GObjectClass *parent_class = NULL;



G_DEFINE_TYPE_WITH_PRIVATE(XfmpcMpdclient, xfmpc_mpdclient, G_TYPE_OBJECT)



static void
xfmpc_mpdclient_class_init (XfmpcMpdclientClass *klass)
{
  GObjectClass *gobject_class;

  parent_class = g_type_class_peek_parent (klass);

  gobject_class = G_OBJECT_CLASS (klass);
  gobject_class->finalize = xfmpc_mpdclient_finalize;

  signals[SIG_CONNECTED] =
    g_signal_new ("connected", G_TYPE_FROM_CLASS (klass),
                  G_SIGNAL_RUN_FIRST|G_SIGNAL_ACTION,
                  G_STRUCT_OFFSET (XfmpcMpdclientClass, connected),
                  NULL, NULL,
                  g_cclosure_marshal_VOID__VOID,
                  G_TYPE_NONE, 0);

  signals[SIG_SONG_CHANGED] =
    g_signal_new ("song-changed", G_TYPE_FROM_CLASS (klass),
                  G_SIGNAL_RUN_FIRST|G_SIGNAL_ACTION,
                  G_STRUCT_OFFSET (XfmpcMpdclientClass, song_changed),
                  NULL, NULL,
                  g_cclosure_marshal_VOID__VOID,
                  G_TYPE_NONE, 0);

  signals[SIG_PP_CHANGED] =
    g_signal_new ("pp-changed", G_TYPE_FROM_CLASS (klass),
                  G_SIGNAL_RUN_FIRST|G_SIGNAL_ACTION,
                  G_STRUCT_OFFSET (XfmpcMpdclientClass, pp_changed),
                  NULL, NULL,
                  g_cclosure_marshal_VOID__BOOLEAN,
                  G_TYPE_NONE, 1,
                  G_TYPE_BOOLEAN);

  signals[SIG_TIME_CHANGED] =
    g_signal_new ("time-changed", G_TYPE_FROM_CLASS (klass),
                  G_SIGNAL_RUN_FIRST|G_SIGNAL_ACTION,
                  G_STRUCT_OFFSET (XfmpcMpdclientClass, time_changed),
                  NULL, NULL,
                  g_cclosure_marshal_VOID__INT,
                  G_TYPE_NONE, 1,
                  G_TYPE_INT);

  signals[SIG_TOTAL_TIME_CHANGED] =
    g_signal_new ("total-time-changed", G_TYPE_FROM_CLASS (klass),
                  G_SIGNAL_RUN_FIRST|G_SIGNAL_ACTION,
                  G_STRUCT_OFFSET (XfmpcMpdclientClass, total_time_changed),
                  NULL, NULL,
                  g_cclosure_marshal_VOID__INT,
                  G_TYPE_NONE, 1,
                  G_TYPE_INT);

  signals[SIG_VOLUME_CHANGED] =
    g_signal_new ("volume-changed", G_TYPE_FROM_CLASS (klass),
                  G_SIGNAL_RUN_FIRST|G_SIGNAL_ACTION,
                  G_STRUCT_OFFSET (XfmpcMpdclientClass, volume_changed),
                  NULL, NULL,
                  g_cclosure_marshal_VOID__INT,
                  G_TYPE_NONE, 1,
                  G_TYPE_INT);

  signals[SIG_STOPPED] =
    g_signal_new ("stopped", G_TYPE_FROM_CLASS (klass),
                  G_SIGNAL_RUN_FIRST|G_SIGNAL_ACTION,
                  G_STRUCT_OFFSET (XfmpcMpdclientClass, stopped),
                  NULL, NULL,
                  g_cclosure_marshal_VOID__VOID,
                  G_TYPE_NONE, 0);

  signals[SIG_DATABASE_CHANGED] =
    g_signal_new ("database-changed", G_TYPE_FROM_CLASS (klass),
                  G_SIGNAL_RUN_FIRST|G_SIGNAL_ACTION,
                  G_STRUCT_OFFSET (XfmpcMpdclientClass, database_changed),
                  NULL, NULL,
                  g_cclosure_marshal_VOID__VOID,
                  G_TYPE_NONE, 0);

  signals[SIG_PLAYLIST_CHANGED] =
    g_signal_new ("playlist-changed", G_TYPE_FROM_CLASS (klass),
                  G_SIGNAL_RUN_FIRST|G_SIGNAL_ACTION,
                  G_STRUCT_OFFSET (XfmpcMpdclientClass, playlist_changed),
                  NULL, NULL,
                  g_cclosure_marshal_VOID__VOID,
                  G_TYPE_NONE, 0);

  signals[SIG_REPEAT] =
    g_signal_new ("repeat", G_TYPE_FROM_CLASS (klass),
                  G_SIGNAL_RUN_FIRST|G_SIGNAL_ACTION,
                  G_STRUCT_OFFSET (XfmpcMpdclientClass, repeat),
                  NULL, NULL,
                  g_cclosure_marshal_VOID__BOOLEAN,
                  G_TYPE_NONE, 1,
                  G_TYPE_BOOLEAN);

  signals[SIG_RANDOM] =
    g_signal_new ("random", G_TYPE_FROM_CLASS (klass),
                  G_SIGNAL_RUN_FIRST|G_SIGNAL_ACTION,
                  G_STRUCT_OFFSET (XfmpcMpdclientClass, random),
                  NULL, NULL,
                  g_cclosure_marshal_VOID__BOOLEAN,
                  G_TYPE_NONE, 1,
                  G_TYPE_BOOLEAN);

  signals[SIG_SINGLE] =
    g_signal_new ("single", G_TYPE_FROM_CLASS (klass),
                  G_SIGNAL_RUN_FIRST|G_SIGNAL_ACTION,
                  G_STRUCT_OFFSET (XfmpcMpdclientClass, single),
                  NULL, NULL,
                  g_cclosure_marshal_VOID__BOOLEAN,
                  G_TYPE_NONE, 1,
                  G_TYPE_BOOLEAN);

  signals[SIG_CONSUME] =
    g_signal_new ("consume", G_TYPE_FROM_CLASS (klass),
                  G_SIGNAL_RUN_FIRST|G_SIGNAL_ACTION,
                  G_STRUCT_OFFSET (XfmpcMpdclientClass, consume),
                  NULL, NULL,
                  g_cclosure_marshal_VOID__BOOLEAN,
                  G_TYPE_NONE, 1,
                  G_TYPE_BOOLEAN);
}

static void
xfmpc_mpdclient_init (XfmpcMpdclient *mpdclient)
{
  XfmpcMpdclientPrivate *priv = mpdclient->priv = xfmpc_mpdclient_get_instance_private (mpdclient);

  priv->mi = mpd_new_default ();
  g_mutex_init (&priv->mutex);

  mpd_signal_connect_status_changed (priv->mi, (StatusChangedCallback)cb_status_changed, mpdclient);
}

static void
xfmpc_mpdclient_finalize (GObject *object)
{
  XfmpcMpdclient *mpdclient = XFMPC_MPDCLIENT (object);
  XfmpcMpdclientPrivate *priv = XFMPC_MPDCLIENT (mpdclient)->priv;

  mpd_free (priv->mi);
  g_mutex_clear (&priv->mutex);

  (*G_OBJECT_CLASS (parent_class)->finalize) (object);
}



XfmpcMpdclient *
xfmpc_mpdclient_get_default (void)
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
  XfmpcMpdclientPrivate *priv = XFMPC_MPDCLIENT (mpdclient)->priv;
  XfmpcPreferences *preferences = xfmpc_preferences_get_default ();
  gboolean use_defaults;

  g_object_get (preferences, "mpd-use-defaults", &use_defaults, NULL);

  if (use_defaults)
    {
      if (!priv->env_cached)
        {
          g_free (priv->host);
          g_free (priv->passwd);
          priv->env_cached = TRUE;

          priv->host = (g_getenv ("MPD_HOST") != NULL) ?
            g_strdup (g_getenv ("MPD_HOST")) :
            g_strdup ("127.0.0.1");

          priv->port = (g_getenv ("MPD_PORT") != NULL) ?
            (gint) g_ascii_strtoll (g_getenv ("MPD_PORT"), NULL, 0) :
            6600;

          priv->passwd = NULL;
	  {
            gchar **split = g_strsplit (priv->host, "@", 2);
            if (g_strv_length (split) == 2)
              {
                g_free (priv->host);
                priv->host = g_strdup (split[1]);
                priv->passwd = g_strdup (split[0]);
              }
            g_strfreev (split);
	  }
        }
    }
  else
    {
      g_free (priv->host);
      g_free (priv->passwd);
      priv->env_cached = FALSE;

      g_object_get (preferences,
                    "mpd-hostname", &priv->host,
                    "mpd-port", &priv->port,
                    "mpd-password", &priv->passwd,
                    NULL);
    }
}

static gpointer
xfmpc_mpdclient_connect_thread (XfmpcMpdclient *mpdclient)
{
  XfmpcMpdclientPrivate *priv = XFMPC_MPDCLIENT (mpdclient)->priv;
  mpd_Connection *connection;

  xfmpc_mpdclient_initenv (mpdclient);
  mpd_set_hostname (priv->mi, priv->host);
  mpd_set_port (priv->mi, priv->port);
  mpd_set_password (priv->mi, (priv->passwd != NULL) ? priv->passwd : "");
  mpd_set_connection_timeout (priv->mi, 5.0);

  connection = mpd_newConnection (priv->host, priv->port, 5.0);
  if (mpd_connect_real (priv->mi, connection))
    {
      if ((priv->connection_count++) == 1)
        g_message (_("Failed to connect to MPD"));
    }
  else
    {
      priv->connection_count = 0;
      g_message (_("Connected to MPD"));
    }

  priv->connecting = FALSE;

  return NULL;
}

gboolean
xfmpc_mpdclient_connect (XfmpcMpdclient *mpdclient)
{
  XfmpcMpdclientPrivate *priv = XFMPC_MPDCLIENT (mpdclient)->priv;

  GThread *thread;

  if (xfmpc_mpdclient_is_connected (mpdclient))
    return TRUE;

  /* return FALSE if a we are already trying to connect to mpd */
  if (!g_mutex_trylock (&priv->mutex))
  {
    g_warning ("Already connecting to mpd");
    return FALSE;
  }

  priv->connecting = TRUE;

  thread = g_thread_new ("connection thread", (GThreadFunc) xfmpc_mpdclient_connect_thread,
          mpdclient);

  while (priv->connecting)
  {
    while (gtk_events_pending ())
      gtk_main_iteration ();
    g_usleep (200000);
  }

  g_thread_join (thread);

  g_signal_emit (mpdclient, signals[SIG_CONNECTED], 0);

  g_mutex_unlock (&priv->mutex);

  return TRUE;
}

void
xfmpc_mpdclient_disconnect (XfmpcMpdclient *mpdclient)
{
  XfmpcMpdclientPrivate *priv = XFMPC_MPDCLIENT (mpdclient)->priv;

  if (xfmpc_mpdclient_is_connected (mpdclient))
    mpd_disconnect (priv->mi);
}

gboolean
xfmpc_mpdclient_is_connected (XfmpcMpdclient *mpdclient)
{
  XfmpcMpdclientPrivate *priv = XFMPC_MPDCLIENT (mpdclient)->priv;

  return mpd_check_connected (priv->mi);
}

gboolean
xfmpc_mpdclient_previous (XfmpcMpdclient *mpdclient)
{
  XfmpcMpdclientPrivate *priv = XFMPC_MPDCLIENT (mpdclient)->priv;

  if (mpd_player_prev (priv->mi) != MPD_OK)
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
  XfmpcMpdclientPrivate *priv = XFMPC_MPDCLIENT (mpdclient)->priv;

  if (mpd_player_play (priv->mi) != MPD_OK)
    return FALSE;
  else
    return TRUE;
}

gboolean
xfmpc_mpdclient_pause (XfmpcMpdclient *mpdclient)
{
  XfmpcMpdclientPrivate *priv = XFMPC_MPDCLIENT (mpdclient)->priv;

  if (mpd_player_pause (priv->mi) != MPD_OK)
    return FALSE;
  else
    return TRUE;
}

gboolean
xfmpc_mpdclient_stop (XfmpcMpdclient *mpdclient)
{
  XfmpcMpdclientPrivate *priv = XFMPC_MPDCLIENT (mpdclient)->priv;

  if (mpd_player_stop (priv->mi) != MPD_OK)
    return FALSE;
  else
    return TRUE;
}

gboolean
xfmpc_mpdclient_next (XfmpcMpdclient *mpdclient)
{
  XfmpcMpdclientPrivate *priv = XFMPC_MPDCLIENT (mpdclient)->priv;

  if (mpd_player_next (priv->mi) != MPD_OK)
    return FALSE;
  else
    return TRUE;
}

gboolean
xfmpc_mpdclient_set_id (XfmpcMpdclient *mpdclient,
                        gint id)
{
  XfmpcMpdclientPrivate *priv = XFMPC_MPDCLIENT (mpdclient)->priv;

  return mpd_player_play_id (priv->mi, id) == MPD_OK ? TRUE : FALSE;
}

gboolean
xfmpc_mpdclient_set_song_time (XfmpcMpdclient *mpdclient,
                               guint song_time)
{
  XfmpcMpdclientPrivate *priv = XFMPC_MPDCLIENT (mpdclient)->priv;

  if (mpd_player_seek (priv->mi, song_time) != MPD_OK)
    return FALSE;
  else
    return TRUE;
}

gboolean
xfmpc_mpdclient_set_volume (XfmpcMpdclient *mpdclient,
                            guint8 volume)
{
  XfmpcMpdclientPrivate *priv = XFMPC_MPDCLIENT (mpdclient)->priv;

  if (mpd_status_set_volume (priv->mi, volume) < 0)
    return FALSE;
  else
    return TRUE;
}

gboolean
xfmpc_mpdclient_set_repeat (XfmpcMpdclient *mpdclient,
                            gboolean repeat)
{
  XfmpcMpdclientPrivate *priv = XFMPC_MPDCLIENT (mpdclient)->priv;

  if (MPD_OK != mpd_player_set_repeat (priv->mi, repeat))
    return FALSE;
  else
    return TRUE;
}

gboolean
xfmpc_mpdclient_set_random (XfmpcMpdclient *mpdclient,
                            gboolean random_)
{
  XfmpcMpdclientPrivate *priv = XFMPC_MPDCLIENT (mpdclient)->priv;

  if (MPD_OK != mpd_player_set_random (priv->mi, random_))
    return FALSE;
  else
    return TRUE;
}

gboolean
xfmpc_mpdclient_set_single (XfmpcMpdclient *mpdclient,
                            gboolean single)
{
  XfmpcMpdclientPrivate *priv = XFMPC_MPDCLIENT (mpdclient)->priv;

  if (MPD_OK != mpd_player_set_single (priv->mi, single))
    return FALSE;
  else
    return TRUE;
}

gboolean
xfmpc_mpdclient_set_consume (XfmpcMpdclient *mpdclient,
                             gboolean consume)
{
  XfmpcMpdclientPrivate *priv = XFMPC_MPDCLIENT (mpdclient)->priv;

  if (MPD_OK != mpd_player_set_consume (priv->mi, consume))
    return FALSE;
  else
    return TRUE;
}



gint
xfmpc_mpdclient_get_pos (XfmpcMpdclient *mpdclient)
{
  XfmpcMpdclientPrivate *priv = XFMPC_MPDCLIENT (mpdclient)->priv;

  gint pos = mpd_player_get_current_song_pos (priv->mi);
  if (pos < 0)
    pos = 0;

  return pos;
}

gint
xfmpc_mpdclient_get_id (XfmpcMpdclient *mpdclient)
{
  XfmpcMpdclientPrivate *priv = XFMPC_MPDCLIENT (mpdclient)->priv;

  return mpd_player_get_current_song_id (priv->mi);
}

const gchar *
xfmpc_mpdclient_get_artist (XfmpcMpdclient *mpdclient)
{
  XfmpcMpdclientPrivate *priv = XFMPC_MPDCLIENT (mpdclient)->priv;

  mpd_Song *song = mpd_playlist_get_current_song (priv->mi);
  if (G_UNLIKELY (NULL == song))
    return NULL;

  if (G_UNLIKELY (NULL == song->artist))
    song->artist = g_strdup (_("n/a"));

  return song->artist;
}

const gchar *
xfmpc_mpdclient_get_title (XfmpcMpdclient *mpdclient)
{
  XfmpcMpdclientPrivate *priv = XFMPC_MPDCLIENT (mpdclient)->priv;

  mpd_Song *song = mpd_playlist_get_current_song (priv->mi);
  if (G_UNLIKELY (NULL == song))
    return NULL;

  if (G_UNLIKELY (NULL == song->title))
    song->title = g_path_get_basename (song->file);

  return song->title;
}

const gchar *
xfmpc_mpdclient_get_album (XfmpcMpdclient *mpdclient)
{
  XfmpcMpdclientPrivate *priv = XFMPC_MPDCLIENT (mpdclient)->priv;

  mpd_Song *song = mpd_playlist_get_current_song (priv->mi);
  if (G_UNLIKELY (NULL == song))
    return NULL;

  if (G_UNLIKELY (NULL == song->album))
    song->album = g_strdup (_("n/a"));

  return song->album;
}

const gchar *
xfmpc_mpdclient_get_date (XfmpcMpdclient *mpdclient)
{
  XfmpcMpdclientPrivate *priv = XFMPC_MPDCLIENT (mpdclient)->priv;

  mpd_Song *song = mpd_playlist_get_current_song (priv->mi);
  if (G_UNLIKELY (NULL == song))
    return NULL;

  if (G_UNLIKELY (NULL == song->date))
    song->date = g_strdup (_("n/a"));

  return song->date;
}

gint
xfmpc_mpdclient_get_time (XfmpcMpdclient *mpdclient)
{
  XfmpcMpdclientPrivate *priv = XFMPC_MPDCLIENT (mpdclient)->priv;
  return mpd_status_get_elapsed_song_time (priv->mi);
}

gint
xfmpc_mpdclient_get_total_time (XfmpcMpdclient *mpdclient)
{
  XfmpcMpdclientPrivate *priv = XFMPC_MPDCLIENT (mpdclient)->priv;
  return mpd_status_get_total_song_time (priv->mi);
}

guint8
xfmpc_mpdclient_get_volume (XfmpcMpdclient *mpdclient)
{
  XfmpcMpdclientPrivate *priv = XFMPC_MPDCLIENT (mpdclient)->priv;
  gint volume = mpd_status_get_volume (priv->mi);
  return (volume < 0) ? 100 : volume;
}

gboolean
xfmpc_mpdclient_get_repeat (XfmpcMpdclient *mpdclient)
{
  XfmpcMpdclientPrivate *priv = XFMPC_MPDCLIENT (mpdclient)->priv;
  return mpd_player_get_repeat (priv->mi);
}

gboolean
xfmpc_mpdclient_get_random (XfmpcMpdclient *mpdclient)
{
  XfmpcMpdclientPrivate *priv = XFMPC_MPDCLIENT (mpdclient)->priv;
  return mpd_player_get_random (priv->mi);
}

gboolean
xfmpc_mpdclient_get_single (XfmpcMpdclient *mpdclient)
{
  XfmpcMpdclientPrivate *priv = XFMPC_MPDCLIENT (mpdclient)->priv;
  return mpd_player_get_single (priv->mi);
}

gboolean
xfmpc_mpdclient_get_consume (XfmpcMpdclient *mpdclient)
{
  XfmpcMpdclientPrivate *priv = XFMPC_MPDCLIENT (mpdclient)->priv;
  return mpd_player_get_consume (priv->mi);
}

gboolean
xfmpc_mpdclient_is_playing (XfmpcMpdclient *mpdclient)
{
  XfmpcMpdclientPrivate *priv = XFMPC_MPDCLIENT (mpdclient)->priv;
  return mpd_player_get_state (priv->mi) == MPD_PLAYER_PLAY;
}

gboolean
xfmpc_mpdclient_is_paused (XfmpcMpdclient *mpdclient)
{
  XfmpcMpdclientPrivate *priv = XFMPC_MPDCLIENT (mpdclient)->priv;
  return mpd_player_get_state (priv->mi) == MPD_PLAYER_PAUSE;
}

gboolean
xfmpc_mpdclient_is_stopped (XfmpcMpdclient *mpdclient)
{
  XfmpcMpdclientPrivate *priv = XFMPC_MPDCLIENT (mpdclient)->priv;
  return mpd_player_get_state (priv->mi) == MPD_PLAYER_STOP;
}



void
xfmpc_mpdclient_update_status (XfmpcMpdclient *mpdclient)
{
  XfmpcMpdclientPrivate *priv = XFMPC_MPDCLIENT (mpdclient)->priv;
  mpd_status_update (priv->mi);
}

static void
cb_status_changed (MpdObj *mi,
                   ChangedStatusType what,
                   gpointer user_data)
{
  XfmpcMpdclient *mpdclient = XFMPC_MPDCLIENT (user_data);
  g_return_if_fail (NULL != user_data);

  if (what & MPD_CST_DATABASE)
    g_signal_emit (mpdclient, signals[SIG_DATABASE_CHANGED], 0);

  if (what & MPD_CST_PLAYLIST)
    g_signal_emit (mpdclient, signals[SIG_PLAYLIST_CHANGED], 0);

  if (what & MPD_CST_SONGID && !(what & MPD_CST_DATABASE) && !(what & MPD_CST_PLAYLIST))
    g_signal_emit (mpdclient, signals[SIG_SONG_CHANGED], 0);

  if (what & MPD_CST_STATE)
    {
      gint state = mpd_player_get_state (mi);
      if (state == MPD_PLAYER_STOP)
        g_signal_emit (mpdclient, signals[SIG_STOPPED], 0);
      else if (state == MPD_PLAYER_PLAY || state == MPD_PLAYER_PAUSE)
        g_signal_emit (mpdclient, signals[SIG_PP_CHANGED], 0,
                       state == MPD_PLAYER_PLAY);
    }

  if (what & MPD_CST_VOLUME)
    g_signal_emit (mpdclient, signals[SIG_VOLUME_CHANGED], 0,
                   xfmpc_mpdclient_get_volume (mpdclient));

  if (what & MPD_CST_ELAPSED_TIME)
    g_signal_emit (mpdclient, signals[SIG_TIME_CHANGED], 0,
                   xfmpc_mpdclient_get_time (mpdclient));

  if (what & MPD_CST_TOTAL_TIME)
    g_signal_emit (mpdclient, signals[SIG_TOTAL_TIME_CHANGED], 0,
                   xfmpc_mpdclient_get_total_time (mpdclient));

  if (what & MPD_CST_REPEAT)
    g_signal_emit (mpdclient, signals[SIG_REPEAT], 0,
                   xfmpc_mpdclient_get_repeat (mpdclient));

  if (what & MPD_CST_RANDOM)
    g_signal_emit (mpdclient, signals[SIG_RANDOM], 0,
                   xfmpc_mpdclient_get_random (mpdclient));

  if (what & MPD_CST_SINGLE_MODE)
    g_signal_emit (mpdclient, signals[SIG_SINGLE], 0,
                   xfmpc_mpdclient_get_single (mpdclient));

  if (what & MPD_CST_CONSUME_MODE)
    g_signal_emit (mpdclient, signals[SIG_CONSUME], 0,
                   xfmpc_mpdclient_get_consume (mpdclient));
}



gboolean
xfmpc_mpdclient_queue_commit (XfmpcMpdclient *mpdclient)
{
  XfmpcMpdclientPrivate *priv = XFMPC_MPDCLIENT (mpdclient)->priv;

  if (mpd_playlist_queue_commit (priv->mi) != MPD_OK)
    return FALSE;

  return TRUE;
}

gboolean
xfmpc_mpdclient_queue_add (XfmpcMpdclient *mpdclient,
                           const gchar *path)
{
  XfmpcMpdclientPrivate *priv = XFMPC_MPDCLIENT (mpdclient)->priv;

  if (mpd_playlist_queue_add (priv->mi, (gchar *)path) != MPD_OK)
    return FALSE;

  return TRUE;
}

gboolean
xfmpc_mpdclient_queue_remove_id (XfmpcMpdclient *mpdclient,
                                 gint id)
{
  XfmpcMpdclientPrivate *priv = XFMPC_MPDCLIENT (mpdclient)->priv;

  if (mpd_playlist_queue_delete_id (priv->mi, id) != MPD_OK)
    return FALSE;

  return TRUE;
}

gboolean
xfmpc_mpdclient_queue_clear (XfmpcMpdclient *mpdclient)
{
  gint id;

  while (xfmpc_mpdclient_playlist_read (mpdclient, &id, NULL, NULL, NULL, NULL))
    {
      if (!xfmpc_mpdclient_queue_remove_id (mpdclient, id))
        return FALSE;
    }

  return TRUE;
}



gboolean
xfmpc_mpdclient_playlist_read (XfmpcMpdclient *mpdclient,
                               gint *id,
                               gint *pos,
                               gchar **filename,
                               gchar **song,
                               gchar **length)
{
  static MpdData       *data = NULL;
  XfmpcMpdclientPrivate *priv = XFMPC_MPDCLIENT (mpdclient)->priv;

  if (NULL == data)
    data = mpd_playlist_get_changes (priv->mi, -1);
  else
    data = mpd_data_get_next (data);

  if (NULL != data)
    {
      if (NULL != filename)
        *filename = g_strdup (data->song->file);
      if (NULL != song)
        *song = _get_formatted_name (data->song);
      if (NULL != length)
        *length = g_strdup_printf ("%d:%02d", data->song->time / 60, data->song->time % 60);
      if (NULL != pos)
        *pos = data->song->pos;
      *id = data->song->id;
    }

  return NULL != data;
}

gboolean
xfmpc_mpdclient_playlist_clear (XfmpcMpdclient *mpdclient)
{
  XfmpcMpdclientPrivate *priv = XFMPC_MPDCLIENT (mpdclient)->priv;

  if (mpd_playlist_clear (priv->mi) != FALSE)
    return FALSE;

  return TRUE;
}

gint
xfmpc_mpdclient_playlist_get_length (XfmpcMpdclient *mpdclient)
{
  return mpd_playlist_get_playlist_length (mpdclient->priv->mi);
}

gint
xfmpc_mpdclient_playlist_get_total_time (XfmpcMpdclient *mpdclient)
{
  XfmpcMpdclientPrivate *priv = mpdclient->priv;
  MpdData               *data;
  gint                   seconds;

  for (seconds = 0, data = mpd_playlist_get_changes (priv->mi, -1);
       data != NULL; data = mpd_data_get_next (data))
    seconds += data->song->time;

  return seconds;
}



gboolean
xfmpc_mpdclient_database_refresh (XfmpcMpdclient *mpdclient)
{
  XfmpcMpdclientPrivate *priv = XFMPC_MPDCLIENT (mpdclient)->priv;

  if (mpd_database_update_dir (priv->mi, "/") != MPD_OK)
    return FALSE;

  return TRUE;
}

gboolean
xfmpc_mpdclient_database_read (XfmpcMpdclient *mpdclient,
                               const gchar *dir,
                               gchar **filename,
                               gchar **basename,
                               gboolean *is_dir)
{
  XfmpcMpdclientPrivate *priv = XFMPC_MPDCLIENT (mpdclient)->priv;
  static MpdData        *data = NULL;

  if (NULL == data)
    data = mpd_database_get_directory (priv->mi, (gchar *)dir);
  else
    data = mpd_data_get_next (data);

  if (NULL != data)
    {
      switch (data->type)
        {
        case MPD_DATA_TYPE_DIRECTORY:
          *is_dir = TRUE;
          *filename = g_strdup (data->directory);
          *basename = g_path_get_basename (data->directory);
          break;

        case MPD_DATA_TYPE_SONG:
          *is_dir = FALSE;
          *filename = g_strdup (data->song->file);
          *basename = _get_formatted_name (data->song);
          break;

        case MPD_DATA_TYPE_PLAYLIST:
#if 0
          *is_dir = FALSE;
          *filename = g_strdup (data->playlist);
          *basename = g_strconcat ("Playlist: ", data->playlist, NULL);
          break;
#endif

        default:
          return xfmpc_mpdclient_database_read (mpdclient, dir, filename, basename, is_dir);
          break;
        }
    }

  return NULL != data;
}

gboolean
xfmpc_mpdclient_database_search (XfmpcMpdclient *mpdclient,
                                 const gchar *query,
                                 gchar **filename,
                                 gchar **basename)
{
  XfmpcMpdclientPrivate *priv = XFMPC_MPDCLIENT (mpdclient)->priv;
  static MpdData        *data = NULL;
  gchar                **queries;
  gint                   i;

  if (NULL == data)
    {
      queries = g_strsplit (query, " ", -1);

      mpd_database_search_start (priv->mi, FALSE);
      for (i = 0; queries[i] != NULL; i++)
        mpd_database_search_add_constraint (priv->mi, MPD_TAG_ITEM_ANY, queries[i]);
      data = mpd_database_search_commit (priv->mi);

      g_strfreev (queries);
    }
  else
    data = mpd_data_get_next (data);

  if (NULL != data)
    {
      *filename = g_strdup (data->song->file);
      *basename = _get_formatted_name (data->song);
    }

  return NULL != data;
}



void
xfmpc_mpdclient_reload (XfmpcMpdclient *mpdclient)
{
  g_signal_emit (mpdclient, signals[SIG_DATABASE_CHANGED], 0);
  g_signal_emit (mpdclient, signals[SIG_PLAYLIST_CHANGED], 0);
}



static gchar *
_get_formatted_name (mpd_Song *song)
{
  XfmpcPreferences *preferences = xfmpc_preferences_get_default ();
  XfmpcPreferencesSongFormat song_format;
  gchar *format_custom;
  gchar *formatted_name;

  g_object_get (preferences,
                "song-format", &song_format,
                "song-format-custom", &format_custom,
                NULL);

  if (NULL == song->title)
    {
      formatted_name = g_path_get_basename (song->file);
    }
  else if (song_format == XFMPC_PREFERENCES_SONG_FORMAT_CUSTOM_FORMAT)
    {
      formatted_name = _get_formatted_name_custom (song, format_custom);
    }
  else
    {
      formatted_name = _get_formatted_name_predefined (song, song_format);
    }

  g_free (format_custom);
  return formatted_name;
}

static gchar *
_get_formatted_name_predefined (mpd_Song *song,
                                XfmpcPreferencesSongFormat song_format)
{
  gchar *formatted_name, *tmp;

  switch (song_format)
    {
    case XFMPC_PREFERENCES_SONG_FORMAT_TITLE:
      formatted_name = g_strdup_printf ("%s", song->title);
      break;

    case XFMPC_PREFERENCES_SONG_FORMAT_ARTIST_TITLE:
      if (NULL != song->artist)
        {
          formatted_name = g_strdup_printf ("%s - %s", song->artist, song->title);
        }
      else
        {
          formatted_name = g_strdup_printf ("%s", song->title);
        }
      break;

    case XFMPC_PREFERENCES_SONG_FORMAT_ALBUM_TITLE:
      if (NULL != song->album)
        {
          formatted_name = g_strdup_printf ("%s - %s", song->album, song->title);
        }
      else
        {
          formatted_name = g_strdup_printf ("%s", song->title);
        }
      break;

    case XFMPC_PREFERENCES_SONG_FORMAT_ARTIST_TITLE_DATE:
      if (NULL != song->artist && NULL != song->date)
        {
          formatted_name = g_strdup_printf ("%s - %s (%s)", song->artist, song->title, song->date);
        }
      else if (NULL != song->date)
        {
          formatted_name = g_strdup_printf ("%s (%s)", song->title, song->date);
        }
      else if (NULL != song->artist)
        {
          formatted_name = g_strdup_printf ("%s - %s", song->artist, song->title);
        }
      else
        {
          formatted_name = g_strdup_printf ("%s", song->title);
        }
      break;

    case XFMPC_PREFERENCES_SONG_FORMAT_ARTIST_ALBUM_TITLE:
      if (NULL != song->artist && NULL != song->album)
        {
          formatted_name = g_strdup_printf ("%s - %s - %s", song->artist, song->album, song->title);
        }
      else if (NULL != song->album)
        {
          formatted_name = g_strdup_printf ("%s - %s", song->album, song->title);
        }
      else if (NULL != song->artist)
        {
          formatted_name = g_strdup_printf ("%s - %s", song->artist, song->title);
        }
      else
        {
          formatted_name = g_strdup_printf ("%s", song->title);
        }
      break;

    case XFMPC_PREFERENCES_SONG_FORMAT_ARTIST_ALBUM_TRACK_TITLE:
      formatted_name = g_strdup ("");

      if (song->artist != NULL)
        {
          tmp = g_strconcat (formatted_name, song->artist, " - ", NULL);
          g_free (formatted_name);
          formatted_name = tmp;
        }

      if (song->album != NULL)
        {
          tmp = g_strconcat (formatted_name, song->album, " - ", NULL);
          g_free (formatted_name);
          formatted_name = tmp;
        }

      if (song->track != NULL)
        {
          tmp = g_strconcat (formatted_name, song->track, ". ", NULL);
          g_free (formatted_name);
          formatted_name = tmp;
        }

      tmp = g_strconcat (formatted_name, song->title, NULL);
      g_free (formatted_name);
      formatted_name = tmp;
      break;

    default:
      g_critical ("Unsupported song format (%d)", song_format);
      formatted_name = g_path_get_basename (song->file);
      break;
    }

  return formatted_name;
}

static gchar *
_get_formatted_name_custom (mpd_Song *song,
                            const gchar *format)
{
  gchar *formatted_name, *tmp;

  g_return_val_if_fail (format != NULL, NULL);

  formatted_name = g_strdup ("");

  do
    {
      if (*format == '%')
        {
          format ++;

          switch (*(format))
            {
            case 'a':
              tmp = g_strconcat (formatted_name, song->artist, NULL);
              break;

            case 'A':
              tmp = g_strconcat (formatted_name, song->album, NULL);
              break;

            case 'd':
              tmp = g_strconcat (formatted_name, song->date, NULL);
              break;

            case 'D':
              tmp = g_strconcat (formatted_name, song->disc, NULL);
              break;

            case 'f':
              tmp = g_strconcat (formatted_name, song->file, NULL);
              break;

            case 'g':
              tmp = g_strconcat (formatted_name, song->genre, NULL);
              break;

            case 't':
              tmp = g_strconcat (formatted_name, song->title, NULL);
              break;

            case 'T':
              tmp = g_strconcat (formatted_name, song->track, NULL);
              break;

            default:
              tmp = g_strconcat (formatted_name, "%", NULL);
              format --;
              break;
            }

          g_free (formatted_name);
          formatted_name = tmp;
        }
      else
        {
          tmp = g_strdup_printf ("%s%c", formatted_name, *format);
          g_free (formatted_name);
          formatted_name = tmp;
        }
    }
  while (*format ++);

  return formatted_name;
}

XfmpcSongInfo *
xfmpc_mpdclient_get_song_info (XfmpcMpdclient *mpdclient, gint id)
{
  XfmpcMpdclientPrivate *priv = XFMPC_MPDCLIENT (mpdclient)->priv;
  XfmpcSongInfo *song_info = g_slice_new0 (XfmpcSongInfo);
  mpd_Song *song = mpd_playlist_get_song (priv->mi, id);

  song_info->filename = song->file;
  song_info->artist = song->artist;
  song_info->title = song->title;
  song_info->album = song->album;
  song_info->track = song->track;
  song_info->date = song->date;
  song_info->genre = song->genre;

  return song_info;
}

void
xfmpc_song_info_free (XfmpcSongInfo *song_info)
{
  g_free (song_info->filename);
  g_free (song_info->artist);
  g_free (song_info->title);
  g_free (song_info->album);
  g_free (song_info->date);
  g_free (song_info->track);
  g_free (song_info->genre);

  g_slice_free (XfmpcSongInfo, song_info);
}

