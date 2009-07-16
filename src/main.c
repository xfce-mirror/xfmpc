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
#include <config.h>
#include <gtk/gtk.h>


#define XFMPC_PREFERENCES_TYPE_SONG_FORMAT (xfmpc_preferences_song_format_get_type ())

#define XFMPC_TYPE_MAIN_WINDOW (xfmpc_main_window_get_type ())
#define XFMPC_MAIN_WINDOW(obj) (G_TYPE_CHECK_INSTANCE_CAST ((obj), XFMPC_TYPE_MAIN_WINDOW, XfmpcMainWindow))
#define XFMPC_MAIN_WINDOW_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST ((klass), XFMPC_TYPE_MAIN_WINDOW, XfmpcMainWindowClass))
#define XFMPC_IS_MAIN_WINDOW(obj) (G_TYPE_CHECK_INSTANCE_TYPE ((obj), XFMPC_TYPE_MAIN_WINDOW))
#define XFMPC_IS_MAIN_WINDOW_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), XFMPC_TYPE_MAIN_WINDOW))
#define XFMPC_MAIN_WINDOW_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj), XFMPC_TYPE_MAIN_WINDOW, XfmpcMainWindowClass))

typedef struct _XfmpcMainWindow XfmpcMainWindow;
typedef struct _XfmpcMainWindowClass XfmpcMainWindowClass;

typedef enum  {
	XFMPC_PREFERENCES_SONG_FORMAT_TITLE,
	XFMPC_PREFERENCES_SONG_FORMAT_ALBUM_TITLE,
	XFMPC_PREFERENCES_SONG_FORMAT_ARTIST_TITLE,
	XFMPC_PREFERENCES_SONG_FORMAT_ARTIST_TITLE_DATE,
	XFMPC_PREFERENCES_SONG_FORMAT_ARTIST_ALBUM_TITLE,
	XFMPC_PREFERENCES_SONG_FORMAT_ARTIST_ALBUM_TRACK_TITLE,
	XFMPC_PREFERENCES_SONG_FORMAT_CUSTOM_FORMAT
} XfmpcPreferencesSongFormat;



void xfmpc_transform_string_to_int (const GValue* src, GValue* dst);
void xfmpc_transform_string_to_boolean (const GValue* src, GValue* dst);
void xfmpc_transform_string_to_enum (const GValue* src, GValue* dst);
GType xfmpc_preferences_song_format_get_type (void);
XfmpcMainWindow* xfmpc_main_window_new (void);
XfmpcMainWindow* xfmpc_main_window_construct (GType object_type);
GType xfmpc_main_window_get_type (void);
gint xfmpc_main (char** args, int args_length1);



void xfmpc_transform_string_to_int (const GValue* src, GValue* dst) {
	g_value_set_int (&(*dst), (gint) strtoul (g_value_get_string (&(*src)), NULL, 0));
}


void xfmpc_transform_string_to_boolean (const GValue* src, GValue* dst) {
	g_value_set_boolean (&(*dst), g_utf8_collate (g_value_get_string (&(*src)), "FALSE") != 0);
}


void xfmpc_transform_string_to_enum (const GValue* src, GValue* dst) {
	GEnumClass* klass;
	gint i;
	GEnumValue* enum_value;
	klass = (GEnumClass*) g_type_class_ref (G_VALUE_TYPE (&(*dst)));
	i = 0;
	enum_value = NULL;
	while (TRUE) {
		char* _tmp1_;
		char* _tmp0_;
		gboolean _tmp2_;
		if (!((enum_value = g_enum_get_value (klass, i)) != NULL)) {
			break;
		}
		_tmp1_ = NULL;
		_tmp0_ = NULL;
		if ((_tmp2_ = strcmp (_tmp0_ = g_utf8_casefold (enum_value->value_name, -1), _tmp1_ = g_utf8_casefold (g_value_get_string (&(*src)), -1)) == 0, _tmp1_ = (g_free (_tmp1_), NULL), _tmp0_ = (g_free (_tmp0_), NULL), _tmp2_)) {
			break;
		}
		i++;
	}
	g_value_set_enum (&(*dst), enum_value->value);
	(klass == NULL) ? NULL : (klass = (g_type_class_unref (klass), NULL));
}


gint xfmpc_main (char** args, int args_length1) {
	gint result;
	XfmpcMainWindow* window;
	xfce_textdomain (GETTEXT_PACKAGE, PACKAGE_LOCALE_DIR, "UTF-8");
	gtk_init (&args_length1, &args);
	g_value_register_transform_func (G_TYPE_STRING, G_TYPE_INT, (GValueTransform) xfmpc_transform_string_to_int);
	g_value_register_transform_func (G_TYPE_STRING, G_TYPE_BOOLEAN, (GValueTransform) xfmpc_transform_string_to_boolean);
	g_value_register_transform_func (G_TYPE_STRING, XFMPC_PREFERENCES_TYPE_SONG_FORMAT, (GValueTransform) xfmpc_transform_string_to_enum);
	window = g_object_ref_sink (xfmpc_main_window_new ());
	gtk_widget_show_all ((GtkWidget*) window);
	gtk_main ();
	result = 0;
	(window == NULL) ? NULL : (window = (g_object_unref (window), NULL));
	return result;
}


int main (int argc, char ** argv) {
	g_type_init ();
	return xfmpc_main (argv, argc);
}




