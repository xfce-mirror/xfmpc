/*
 *  Copyright (c) 2008-2009 Mike Massonnet <mmassonnet@xfce.org>
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

#ifndef __XFMPC_PREFERENCES_H__
#define __XFMPC_PREFERENCES_H__

#include <glib-object.h>

G_BEGIN_DECLS

typedef enum
{
  XFMPC_SONG_FORMAT_TITLE,
  XFMPC_SONG_FORMAT_ALBUM_TITLE,
  XFMPC_SONG_FORMAT_ARTIST_TITLE,
  XFMPC_SONG_FORMAT_ARTIST_TITLE_DATE,
  XFMPC_SONG_FORMAT_ARTIST_ALBUM_TITLE,
  XFMPC_SONG_FORMAT_ARTIST_ALBUM_TRACK_TITLE,
  XFMPC_SONG_FORMAT_CUSTOM,
} XfmpcSongFormat;

GType xfmpc_song_format_get_type () G_GNUC_CONST;
#define XFMPC_TYPE_SONG_FORMAT (xfmpc_song_format_get_type ())

#define XFMPC_TYPE_PREFERENCES              (xfmpc_preferences_get_type())

#define XFMPC_PREFERENCES(obj)              (G_TYPE_CHECK_INSTANCE_CAST ((obj), XFMPC_TYPE_PREFERENCES, XfmpcPreferences))
#define XFMPC_PREFERENCES_CLASS(klass)      (G_TYPE_CHECK_CLASS_CAST ((klass), XFMPC_TYPE_PREFERENCES, XfmpcPreferencesClass))

#define XFMPC_IS_PREFERENCES(obj)           (G_TYPE_CHECK_INSTANCE_TYPE ((obj), XFMPC_TYPE_PREFERENCES))
#define XFMPC_IS_PREFERENCES_CLASS(klass)   (G_TYPE_CHECK_CLASS_TYPE ((klass), XFMPC_TYPE_PREFERENCES))

#define XFMPC_PREFERENCES_GET_CLASS(obj)    (G_TYPE_INSTANCE_GET_CLASS ((obj), XFMPC_TYPE_PREFERENCES, XfmpcPreferencesClass))

typedef struct _XfmpcPreferencesClass       XfmpcPreferencesClass;
typedef struct _XfmpcPreferences            XfmpcPreferences;
typedef struct _XfmpcPreferencesPriv        XfmpcPreferencesPriv;

GType                   xfmpc_preferences_get_type               () G_GNUC_CONST;

XfmpcPreferences *      xfmpc_preferences_get                    ();

gint                    xfmpc_preferences_get_last_window_posx   (XfmpcPreferences *preferences);
gint                    xfmpc_preferences_get_last_window_posy   (XfmpcPreferences *preferences);
gint                    xfmpc_preferences_get_last_window_width  (XfmpcPreferences *preferences);
gint                    xfmpc_preferences_get_last_window_height (XfmpcPreferences *preferences);
gboolean                xfmpc_preferences_get_last_window_state_sticky (XfmpcPreferences *preferences);
gboolean                xfmpc_preferences_get_playlist_autocenter (XfmpcPreferences *preferences);
gchar *                 xfmpc_preferences_get_dbbrowser_last_path (XfmpcPreferences *preferences);
gchar *                 xfmpc_preferences_get_mpd_hostname (XfmpcPreferences *preferences);
gint                    xfmpc_preferences_get_mpd_port (XfmpcPreferences *preferences);
gchar *                 xfmpc_preferences_get_mpd_password (XfmpcPreferences *preferences);
gboolean                xfmpc_preferences_get_mpd_use_defaults (XfmpcPreferences *preferences);
gboolean                xfmpc_preferences_get_show_statusbar     (XfmpcPreferences *preferences);
gint                    xfmpc_preferences_get_song_format (XfmpcPreferences *preferences);
gchar *                 xfmpc_preferences_get_song_format_custom (XfmpcPreferences *preferences);

void                    xfmpc_preferences_set_last_window_posx (XfmpcPreferences *preferences, gint src);
void                    xfmpc_preferences_set_last_window_posy (XfmpcPreferences *preferences, gint src);
void                    xfmpc_preferences_set_last_window_width (XfmpcPreferences *preferences, gint src);
void                    xfmpc_preferences_set_last_window_height (XfmpcPreferences *preferences, gint src);
void                    xfmpc_preferences_set_last_window_state_sticky (XfmpcPreferences *preferences, gboolean src);
void                    xfmpc_preferences_set_playlist_autocenter (XfmpcPreferences *preferences, gboolean src);
void                    xfmpc_preferences_set_dbbrowser_last_path (XfmpcPreferences *preferences, gchar *src);
void                    xfmpc_preferences_set_mpd_hostname (XfmpcPreferences *preferences, gchar *src);
void                    xfmpc_preferences_set_mpd_port (XfmpcPreferences *preferences, gint src);
void                    xfmpc_preferences_set_mpd_password (XfmpcPreferences *preferences, gchar *src) ;
void                    xfmpc_preferences_set_mpd_use_defaults (XfmpcPreferences *preferences, gboolean src);
void                    xfmpc_preferences_set_show_statusbar (XfmpcPreferences *preferences, gboolean src);
void                    xfmpc_preferences_set_song_format (XfmpcPreferences *preferences, gint src);
void                    xfmpc_preferences_set_song_format_custom (XfmpcPreferences *preferences, gchar *src);

G_END_DECLS

#endif

