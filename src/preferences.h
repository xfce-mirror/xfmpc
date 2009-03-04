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

GType                   xfmpc_preferences_get_type              () G_GNUC_CONST;

XfmpcPreferences *      xfmpc_preferences_get                   ();

G_END_DECLS

#endif

