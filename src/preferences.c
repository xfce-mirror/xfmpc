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

#include <glib.h>
#include <glib-object.h>
#include <stdlib.h>
#include <string.h>
#include <libxfce4util/libxfce4util.h>


#define XFMPC_TYPE_PREFERENCES (xfmpc_preferences_get_type ())
#define XFMPC_PREFERENCES(obj) (G_TYPE_CHECK_INSTANCE_CAST ((obj), XFMPC_TYPE_PREFERENCES, XfmpcPreferences))
#define XFMPC_PREFERENCES_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST ((klass), XFMPC_TYPE_PREFERENCES, XfmpcPreferencesClass))
#define XFMPC_IS_PREFERENCES(obj) (G_TYPE_CHECK_INSTANCE_TYPE ((obj), XFMPC_TYPE_PREFERENCES))
#define XFMPC_IS_PREFERENCES_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), XFMPC_TYPE_PREFERENCES))
#define XFMPC_PREFERENCES_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj), XFMPC_TYPE_PREFERENCES, XfmpcPreferencesClass))

typedef struct _XfmpcPreferences XfmpcPreferences;
typedef struct _XfmpcPreferencesClass XfmpcPreferencesClass;
typedef struct _XfmpcPreferencesPrivate XfmpcPreferencesPrivate;

#define XFMPC_PREFERENCES_TYPE_SONG_FORMAT (xfmpc_preferences_song_format_get_type ())

struct _XfmpcPreferences {
	GObject parent_instance;
	XfmpcPreferencesPrivate * priv;
};

struct _XfmpcPreferencesClass {
	GObjectClass parent_class;
};

typedef enum  {
	XFMPC_PREFERENCES_SONG_FORMAT_TITLE,
	XFMPC_PREFERENCES_SONG_FORMAT_ALBUM_TITLE,
	XFMPC_PREFERENCES_SONG_FORMAT_ARTIST_TITLE,
	XFMPC_PREFERENCES_SONG_FORMAT_ARTIST_TITLE_DATE,
	XFMPC_PREFERENCES_SONG_FORMAT_ARTIST_ALBUM_TITLE,
	XFMPC_PREFERENCES_SONG_FORMAT_ARTIST_ALBUM_TRACK_TITLE,
	XFMPC_PREFERENCES_SONG_FORMAT_CUSTOM_FORMAT
} XfmpcPreferencesSongFormat;

struct _XfmpcPreferencesPrivate {
	gint _last_window_posx;
	gint _last_window_posy;
	gint _last_window_width;
	gint _last_window_height;
	gboolean _last_window_state_sticky;
	gboolean _playlist_autocenter;
	char* _dbbrowser_last_path;
	char* _mpd_hostname;
	gint _mpd_port;
	char* _mpd_password;
	gboolean _mpd_use_defaults;
	gboolean _show_statusbar;
	XfmpcPreferencesSongFormat _song_format;
	char* _song_format_custom;
};


static XfmpcPreferences* xfmpc_preferences_preferences;
static XfmpcPreferences* xfmpc_preferences_preferences = NULL;
static gpointer xfmpc_preferences_parent_class = NULL;

GType xfmpc_preferences_get_type (void);
GType xfmpc_preferences_song_format_get_type (void);
#define XFMPC_PREFERENCES_GET_PRIVATE(o) (G_TYPE_INSTANCE_GET_PRIVATE ((o), XFMPC_TYPE_PREFERENCES, XfmpcPreferencesPrivate))
enum  {
	XFMPC_PREFERENCES_DUMMY_PROPERTY,
	XFMPC_PREFERENCES_LAST_WINDOW_POSX,
	XFMPC_PREFERENCES_LAST_WINDOW_POSY,
	XFMPC_PREFERENCES_LAST_WINDOW_WIDTH,
	XFMPC_PREFERENCES_LAST_WINDOW_HEIGHT,
	XFMPC_PREFERENCES_LAST_WINDOW_STATE_STICKY,
	XFMPC_PREFERENCES_PLAYLIST_AUTOCENTER,
	XFMPC_PREFERENCES_DBBROWSER_LAST_PATH,
	XFMPC_PREFERENCES_MPD_HOSTNAME,
	XFMPC_PREFERENCES_MPD_PORT,
	XFMPC_PREFERENCES_MPD_PASSWORD,
	XFMPC_PREFERENCES_MPD_USE_DEFAULTS,
	XFMPC_PREFERENCES_SHOW_STATUSBAR,
	XFMPC_PREFERENCES_SONG_FORMAT,
	XFMPC_PREFERENCES_SONG_FORMAT_CUSTOM
};
static void xfmpc_preferences_load (XfmpcPreferences* self);
XfmpcPreferences* xfmpc_preferences_new (void);
XfmpcPreferences* xfmpc_preferences_construct (GType object_type);
XfmpcPreferences* xfmpc_preferences_get_default (void);
static void xfmpc_preferences_store (XfmpcPreferences* self);
gint xfmpc_preferences_get_last_window_posx (XfmpcPreferences* self);
void xfmpc_preferences_set_last_window_posx (XfmpcPreferences* self, gint value);
gint xfmpc_preferences_get_last_window_posy (XfmpcPreferences* self);
void xfmpc_preferences_set_last_window_posy (XfmpcPreferences* self, gint value);
gint xfmpc_preferences_get_last_window_width (XfmpcPreferences* self);
void xfmpc_preferences_set_last_window_width (XfmpcPreferences* self, gint value);
gint xfmpc_preferences_get_last_window_height (XfmpcPreferences* self);
void xfmpc_preferences_set_last_window_height (XfmpcPreferences* self, gint value);
gboolean xfmpc_preferences_get_last_window_state_sticky (XfmpcPreferences* self);
void xfmpc_preferences_set_last_window_state_sticky (XfmpcPreferences* self, gboolean value);
gboolean xfmpc_preferences_get_playlist_autocenter (XfmpcPreferences* self);
void xfmpc_preferences_set_playlist_autocenter (XfmpcPreferences* self, gboolean value);
const char* xfmpc_preferences_get_dbbrowser_last_path (XfmpcPreferences* self);
void xfmpc_preferences_set_dbbrowser_last_path (XfmpcPreferences* self, const char* value);
const char* xfmpc_preferences_get_mpd_hostname (XfmpcPreferences* self);
void xfmpc_preferences_set_mpd_hostname (XfmpcPreferences* self, const char* value);
gint xfmpc_preferences_get_mpd_port (XfmpcPreferences* self);
void xfmpc_preferences_set_mpd_port (XfmpcPreferences* self, gint value);
const char* xfmpc_preferences_get_mpd_password (XfmpcPreferences* self);
void xfmpc_preferences_set_mpd_password (XfmpcPreferences* self, const char* value);
gboolean xfmpc_preferences_get_mpd_use_defaults (XfmpcPreferences* self);
void xfmpc_preferences_set_mpd_use_defaults (XfmpcPreferences* self, gboolean value);
gboolean xfmpc_preferences_get_show_statusbar (XfmpcPreferences* self);
void xfmpc_preferences_set_show_statusbar (XfmpcPreferences* self, gboolean value);
XfmpcPreferencesSongFormat xfmpc_preferences_get_song_format (XfmpcPreferences* self);
void xfmpc_preferences_set_song_format (XfmpcPreferences* self, XfmpcPreferencesSongFormat value);
const char* xfmpc_preferences_get_song_format_custom (XfmpcPreferences* self);
void xfmpc_preferences_set_song_format_custom (XfmpcPreferences* self, const char* value);
static GObject * xfmpc_preferences_constructor (GType type, guint n_construct_properties, GObjectConstructParam * construct_properties);
static void xfmpc_preferences_finalize (GObject* obj);
static void xfmpc_preferences_get_property (GObject * object, guint property_id, GValue * value, GParamSpec * pspec);
static void xfmpc_preferences_set_property (GObject * object, guint property_id, const GValue * value, GParamSpec * pspec);
static int _vala_strcmp0 (const char * str1, const char * str2);




GType xfmpc_preferences_song_format_get_type (void) {
	static GType xfmpc_preferences_song_format_type_id = 0;
	if (G_UNLIKELY (xfmpc_preferences_song_format_type_id == 0)) {
		static const GEnumValue values[] = {{XFMPC_PREFERENCES_SONG_FORMAT_TITLE, "XFMPC_PREFERENCES_SONG_FORMAT_TITLE", "title"}, {XFMPC_PREFERENCES_SONG_FORMAT_ALBUM_TITLE, "XFMPC_PREFERENCES_SONG_FORMAT_ALBUM_TITLE", "album-title"}, {XFMPC_PREFERENCES_SONG_FORMAT_ARTIST_TITLE, "XFMPC_PREFERENCES_SONG_FORMAT_ARTIST_TITLE", "artist-title"}, {XFMPC_PREFERENCES_SONG_FORMAT_ARTIST_TITLE_DATE, "XFMPC_PREFERENCES_SONG_FORMAT_ARTIST_TITLE_DATE", "artist-title-date"}, {XFMPC_PREFERENCES_SONG_FORMAT_ARTIST_ALBUM_TITLE, "XFMPC_PREFERENCES_SONG_FORMAT_ARTIST_ALBUM_TITLE", "artist-album-title"}, {XFMPC_PREFERENCES_SONG_FORMAT_ARTIST_ALBUM_TRACK_TITLE, "XFMPC_PREFERENCES_SONG_FORMAT_ARTIST_ALBUM_TRACK_TITLE", "artist-album-track-title"}, {XFMPC_PREFERENCES_SONG_FORMAT_CUSTOM_FORMAT, "XFMPC_PREFERENCES_SONG_FORMAT_CUSTOM_FORMAT", "custom-format"}, {0, NULL, NULL}};
		xfmpc_preferences_song_format_type_id = g_enum_register_static ("XfmpcPreferencesSongFormat", values);
	}
	return xfmpc_preferences_song_format_type_id;
}


XfmpcPreferences* xfmpc_preferences_construct (GType object_type) {
	XfmpcPreferences * self;
	self = g_object_newv (object_type, 0, NULL);
	xfmpc_preferences_load (self);
	return self;
}


XfmpcPreferences* xfmpc_preferences_new (void) {
	return xfmpc_preferences_construct (XFMPC_TYPE_PREFERENCES);
}


XfmpcPreferences* xfmpc_preferences_get_default (void) {
	XfmpcPreferences* result;
	if (xfmpc_preferences_preferences == NULL) {
		XfmpcPreferences* _tmp0_;
		_tmp0_ = NULL;
		xfmpc_preferences_preferences = (_tmp0_ = xfmpc_preferences_new (), (xfmpc_preferences_preferences == NULL) ? NULL : (xfmpc_preferences_preferences = (g_object_unref (xfmpc_preferences_preferences), NULL)), _tmp0_);
		g_object_add_weak_pointer ((GObject*) xfmpc_preferences_preferences, &xfmpc_preferences_preferences);
	} else {
		g_object_ref ((GObject*) xfmpc_preferences_preferences);
	}
	result = xfmpc_preferences_preferences;
	return result;
}


static void xfmpc_preferences_load (XfmpcPreferences* self) {
	XfceRc* rc;
	char* entry;
	GValue _tmp0_ = {0};
	GValue src;
	g_return_if_fail (self != NULL);
	rc = xfce_rc_config_open (XFCE_RESOURCE_CONFIG, "xfce4/xfmpcrc", FALSE);
	if (rc == NULL) {
		g_warning ("preferences.vala:146: Failed to load the preferences");
		(rc == NULL) ? NULL : (rc = (xfce_rc_close (rc), NULL));
		return;
	}
	g_object_freeze_notify ((GObject*) self);
	xfce_rc_set_group (rc, "Configuration");
	entry = NULL;
	src = (g_value_init (&_tmp0_, G_TYPE_STRING), _tmp0_);
	{
		gint _tmp1_;
		GParamSpec** spec_collection;
		int spec_collection_length1;
		int spec_it;
		spec_collection = g_object_class_list_properties ((GObjectClass*) g_type_class_peek (G_TYPE_FROM_INSTANCE ((GObject*) self)), &_tmp1_);
		spec_collection_length1 = _tmp1_;
		for (spec_it = 0; spec_it < _tmp1_; spec_it = spec_it + 1) {
			GParamSpec* _tmp4_;
			GParamSpec* spec;
			_tmp4_ = NULL;
			spec = (_tmp4_ = spec_collection[spec_it], (_tmp4_ == NULL) ? NULL : g_param_spec_ref (_tmp4_));
			{
				char* _tmp3_;
				const char* _tmp2_;
				_tmp3_ = NULL;
				_tmp2_ = NULL;
				entry = (_tmp3_ = (_tmp2_ = xfce_rc_read_entry (rc, g_param_spec_get_nick (spec), ""), (_tmp2_ == NULL) ? NULL : g_strdup (_tmp2_)), entry = (g_free (entry), NULL), _tmp3_);
				if (_vala_strcmp0 (entry, "") == 0) {
					(spec == NULL) ? NULL : (spec = (g_param_spec_unref (spec), NULL));
					continue;
				}
				g_value_set_static_string (&src, entry);
				g_object_set_property ((GObject*) self, spec->name, &src);
				(spec == NULL) ? NULL : (spec = (g_param_spec_unref (spec), NULL));
			}
		}
	}
	g_object_thaw_notify ((GObject*) self);
	(rc == NULL) ? NULL : (rc = (xfce_rc_close (rc), NULL));
	entry = (g_free (entry), NULL);
	G_IS_VALUE (&src) ? (g_value_unset (&src), NULL) : NULL;
}


static void xfmpc_preferences_store (XfmpcPreferences* self) {
	XfceRc* rc;
	char* entry;
	GValue _tmp0_ = {0};
	GValue dst;
	g_return_if_fail (self != NULL);
	rc = xfce_rc_config_open (XFCE_RESOURCE_CONFIG, "xfce4/xfmpcrc", FALSE);
	if (rc == NULL) {
		g_warning ("preferences.vala:174: Failed to save the preferences");
		(rc == NULL) ? NULL : (rc = (xfce_rc_close (rc), NULL));
		return;
	}
	xfce_rc_set_group (rc, "Configuration");
	entry = NULL;
	dst = (g_value_init (&_tmp0_, G_TYPE_STRING), _tmp0_);
	{
		gint _tmp1_;
		GParamSpec** spec_collection;
		int spec_collection_length1;
		int spec_it;
		spec_collection = g_object_class_list_properties ((GObjectClass*) g_type_class_peek (G_TYPE_FROM_INSTANCE ((GObject*) self)), &_tmp1_);
		spec_collection_length1 = _tmp1_;
		for (spec_it = 0; spec_it < _tmp1_; spec_it = spec_it + 1) {
			GParamSpec* _tmp5_;
			GParamSpec* spec;
			_tmp5_ = NULL;
			spec = (_tmp5_ = spec_collection[spec_it], (_tmp5_ == NULL) ? NULL : g_param_spec_ref (_tmp5_));
			{
				char* _tmp4_;
				const char* _tmp3_;
				if (spec->value_type == G_TYPE_STRING) {
					g_object_get_property ((GObject*) self, spec->name, &dst);
				} else {
					GValue _tmp2_ = {0};
					GValue src;
					src = (g_value_init (&_tmp2_, spec->value_type), _tmp2_);
					g_object_get_property ((GObject*) self, spec->name, &src);
					g_value_transform (&src, &dst);
					G_IS_VALUE (&src) ? (g_value_unset (&src), NULL) : NULL;
				}
				_tmp4_ = NULL;
				_tmp3_ = NULL;
				entry = (_tmp4_ = (_tmp3_ = g_value_get_string (&dst), (_tmp3_ == NULL) ? NULL : g_strdup (_tmp3_)), entry = (g_free (entry), NULL), _tmp4_);
				if (entry != NULL) {
					xfce_rc_write_entry (rc, g_param_spec_get_nick (spec), entry);
				}
				(spec == NULL) ? NULL : (spec = (g_param_spec_unref (spec), NULL));
			}
		}
	}
	(rc == NULL) ? NULL : (rc = (xfce_rc_close (rc), NULL));
	entry = (g_free (entry), NULL);
	G_IS_VALUE (&dst) ? (g_value_unset (&dst), NULL) : NULL;
}


gint xfmpc_preferences_get_last_window_posx (XfmpcPreferences* self) {
	gint result;
	g_return_val_if_fail (self != NULL, 0);
	result = self->priv->_last_window_posx;
	return result;
}


void xfmpc_preferences_set_last_window_posx (XfmpcPreferences* self, gint value) {
	g_return_if_fail (self != NULL);
	self->priv->_last_window_posx = value;
	xfmpc_preferences_store (self);
	g_object_notify ((GObject *) self, "last-window-posx");
}


gint xfmpc_preferences_get_last_window_posy (XfmpcPreferences* self) {
	gint result;
	g_return_val_if_fail (self != NULL, 0);
	result = self->priv->_last_window_posy;
	return result;
}


void xfmpc_preferences_set_last_window_posy (XfmpcPreferences* self, gint value) {
	g_return_if_fail (self != NULL);
	self->priv->_last_window_posy = value;
	xfmpc_preferences_store (self);
	g_object_notify ((GObject *) self, "last-window-posy");
}


gint xfmpc_preferences_get_last_window_width (XfmpcPreferences* self) {
	gint result;
	g_return_val_if_fail (self != NULL, 0);
	result = self->priv->_last_window_width;
	return result;
}


void xfmpc_preferences_set_last_window_width (XfmpcPreferences* self, gint value) {
	g_return_if_fail (self != NULL);
	self->priv->_last_window_width = value;
	xfmpc_preferences_store (self);
	g_object_notify ((GObject *) self, "last-window-width");
}


gint xfmpc_preferences_get_last_window_height (XfmpcPreferences* self) {
	gint result;
	g_return_val_if_fail (self != NULL, 0);
	result = self->priv->_last_window_height;
	return result;
}


void xfmpc_preferences_set_last_window_height (XfmpcPreferences* self, gint value) {
	g_return_if_fail (self != NULL);
	self->priv->_last_window_height = value;
	xfmpc_preferences_store (self);
	g_object_notify ((GObject *) self, "last-window-height");
}


gboolean xfmpc_preferences_get_last_window_state_sticky (XfmpcPreferences* self) {
	gboolean result;
	g_return_val_if_fail (self != NULL, FALSE);
	result = self->priv->_last_window_state_sticky;
	return result;
}


void xfmpc_preferences_set_last_window_state_sticky (XfmpcPreferences* self, gboolean value) {
	g_return_if_fail (self != NULL);
	self->priv->_last_window_state_sticky = value;
	xfmpc_preferences_store (self);
	g_object_notify ((GObject *) self, "last-window-state-sticky");
}


gboolean xfmpc_preferences_get_playlist_autocenter (XfmpcPreferences* self) {
	gboolean result;
	g_return_val_if_fail (self != NULL, FALSE);
	result = self->priv->_playlist_autocenter;
	return result;
}


void xfmpc_preferences_set_playlist_autocenter (XfmpcPreferences* self, gboolean value) {
	g_return_if_fail (self != NULL);
	self->priv->_playlist_autocenter = value;
	xfmpc_preferences_store (self);
	g_object_notify ((GObject *) self, "playlist-autocenter");
}


const char* xfmpc_preferences_get_dbbrowser_last_path (XfmpcPreferences* self) {
	const char* result;
	g_return_val_if_fail (self != NULL, NULL);
	result = self->priv->_dbbrowser_last_path;
	return result;
}


void xfmpc_preferences_set_dbbrowser_last_path (XfmpcPreferences* self, const char* value) {
	char* _tmp1_;
	const char* _tmp0_;
	g_return_if_fail (self != NULL);
	_tmp1_ = NULL;
	_tmp0_ = NULL;
	self->priv->_dbbrowser_last_path = (_tmp1_ = (_tmp0_ = value, (_tmp0_ == NULL) ? NULL : g_strdup (_tmp0_)), self->priv->_dbbrowser_last_path = (g_free (self->priv->_dbbrowser_last_path), NULL), _tmp1_);
	xfmpc_preferences_store (self);
	g_object_notify ((GObject *) self, "dbbrowser-last-path");
}


const char* xfmpc_preferences_get_mpd_hostname (XfmpcPreferences* self) {
	const char* result;
	g_return_val_if_fail (self != NULL, NULL);
	result = self->priv->_mpd_hostname;
	return result;
}


void xfmpc_preferences_set_mpd_hostname (XfmpcPreferences* self, const char* value) {
	char* _tmp1_;
	const char* _tmp0_;
	g_return_if_fail (self != NULL);
	_tmp1_ = NULL;
	_tmp0_ = NULL;
	self->priv->_mpd_hostname = (_tmp1_ = (_tmp0_ = value, (_tmp0_ == NULL) ? NULL : g_strdup (_tmp0_)), self->priv->_mpd_hostname = (g_free (self->priv->_mpd_hostname), NULL), _tmp1_);
	xfmpc_preferences_store (self);
	g_object_notify ((GObject *) self, "mpd-hostname");
}


gint xfmpc_preferences_get_mpd_port (XfmpcPreferences* self) {
	gint result;
	g_return_val_if_fail (self != NULL, 0);
	result = self->priv->_mpd_port;
	return result;
}


void xfmpc_preferences_set_mpd_port (XfmpcPreferences* self, gint value) {
	g_return_if_fail (self != NULL);
	self->priv->_mpd_port = value;
	xfmpc_preferences_store (self);
	g_object_notify ((GObject *) self, "mpd-port");
}


const char* xfmpc_preferences_get_mpd_password (XfmpcPreferences* self) {
	const char* result;
	g_return_val_if_fail (self != NULL, NULL);
	result = self->priv->_mpd_password;
	return result;
}


void xfmpc_preferences_set_mpd_password (XfmpcPreferences* self, const char* value) {
	char* _tmp1_;
	const char* _tmp0_;
	g_return_if_fail (self != NULL);
	_tmp1_ = NULL;
	_tmp0_ = NULL;
	self->priv->_mpd_password = (_tmp1_ = (_tmp0_ = value, (_tmp0_ == NULL) ? NULL : g_strdup (_tmp0_)), self->priv->_mpd_password = (g_free (self->priv->_mpd_password), NULL), _tmp1_);
	xfmpc_preferences_store (self);
	g_object_notify ((GObject *) self, "mpd-password");
}


gboolean xfmpc_preferences_get_mpd_use_defaults (XfmpcPreferences* self) {
	gboolean result;
	g_return_val_if_fail (self != NULL, FALSE);
	result = self->priv->_mpd_use_defaults;
	return result;
}


void xfmpc_preferences_set_mpd_use_defaults (XfmpcPreferences* self, gboolean value) {
	g_return_if_fail (self != NULL);
	self->priv->_mpd_use_defaults = value;
	xfmpc_preferences_store (self);
	g_object_notify ((GObject *) self, "mpd-use-defaults");
}


gboolean xfmpc_preferences_get_show_statusbar (XfmpcPreferences* self) {
	gboolean result;
	g_return_val_if_fail (self != NULL, FALSE);
	result = self->priv->_show_statusbar;
	return result;
}


void xfmpc_preferences_set_show_statusbar (XfmpcPreferences* self, gboolean value) {
	g_return_if_fail (self != NULL);
	self->priv->_show_statusbar = value;
	xfmpc_preferences_store (self);
	g_object_notify ((GObject *) self, "show-statusbar");
}


XfmpcPreferencesSongFormat xfmpc_preferences_get_song_format (XfmpcPreferences* self) {
	XfmpcPreferencesSongFormat result;
	g_return_val_if_fail (self != NULL, 0);
	result = self->priv->_song_format;
	return result;
}


void xfmpc_preferences_set_song_format (XfmpcPreferences* self, XfmpcPreferencesSongFormat value) {
	g_return_if_fail (self != NULL);
	self->priv->_song_format = value;
	xfmpc_preferences_store (self);
	g_object_notify ((GObject *) self, "song-format");
}


const char* xfmpc_preferences_get_song_format_custom (XfmpcPreferences* self) {
	const char* result;
	g_return_val_if_fail (self != NULL, NULL);
	result = self->priv->_song_format_custom;
	return result;
}


void xfmpc_preferences_set_song_format_custom (XfmpcPreferences* self, const char* value) {
	char* _tmp1_;
	const char* _tmp0_;
	g_return_if_fail (self != NULL);
	_tmp1_ = NULL;
	_tmp0_ = NULL;
	self->priv->_song_format_custom = (_tmp1_ = (_tmp0_ = value, (_tmp0_ == NULL) ? NULL : g_strdup (_tmp0_)), self->priv->_song_format_custom = (g_free (self->priv->_song_format_custom), NULL), _tmp1_);
	xfmpc_preferences_store (self);
	g_object_notify ((GObject *) self, "song-format-custom");
}


static GObject * xfmpc_preferences_constructor (GType type, guint n_construct_properties, GObjectConstructParam * construct_properties) {
	GObject * obj;
	XfmpcPreferencesClass * klass;
	GObjectClass * parent_class;
	XfmpcPreferences * self;
	klass = XFMPC_PREFERENCES_CLASS (g_type_class_peek (XFMPC_TYPE_PREFERENCES));
	parent_class = G_OBJECT_CLASS (g_type_class_peek_parent (klass));
	obj = parent_class->constructor (type, n_construct_properties, construct_properties);
	self = XFMPC_PREFERENCES (obj);
	{
		char* _tmp0_;
		char* _tmp1_;
		char* _tmp2_;
		char* _tmp3_;
		self->priv->_last_window_posx = 200;
		self->priv->_last_window_posy = 100;
		self->priv->_last_window_width = 330;
		self->priv->_last_window_height = 330;
		self->priv->_last_window_state_sticky = TRUE;
		self->priv->_playlist_autocenter = TRUE;
		_tmp0_ = NULL;
		self->priv->_dbbrowser_last_path = (_tmp0_ = g_strdup (""), self->priv->_dbbrowser_last_path = (g_free (self->priv->_dbbrowser_last_path), NULL), _tmp0_);
		_tmp1_ = NULL;
		self->priv->_mpd_hostname = (_tmp1_ = g_strdup ("localhost"), self->priv->_mpd_hostname = (g_free (self->priv->_mpd_hostname), NULL), _tmp1_);
		self->priv->_mpd_port = 6600;
		_tmp2_ = NULL;
		self->priv->_mpd_password = (_tmp2_ = g_strdup (""), self->priv->_mpd_password = (g_free (self->priv->_mpd_password), NULL), _tmp2_);
		self->priv->_mpd_use_defaults = TRUE;
		self->priv->_show_statusbar = TRUE;
		self->priv->_song_format = XFMPC_PREFERENCES_SONG_FORMAT_ARTIST_TITLE;
		_tmp3_ = NULL;
		self->priv->_song_format_custom = (_tmp3_ = g_strdup ("%a - %t"), self->priv->_song_format_custom = (g_free (self->priv->_song_format_custom), NULL), _tmp3_);
	}
	return obj;
}


static void xfmpc_preferences_class_init (XfmpcPreferencesClass * klass) {
	xfmpc_preferences_parent_class = g_type_class_peek_parent (klass);
	g_type_class_add_private (klass, sizeof (XfmpcPreferencesPrivate));
	G_OBJECT_CLASS (klass)->get_property = xfmpc_preferences_get_property;
	G_OBJECT_CLASS (klass)->set_property = xfmpc_preferences_set_property;
	G_OBJECT_CLASS (klass)->constructor = xfmpc_preferences_constructor;
	G_OBJECT_CLASS (klass)->finalize = xfmpc_preferences_finalize;
	g_object_class_install_property (G_OBJECT_CLASS (klass), XFMPC_PREFERENCES_LAST_WINDOW_POSX, g_param_spec_int ("last-window-posx", "last-window-posx", "last-window-posx", G_MININT, G_MAXINT, 0, G_PARAM_STATIC_NAME | G_PARAM_STATIC_NICK | G_PARAM_STATIC_BLURB | G_PARAM_READABLE | G_PARAM_WRITABLE));
	g_object_class_install_property (G_OBJECT_CLASS (klass), XFMPC_PREFERENCES_LAST_WINDOW_POSY, g_param_spec_int ("last-window-posy", "last-window-posy", "last-window-posy", G_MININT, G_MAXINT, 0, G_PARAM_STATIC_NAME | G_PARAM_STATIC_NICK | G_PARAM_STATIC_BLURB | G_PARAM_READABLE | G_PARAM_WRITABLE));
	g_object_class_install_property (G_OBJECT_CLASS (klass), XFMPC_PREFERENCES_LAST_WINDOW_WIDTH, g_param_spec_int ("last-window-width", "last-window-width", "last-window-width", G_MININT, G_MAXINT, 0, G_PARAM_STATIC_NAME | G_PARAM_STATIC_NICK | G_PARAM_STATIC_BLURB | G_PARAM_READABLE | G_PARAM_WRITABLE));
	g_object_class_install_property (G_OBJECT_CLASS (klass), XFMPC_PREFERENCES_LAST_WINDOW_HEIGHT, g_param_spec_int ("last-window-height", "last-window-height", "last-window-height", G_MININT, G_MAXINT, 0, G_PARAM_STATIC_NAME | G_PARAM_STATIC_NICK | G_PARAM_STATIC_BLURB | G_PARAM_READABLE | G_PARAM_WRITABLE));
	g_object_class_install_property (G_OBJECT_CLASS (klass), XFMPC_PREFERENCES_LAST_WINDOW_STATE_STICKY, g_param_spec_boolean ("last-window-state-sticky", "last-window-state-sticky", "last-window-state-sticky", FALSE, G_PARAM_STATIC_NAME | G_PARAM_STATIC_NICK | G_PARAM_STATIC_BLURB | G_PARAM_READABLE | G_PARAM_WRITABLE));
	g_object_class_install_property (G_OBJECT_CLASS (klass), XFMPC_PREFERENCES_PLAYLIST_AUTOCENTER, g_param_spec_boolean ("playlist-autocenter", "playlist-autocenter", "playlist-autocenter", FALSE, G_PARAM_STATIC_NAME | G_PARAM_STATIC_NICK | G_PARAM_STATIC_BLURB | G_PARAM_READABLE | G_PARAM_WRITABLE));
	g_object_class_install_property (G_OBJECT_CLASS (klass), XFMPC_PREFERENCES_DBBROWSER_LAST_PATH, g_param_spec_string ("dbbrowser-last-path", "dbbrowser-last-path", "dbbrowser-last-path", NULL, G_PARAM_STATIC_NAME | G_PARAM_STATIC_NICK | G_PARAM_STATIC_BLURB | G_PARAM_READABLE | G_PARAM_WRITABLE));
	g_object_class_install_property (G_OBJECT_CLASS (klass), XFMPC_PREFERENCES_MPD_HOSTNAME, g_param_spec_string ("mpd-hostname", "mpd-hostname", "mpd-hostname", NULL, G_PARAM_STATIC_NAME | G_PARAM_STATIC_NICK | G_PARAM_STATIC_BLURB | G_PARAM_READABLE | G_PARAM_WRITABLE));
	g_object_class_install_property (G_OBJECT_CLASS (klass), XFMPC_PREFERENCES_MPD_PORT, g_param_spec_int ("mpd-port", "mpd-port", "mpd-port", G_MININT, G_MAXINT, 0, G_PARAM_STATIC_NAME | G_PARAM_STATIC_NICK | G_PARAM_STATIC_BLURB | G_PARAM_READABLE | G_PARAM_WRITABLE));
	g_object_class_install_property (G_OBJECT_CLASS (klass), XFMPC_PREFERENCES_MPD_PASSWORD, g_param_spec_string ("mpd-password", "mpd-password", "mpd-password", NULL, G_PARAM_STATIC_NAME | G_PARAM_STATIC_NICK | G_PARAM_STATIC_BLURB | G_PARAM_READABLE | G_PARAM_WRITABLE));
	g_object_class_install_property (G_OBJECT_CLASS (klass), XFMPC_PREFERENCES_MPD_USE_DEFAULTS, g_param_spec_boolean ("mpd-use-defaults", "mpd-use-defaults", "mpd-use-defaults", FALSE, G_PARAM_STATIC_NAME | G_PARAM_STATIC_NICK | G_PARAM_STATIC_BLURB | G_PARAM_READABLE | G_PARAM_WRITABLE));
	g_object_class_install_property (G_OBJECT_CLASS (klass), XFMPC_PREFERENCES_SHOW_STATUSBAR, g_param_spec_boolean ("show-statusbar", "show-statusbar", "show-statusbar", FALSE, G_PARAM_STATIC_NAME | G_PARAM_STATIC_NICK | G_PARAM_STATIC_BLURB | G_PARAM_READABLE | G_PARAM_WRITABLE));
	g_object_class_install_property (G_OBJECT_CLASS (klass), XFMPC_PREFERENCES_SONG_FORMAT, g_param_spec_enum ("song-format", "song-format", "song-format", XFMPC_PREFERENCES_TYPE_SONG_FORMAT, 0, G_PARAM_STATIC_NAME | G_PARAM_STATIC_NICK | G_PARAM_STATIC_BLURB | G_PARAM_READABLE | G_PARAM_WRITABLE));
	g_object_class_install_property (G_OBJECT_CLASS (klass), XFMPC_PREFERENCES_SONG_FORMAT_CUSTOM, g_param_spec_string ("song-format-custom", "song-format-custom", "song-format-custom", NULL, G_PARAM_STATIC_NAME | G_PARAM_STATIC_NICK | G_PARAM_STATIC_BLURB | G_PARAM_READABLE | G_PARAM_WRITABLE));
}


static void xfmpc_preferences_instance_init (XfmpcPreferences * self) {
	self->priv = XFMPC_PREFERENCES_GET_PRIVATE (self);
}


static void xfmpc_preferences_finalize (GObject* obj) {
	XfmpcPreferences * self;
	self = XFMPC_PREFERENCES (obj);
	self->priv->_dbbrowser_last_path = (g_free (self->priv->_dbbrowser_last_path), NULL);
	self->priv->_mpd_hostname = (g_free (self->priv->_mpd_hostname), NULL);
	self->priv->_mpd_password = (g_free (self->priv->_mpd_password), NULL);
	self->priv->_song_format_custom = (g_free (self->priv->_song_format_custom), NULL);
	G_OBJECT_CLASS (xfmpc_preferences_parent_class)->finalize (obj);
}


GType xfmpc_preferences_get_type (void) {
	static GType xfmpc_preferences_type_id = 0;
	if (xfmpc_preferences_type_id == 0) {
		static const GTypeInfo g_define_type_info = { sizeof (XfmpcPreferencesClass), (GBaseInitFunc) NULL, (GBaseFinalizeFunc) NULL, (GClassInitFunc) xfmpc_preferences_class_init, (GClassFinalizeFunc) NULL, NULL, sizeof (XfmpcPreferences), 0, (GInstanceInitFunc) xfmpc_preferences_instance_init, NULL };
		xfmpc_preferences_type_id = g_type_register_static (G_TYPE_OBJECT, "XfmpcPreferences", &g_define_type_info, 0);
	}
	return xfmpc_preferences_type_id;
}


static void xfmpc_preferences_get_property (GObject * object, guint property_id, GValue * value, GParamSpec * pspec) {
	XfmpcPreferences * self;
	gpointer boxed;
	self = XFMPC_PREFERENCES (object);
	switch (property_id) {
		case XFMPC_PREFERENCES_LAST_WINDOW_POSX:
		g_value_set_int (value, xfmpc_preferences_get_last_window_posx (self));
		break;
		case XFMPC_PREFERENCES_LAST_WINDOW_POSY:
		g_value_set_int (value, xfmpc_preferences_get_last_window_posy (self));
		break;
		case XFMPC_PREFERENCES_LAST_WINDOW_WIDTH:
		g_value_set_int (value, xfmpc_preferences_get_last_window_width (self));
		break;
		case XFMPC_PREFERENCES_LAST_WINDOW_HEIGHT:
		g_value_set_int (value, xfmpc_preferences_get_last_window_height (self));
		break;
		case XFMPC_PREFERENCES_LAST_WINDOW_STATE_STICKY:
		g_value_set_boolean (value, xfmpc_preferences_get_last_window_state_sticky (self));
		break;
		case XFMPC_PREFERENCES_PLAYLIST_AUTOCENTER:
		g_value_set_boolean (value, xfmpc_preferences_get_playlist_autocenter (self));
		break;
		case XFMPC_PREFERENCES_DBBROWSER_LAST_PATH:
		g_value_set_string (value, xfmpc_preferences_get_dbbrowser_last_path (self));
		break;
		case XFMPC_PREFERENCES_MPD_HOSTNAME:
		g_value_set_string (value, xfmpc_preferences_get_mpd_hostname (self));
		break;
		case XFMPC_PREFERENCES_MPD_PORT:
		g_value_set_int (value, xfmpc_preferences_get_mpd_port (self));
		break;
		case XFMPC_PREFERENCES_MPD_PASSWORD:
		g_value_set_string (value, xfmpc_preferences_get_mpd_password (self));
		break;
		case XFMPC_PREFERENCES_MPD_USE_DEFAULTS:
		g_value_set_boolean (value, xfmpc_preferences_get_mpd_use_defaults (self));
		break;
		case XFMPC_PREFERENCES_SHOW_STATUSBAR:
		g_value_set_boolean (value, xfmpc_preferences_get_show_statusbar (self));
		break;
		case XFMPC_PREFERENCES_SONG_FORMAT:
		g_value_set_enum (value, xfmpc_preferences_get_song_format (self));
		break;
		case XFMPC_PREFERENCES_SONG_FORMAT_CUSTOM:
		g_value_set_string (value, xfmpc_preferences_get_song_format_custom (self));
		break;
		default:
		G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
		break;
	}
}


static void xfmpc_preferences_set_property (GObject * object, guint property_id, const GValue * value, GParamSpec * pspec) {
	XfmpcPreferences * self;
	self = XFMPC_PREFERENCES (object);
	switch (property_id) {
		case XFMPC_PREFERENCES_LAST_WINDOW_POSX:
		xfmpc_preferences_set_last_window_posx (self, g_value_get_int (value));
		break;
		case XFMPC_PREFERENCES_LAST_WINDOW_POSY:
		xfmpc_preferences_set_last_window_posy (self, g_value_get_int (value));
		break;
		case XFMPC_PREFERENCES_LAST_WINDOW_WIDTH:
		xfmpc_preferences_set_last_window_width (self, g_value_get_int (value));
		break;
		case XFMPC_PREFERENCES_LAST_WINDOW_HEIGHT:
		xfmpc_preferences_set_last_window_height (self, g_value_get_int (value));
		break;
		case XFMPC_PREFERENCES_LAST_WINDOW_STATE_STICKY:
		xfmpc_preferences_set_last_window_state_sticky (self, g_value_get_boolean (value));
		break;
		case XFMPC_PREFERENCES_PLAYLIST_AUTOCENTER:
		xfmpc_preferences_set_playlist_autocenter (self, g_value_get_boolean (value));
		break;
		case XFMPC_PREFERENCES_DBBROWSER_LAST_PATH:
		xfmpc_preferences_set_dbbrowser_last_path (self, g_value_get_string (value));
		break;
		case XFMPC_PREFERENCES_MPD_HOSTNAME:
		xfmpc_preferences_set_mpd_hostname (self, g_value_get_string (value));
		break;
		case XFMPC_PREFERENCES_MPD_PORT:
		xfmpc_preferences_set_mpd_port (self, g_value_get_int (value));
		break;
		case XFMPC_PREFERENCES_MPD_PASSWORD:
		xfmpc_preferences_set_mpd_password (self, g_value_get_string (value));
		break;
		case XFMPC_PREFERENCES_MPD_USE_DEFAULTS:
		xfmpc_preferences_set_mpd_use_defaults (self, g_value_get_boolean (value));
		break;
		case XFMPC_PREFERENCES_SHOW_STATUSBAR:
		xfmpc_preferences_set_show_statusbar (self, g_value_get_boolean (value));
		break;
		case XFMPC_PREFERENCES_SONG_FORMAT:
		xfmpc_preferences_set_song_format (self, g_value_get_enum (value));
		break;
		case XFMPC_PREFERENCES_SONG_FORMAT_CUSTOM:
		xfmpc_preferences_set_song_format_custom (self, g_value_get_string (value));
		break;
		default:
		G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
		break;
	}
}


static int _vala_strcmp0 (const char * str1, const char * str2) {
	if (str1 == NULL) {
		return -(str1 != str2);
	}
	if (str2 == NULL) {
		return str1 != str2;
	}
	return strcmp (str1, str2);
}




