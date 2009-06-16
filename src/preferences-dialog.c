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
#include <libxfcegui4/libxfcegui4.h>
#include <gtk/gtk.h>
#include <mpdclient.h>
#include <stdlib.h>
#include <string.h>
#include <glib/gi18n-lib.h>
#include <pango/pango.h>


#define XFMPC_TYPE_PREFERENCES_DIALOG (xfmpc_preferences_dialog_get_type ())
#define XFMPC_PREFERENCES_DIALOG(obj) (G_TYPE_CHECK_INSTANCE_CAST ((obj), XFMPC_TYPE_PREFERENCES_DIALOG, XfmpcPreferencesDialog))
#define XFMPC_PREFERENCES_DIALOG_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST ((klass), XFMPC_TYPE_PREFERENCES_DIALOG, XfmpcPreferencesDialogClass))
#define XFMPC_IS_PREFERENCES_DIALOG(obj) (G_TYPE_CHECK_INSTANCE_TYPE ((obj), XFMPC_TYPE_PREFERENCES_DIALOG))
#define XFMPC_IS_PREFERENCES_DIALOG_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), XFMPC_TYPE_PREFERENCES_DIALOG))
#define XFMPC_PREFERENCES_DIALOG_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj), XFMPC_TYPE_PREFERENCES_DIALOG, XfmpcPreferencesDialogClass))

typedef struct _XfmpcPreferencesDialog XfmpcPreferencesDialog;
typedef struct _XfmpcPreferencesDialogClass XfmpcPreferencesDialogClass;
typedef struct _XfmpcPreferencesDialogPrivate XfmpcPreferencesDialogPrivate;

#define XFMPC_TYPE_PREFERENCES (xfmpc_preferences_get_type ())
#define XFMPC_PREFERENCES(obj) (G_TYPE_CHECK_INSTANCE_CAST ((obj), XFMPC_TYPE_PREFERENCES, XfmpcPreferences))
#define XFMPC_PREFERENCES_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST ((klass), XFMPC_TYPE_PREFERENCES, XfmpcPreferencesClass))
#define XFMPC_IS_PREFERENCES(obj) (G_TYPE_CHECK_INSTANCE_TYPE ((obj), XFMPC_TYPE_PREFERENCES))
#define XFMPC_IS_PREFERENCES_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), XFMPC_TYPE_PREFERENCES))
#define XFMPC_PREFERENCES_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj), XFMPC_TYPE_PREFERENCES, XfmpcPreferencesClass))

typedef struct _XfmpcPreferences XfmpcPreferences;
typedef struct _XfmpcPreferencesClass XfmpcPreferencesClass;

#define XFMPC_PREFERENCES_TYPE_SONG_FORMAT (xfmpc_preferences_song_format_get_type ())

struct _XfmpcPreferencesDialog {
	XfceTitledDialog parent_instance;
	XfmpcPreferencesDialogPrivate * priv;
};

struct _XfmpcPreferencesDialogClass {
	XfceTitledDialogClass parent_class;
};

struct _XfmpcPreferencesDialogPrivate {
	XfmpcPreferences* preferences;
	GtkCheckButton* entry_use_defaults;
	GtkEntry* entry_host;
	GtkEntry* entry_passwd;
	GtkSpinButton* entry_port;
	GtkCheckButton* show_statusbar;
	GtkComboBox* combo_format;
	GtkEntry* entry_custom;
	guint format_timeout;
	GtkVBox* mpd_vbox;
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



GType xfmpc_preferences_dialog_get_type (void);
GType xfmpc_preferences_get_type (void);
#define XFMPC_PREFERENCES_DIALOG_GET_PRIVATE(o) (G_TYPE_INSTANCE_GET_PRIVATE ((o), XFMPC_TYPE_PREFERENCES_DIALOG, XfmpcPreferencesDialogPrivate))
enum  {
	XFMPC_PREFERENCES_DIALOG_DUMMY_PROPERTY
};
static void xfmpc_preferences_dialog_cb_response (XfmpcPreferencesDialog* self, XfmpcPreferencesDialog* source, gint response);
static void xfmpc_preferences_dialog_cb_use_defaults_toggled (XfmpcPreferencesDialog* self, GtkCheckButton* source);
void xfmpc_preferences_set_mpd_hostname (XfmpcPreferences* self, const char* value);
void xfmpc_preferences_set_mpd_port (XfmpcPreferences* self, gint value);
void xfmpc_preferences_set_mpd_password (XfmpcPreferences* self, const char* value);
void xfmpc_preferences_set_mpd_use_defaults (XfmpcPreferences* self, gboolean value);
static void xfmpc_preferences_dialog_cb_update_mpd (XfmpcPreferencesDialog* self, GtkButton* source);
void xfmpc_preferences_set_show_statusbar (XfmpcPreferences* self, gboolean value);
static void xfmpc_preferences_dialog_cb_show_statusbar_toggled (XfmpcPreferencesDialog* self, GtkCheckButton* source);
GType xfmpc_preferences_song_format_get_type (void);
void xfmpc_preferences_set_song_format (XfmpcPreferences* self, XfmpcPreferencesSongFormat value);
static void xfmpc_preferences_dialog_cb_combo_format_changed (XfmpcPreferencesDialog* self, GtkComboBox* source);
static gboolean xfmpc_preferences_dialog_timeout_format (XfmpcPreferencesDialog* self);
static gboolean _xfmpc_preferences_dialog_timeout_format_gsource_func (gpointer self);
static void xfmpc_preferences_dialog_cb_entry_custom_changed (XfmpcPreferencesDialog* self, GtkEntry* source);
void xfmpc_preferences_set_song_format_custom (XfmpcPreferences* self, const char* value);
XfmpcPreferencesDialog* xfmpc_preferences_dialog_new (void);
XfmpcPreferencesDialog* xfmpc_preferences_dialog_construct (GType object_type);
XfmpcPreferencesDialog* xfmpc_preferences_dialog_new (void);
XfmpcPreferences* xfmpc_preferences_get (void);
gboolean xfmpc_preferences_get_mpd_use_defaults (XfmpcPreferences* self);
static void _xfmpc_preferences_dialog_cb_use_defaults_toggled_gtk_toggle_button_toggled (GtkCheckButton* _sender, gpointer self);
const char* xfmpc_preferences_get_mpd_hostname (XfmpcPreferences* self);
gint xfmpc_preferences_get_mpd_port (XfmpcPreferences* self);
const char* xfmpc_preferences_get_mpd_password (XfmpcPreferences* self);
static void _xfmpc_preferences_dialog_cb_update_mpd_gtk_button_clicked (GtkButton* _sender, gpointer self);
gboolean xfmpc_preferences_get_show_statusbar (XfmpcPreferences* self);
static void _xfmpc_preferences_dialog_cb_show_statusbar_toggled_gtk_toggle_button_toggled (GtkCheckButton* _sender, gpointer self);
XfmpcPreferencesSongFormat xfmpc_preferences_get_song_format (XfmpcPreferences* self);
const char* xfmpc_preferences_get_song_format_custom (XfmpcPreferences* self);
static void _xfmpc_preferences_dialog_cb_combo_format_changed_gtk_combo_box_changed (GtkComboBox* _sender, gpointer self);
static void _xfmpc_preferences_dialog_cb_entry_custom_changed_gtk_editable_changed (GtkEntry* _sender, gpointer self);
static void _xfmpc_preferences_dialog_cb_response_gtk_dialog_response (XfmpcPreferencesDialog* _sender, gint response_id, gpointer self);
static GObject * xfmpc_preferences_dialog_constructor (GType type, guint n_construct_properties, GObjectConstructParam * construct_properties);
static gpointer xfmpc_preferences_dialog_parent_class = NULL;
static void xfmpc_preferences_dialog_finalize (GObject* obj);



/*
 * Signal callbacks
 */
static void xfmpc_preferences_dialog_cb_response (XfmpcPreferencesDialog* self, XfmpcPreferencesDialog* source, gint response) {
	g_return_if_fail (self != NULL);
	g_return_if_fail (source != NULL);
	switch (response) {
		case GTK_RESPONSE_CLOSE:
		{
			gtk_object_destroy ((GtkObject*) self);
			break;
		}
	}
}


static void xfmpc_preferences_dialog_cb_use_defaults_toggled (XfmpcPreferencesDialog* self, GtkCheckButton* source) {
	g_return_if_fail (self != NULL);
	g_return_if_fail (source != NULL);
	gtk_widget_set_sensitive ((GtkWidget*) self->priv->mpd_vbox, !gtk_toggle_button_get_active ((GtkToggleButton*) self->priv->entry_use_defaults));
}


static void xfmpc_preferences_dialog_cb_update_mpd (XfmpcPreferencesDialog* self, GtkButton* source) {
	XfmpcMpdclient* mpdclient;
	g_return_if_fail (self != NULL);
	g_return_if_fail (source != NULL);
	mpdclient = xfmpc_mpdclient_get ();
	xfmpc_preferences_set_mpd_hostname (self->priv->preferences, gtk_entry_get_text (self->priv->entry_host));
	xfmpc_preferences_set_mpd_port (self->priv->preferences, gtk_spin_button_get_value_as_int (self->priv->entry_port));
	xfmpc_preferences_set_mpd_password (self->priv->preferences, gtk_entry_get_text (self->priv->entry_passwd));
	xfmpc_preferences_set_mpd_use_defaults (self->priv->preferences, gtk_toggle_button_get_active ((GtkToggleButton*) self->priv->entry_use_defaults));
	xfmpc_mpdclient_disconnect (mpdclient);
	xfmpc_mpdclient_connect (mpdclient);
}


static void xfmpc_preferences_dialog_cb_show_statusbar_toggled (XfmpcPreferencesDialog* self, GtkCheckButton* source) {
	g_return_if_fail (self != NULL);
	g_return_if_fail (source != NULL);
	xfmpc_preferences_set_show_statusbar (self->priv->preferences, gtk_toggle_button_get_active ((GtkToggleButton*) self->priv->show_statusbar));
}


static void xfmpc_preferences_dialog_cb_combo_format_changed (XfmpcPreferencesDialog* self, GtkComboBox* source) {
	g_return_if_fail (self != NULL);
	g_return_if_fail (source != NULL);
	switch (gtk_combo_box_get_active (self->priv->combo_format)) {
		case 0:
		{
			xfmpc_preferences_set_song_format (self->priv->preferences, XFMPC_PREFERENCES_SONG_FORMAT_TITLE);
			break;
		}
		case 1:
		{
			xfmpc_preferences_set_song_format (self->priv->preferences, XFMPC_PREFERENCES_SONG_FORMAT_ALBUM_TITLE);
			break;
		}
		case 2:
		{
			xfmpc_preferences_set_song_format (self->priv->preferences, XFMPC_PREFERENCES_SONG_FORMAT_ARTIST_TITLE);
			break;
		}
		case 3:
		{
			xfmpc_preferences_set_song_format (self->priv->preferences, XFMPC_PREFERENCES_SONG_FORMAT_ARTIST_TITLE_DATE);
			break;
		}
		case 4:
		{
			xfmpc_preferences_set_song_format (self->priv->preferences, XFMPC_PREFERENCES_SONG_FORMAT_ARTIST_ALBUM_TITLE);
			break;
		}
		case 5:
		{
			xfmpc_preferences_set_song_format (self->priv->preferences, XFMPC_PREFERENCES_SONG_FORMAT_ARTIST_ALBUM_TRACK_TITLE);
			break;
		}
		case 6:
		{
			xfmpc_preferences_set_song_format (self->priv->preferences, XFMPC_PREFERENCES_SONG_FORMAT_CUSTOM_FORMAT);
			break;
		}
		default:
		{
			xfmpc_preferences_set_song_format (self->priv->preferences, XFMPC_PREFERENCES_SONG_FORMAT_TITLE);
			break;
		}
	}
	gtk_widget_set_sensitive ((GtkWidget*) self->priv->entry_custom, gtk_combo_box_get_active (self->priv->combo_format) == 6);
}


static gboolean _xfmpc_preferences_dialog_timeout_format_gsource_func (gpointer self) {
	return xfmpc_preferences_dialog_timeout_format (self);
}


static void xfmpc_preferences_dialog_cb_entry_custom_changed (XfmpcPreferencesDialog* self, GtkEntry* source) {
	g_return_if_fail (self != NULL);
	g_return_if_fail (source != NULL);
	if (self->priv->format_timeout > 0) {
		g_source_remove (self->priv->format_timeout);
	}
	self->priv->format_timeout = g_timeout_add_seconds_full (G_PRIORITY_DEFAULT, (guint) 1, _xfmpc_preferences_dialog_timeout_format_gsource_func, g_object_ref (self), g_object_unref);
}


static gboolean xfmpc_preferences_dialog_timeout_format (XfmpcPreferencesDialog* self) {
	const char* _tmp0_;
	char* custom_format;
	gboolean _tmp1_;
	g_return_val_if_fail (self != NULL, FALSE);
	_tmp0_ = NULL;
	custom_format = (_tmp0_ = gtk_entry_get_text (self->priv->entry_custom), (_tmp0_ == NULL) ? NULL : g_strdup (_tmp0_));
	xfmpc_preferences_set_song_format_custom (self->priv->preferences, custom_format);
	return (_tmp1_ = FALSE, custom_format = (g_free (custom_format), NULL), _tmp1_);
}


XfmpcPreferencesDialog* xfmpc_preferences_dialog_construct (GType object_type) {
	XfmpcPreferencesDialog * self;
	self = g_object_newv (object_type, 0, NULL);
	return self;
}


XfmpcPreferencesDialog* xfmpc_preferences_dialog_new (void) {
	return xfmpc_preferences_dialog_construct (XFMPC_TYPE_PREFERENCES_DIALOG);
}


static void _xfmpc_preferences_dialog_cb_use_defaults_toggled_gtk_toggle_button_toggled (GtkCheckButton* _sender, gpointer self) {
	xfmpc_preferences_dialog_cb_use_defaults_toggled (self, _sender);
}


static void _xfmpc_preferences_dialog_cb_update_mpd_gtk_button_clicked (GtkButton* _sender, gpointer self) {
	xfmpc_preferences_dialog_cb_update_mpd (self, _sender);
}


static void _xfmpc_preferences_dialog_cb_show_statusbar_toggled_gtk_toggle_button_toggled (GtkCheckButton* _sender, gpointer self) {
	xfmpc_preferences_dialog_cb_show_statusbar_toggled (self, _sender);
}


static void _xfmpc_preferences_dialog_cb_combo_format_changed_gtk_combo_box_changed (GtkComboBox* _sender, gpointer self) {
	xfmpc_preferences_dialog_cb_combo_format_changed (self, _sender);
}


static void _xfmpc_preferences_dialog_cb_entry_custom_changed_gtk_editable_changed (GtkEntry* _sender, gpointer self) {
	xfmpc_preferences_dialog_cb_entry_custom_changed (self, _sender);
}


static void _xfmpc_preferences_dialog_cb_response_gtk_dialog_response (XfmpcPreferencesDialog* _sender, gint response_id, gpointer self) {
	xfmpc_preferences_dialog_cb_response (self, _sender, response_id);
}


static GObject * xfmpc_preferences_dialog_constructor (GType type, guint n_construct_properties, GObjectConstructParam * construct_properties) {
	GObject * obj;
	XfmpcPreferencesDialogClass * klass;
	GObjectClass * parent_class;
	XfmpcPreferencesDialog * self;
	klass = XFMPC_PREFERENCES_DIALOG_CLASS (g_type_class_peek (XFMPC_TYPE_PREFERENCES_DIALOG));
	parent_class = G_OBJECT_CLASS (g_type_class_peek_parent (klass));
	obj = parent_class->constructor (type, n_construct_properties, construct_properties);
	self = XFMPC_PREFERENCES_DIALOG (obj);
	{
		GtkNotebook* notebook;
		GtkVBox* vbox;
		GtkLabel* label;
		GtkVBox* vbox2;
		GtkWidget* _tmp0_;
		GtkWidget* frame;
		GtkCheckButton* _tmp1_;
		GtkVBox* _tmp2_;
		GtkHBox* hbox;
		GtkLabel* _tmp3_;
		GtkEntry* _tmp4_;
		GtkLabel* _tmp5_;
		GtkSpinButton* _tmp6_;
		GtkHBox* _tmp7_;
		GtkLabel* _tmp8_;
		GtkEntry* _tmp9_;
		GtkButton* button;
		GtkVBox* _tmp10_;
		GtkLabel* _tmp11_;
		GtkVBox* _tmp12_;
		GtkWidget* _tmp14_;
		GtkWidget* _tmp13_;
		GtkCheckButton* _tmp15_;
		GtkVBox* _tmp16_;
		GtkWidget* _tmp18_;
		GtkWidget* _tmp17_;
		GtkHBox* _tmp19_;
		GtkLabel* _tmp20_;
		GtkComboBox* _tmp21_;
		GtkHBox* _tmp22_;
		GtkLabel* _tmp23_;
		GtkEntry* _tmp24_;
		GtkLabel* _tmp25_;
		GtkTable* table;
		PangoAttrList* attrs;
		GtkLabel* _tmp26_;
		GtkLabel* _tmp27_;
		GtkLabel* _tmp28_;
		GtkLabel* _tmp29_;
		GtkLabel* _tmp30_;
		GtkLabel* _tmp31_;
		GtkLabel* _tmp32_;
		GtkLabel* _tmp33_;
		gtk_dialog_set_has_separator ((GtkDialog*) self, TRUE);
		gtk_window_set_skip_taskbar_hint ((GtkWindow*) self, TRUE);
		gtk_window_set_icon_name ((GtkWindow*) self, "stock_volume");
		gtk_window_set_resizable ((GtkWindow*) self, FALSE);
		gtk_window_set_title ((GtkWindow*) self, "Xfmpc Preferences");
		self->priv->preferences = xfmpc_preferences_get ();
		notebook = g_object_ref_sink ((GtkNotebook*) gtk_notebook_new ());
		gtk_container_set_border_width ((GtkContainer*) notebook, (guint) 6);
		gtk_box_pack_start ((GtkBox*) ((GtkDialog*) self)->vbox, (GtkWidget*) notebook, TRUE, TRUE, (guint) 0);
		/* Mpd Settings */
		vbox = g_object_ref_sink ((GtkVBox*) gtk_vbox_new (FALSE, 6));
		gtk_container_set_border_width ((GtkContainer*) vbox, (guint) 6);
		label = g_object_ref_sink ((GtkLabel*) gtk_label_new (_ ("MPD")));
		gtk_notebook_append_page (notebook, (GtkWidget*) vbox, (GtkWidget*) label);
		vbox2 = g_object_ref_sink ((GtkVBox*) gtk_vbox_new (FALSE, 6));
		_tmp0_ = NULL;
		frame = (_tmp0_ = xfce_create_framebox_with_content (_ ("Connection"), (GtkWidget*) vbox2), (_tmp0_ == NULL) ? NULL : g_object_ref (_tmp0_));
		gtk_box_pack_start ((GtkBox*) vbox, frame, FALSE, FALSE, (guint) 0);
		_tmp1_ = NULL;
		self->priv->entry_use_defaults = (_tmp1_ = g_object_ref_sink ((GtkCheckButton*) gtk_check_button_new_with_mnemonic (_ ("Use _default system settings"))), (self->priv->entry_use_defaults == NULL) ? NULL : (self->priv->entry_use_defaults = (g_object_unref (self->priv->entry_use_defaults), NULL)), _tmp1_);
		gtk_widget_set_tooltip_text ((GtkWidget*) self->priv->entry_use_defaults, _ ("If checked, Xfmpc will try to read the environment variables MPD_HOST and MPD_PORT otherwise it will use localhost"));
		gtk_toggle_button_set_active ((GtkToggleButton*) self->priv->entry_use_defaults, xfmpc_preferences_get_mpd_use_defaults (self->priv->preferences));
		gtk_box_pack_start ((GtkBox*) vbox2, (GtkWidget*) self->priv->entry_use_defaults, FALSE, FALSE, (guint) 0);
		_tmp2_ = NULL;
		self->priv->mpd_vbox = (_tmp2_ = g_object_ref_sink ((GtkVBox*) gtk_vbox_new (FALSE, 6)), (self->priv->mpd_vbox == NULL) ? NULL : (self->priv->mpd_vbox = (g_object_unref (self->priv->mpd_vbox), NULL)), _tmp2_);
		gtk_box_pack_start ((GtkBox*) vbox2, (GtkWidget*) self->priv->mpd_vbox, FALSE, FALSE, (guint) 0);
		g_signal_connect_object ((GtkToggleButton*) self->priv->entry_use_defaults, "toggled", (GCallback) _xfmpc_preferences_dialog_cb_use_defaults_toggled_gtk_toggle_button_toggled, self, 0);
		gtk_widget_set_sensitive ((GtkWidget*) self->priv->mpd_vbox, !gtk_toggle_button_get_active ((GtkToggleButton*) self->priv->entry_use_defaults));
		hbox = g_object_ref_sink ((GtkHBox*) gtk_hbox_new (FALSE, 2));
		gtk_box_pack_start ((GtkBox*) self->priv->mpd_vbox, (GtkWidget*) hbox, FALSE, FALSE, (guint) 0);
		_tmp3_ = NULL;
		label = (_tmp3_ = g_object_ref_sink ((GtkLabel*) gtk_label_new (_ ("Hostname:"))), (label == NULL) ? NULL : (label = (g_object_unref (label), NULL)), _tmp3_);
		gtk_box_pack_start ((GtkBox*) hbox, (GtkWidget*) label, FALSE, FALSE, (guint) 0);
		_tmp4_ = NULL;
		self->priv->entry_host = (_tmp4_ = g_object_ref_sink ((GtkEntry*) gtk_entry_new ()), (self->priv->entry_host == NULL) ? NULL : (self->priv->entry_host = (g_object_unref (self->priv->entry_host), NULL)), _tmp4_);
		gtk_entry_set_width_chars (self->priv->entry_host, 15);
		gtk_entry_set_text (self->priv->entry_host, xfmpc_preferences_get_mpd_hostname (self->priv->preferences));
		gtk_box_pack_start ((GtkBox*) hbox, (GtkWidget*) self->priv->entry_host, TRUE, TRUE, (guint) 0);
		_tmp5_ = NULL;
		label = (_tmp5_ = g_object_ref_sink ((GtkLabel*) gtk_label_new (_ ("Port:"))), (label == NULL) ? NULL : (label = (g_object_unref (label), NULL)), _tmp5_);
		gtk_box_pack_start ((GtkBox*) hbox, (GtkWidget*) label, FALSE, FALSE, (guint) 0);
		_tmp6_ = NULL;
		self->priv->entry_port = (_tmp6_ = g_object_ref_sink ((GtkSpinButton*) gtk_spin_button_new_with_range ((double) 0, (double) 65536, (double) 1)), (self->priv->entry_port == NULL) ? NULL : (self->priv->entry_port = (g_object_unref (self->priv->entry_port), NULL)), _tmp6_);
		gtk_spin_button_set_digits (self->priv->entry_port, (guint) 0);
		gtk_spin_button_set_value (self->priv->entry_port, (double) xfmpc_preferences_get_mpd_port (self->priv->preferences));
		gtk_box_pack_start ((GtkBox*) hbox, (GtkWidget*) self->priv->entry_port, TRUE, TRUE, (guint) 0);
		_tmp7_ = NULL;
		hbox = (_tmp7_ = g_object_ref_sink ((GtkHBox*) gtk_hbox_new (FALSE, 2)), (hbox == NULL) ? NULL : (hbox = (g_object_unref (hbox), NULL)), _tmp7_);
		gtk_box_pack_start ((GtkBox*) self->priv->mpd_vbox, (GtkWidget*) hbox, FALSE, FALSE, (guint) 0);
		_tmp8_ = NULL;
		label = (_tmp8_ = g_object_ref_sink ((GtkLabel*) gtk_label_new (_ ("Password:"))), (label == NULL) ? NULL : (label = (g_object_unref (label), NULL)), _tmp8_);
		gtk_box_pack_start ((GtkBox*) hbox, (GtkWidget*) label, FALSE, FALSE, (guint) 0);
		_tmp9_ = NULL;
		self->priv->entry_passwd = (_tmp9_ = g_object_ref_sink ((GtkEntry*) gtk_entry_new ()), (self->priv->entry_passwd == NULL) ? NULL : (self->priv->entry_passwd = (g_object_unref (self->priv->entry_passwd), NULL)), _tmp9_);
		gtk_entry_set_visibility (self->priv->entry_passwd, FALSE);
		if (xfmpc_preferences_get_mpd_password (self->priv->preferences) != NULL) {
			gtk_entry_set_text (self->priv->entry_passwd, xfmpc_preferences_get_mpd_password (self->priv->preferences));
		}
		gtk_box_pack_start ((GtkBox*) hbox, (GtkWidget*) self->priv->entry_passwd, TRUE, TRUE, (guint) 0);
		button = g_object_ref_sink ((GtkButton*) gtk_button_new_from_stock (GTK_STOCK_APPLY));
		g_signal_connect_object (button, "clicked", (GCallback) _xfmpc_preferences_dialog_cb_update_mpd_gtk_button_clicked, self, 0);
		gtk_box_pack_start ((GtkBox*) vbox2, (GtkWidget*) button, TRUE, TRUE, (guint) 0);
		/* Display */
		_tmp10_ = NULL;
		vbox = (_tmp10_ = g_object_ref_sink ((GtkVBox*) gtk_vbox_new (FALSE, 6)), (vbox == NULL) ? NULL : (vbox = (g_object_unref (vbox), NULL)), _tmp10_);
		gtk_container_set_border_width ((GtkContainer*) vbox, (guint) 6);
		_tmp11_ = NULL;
		label = (_tmp11_ = g_object_ref_sink ((GtkLabel*) gtk_label_new (_ ("Appearance"))), (label == NULL) ? NULL : (label = (g_object_unref (label), NULL)), _tmp11_);
		gtk_notebook_append_page (notebook, (GtkWidget*) vbox, (GtkWidget*) label);
		_tmp12_ = NULL;
		vbox2 = (_tmp12_ = g_object_ref_sink ((GtkVBox*) gtk_vbox_new (FALSE, 6)), (vbox2 == NULL) ? NULL : (vbox2 = (g_object_unref (vbox2), NULL)), _tmp12_);
		_tmp14_ = NULL;
		_tmp13_ = NULL;
		frame = (_tmp14_ = (_tmp13_ = xfce_create_framebox_with_content (_ ("Statusbar"), (GtkWidget*) vbox2), (_tmp13_ == NULL) ? NULL : g_object_ref (_tmp13_)), (frame == NULL) ? NULL : (frame = (g_object_unref (frame), NULL)), _tmp14_);
		gtk_box_pack_start ((GtkBox*) vbox, frame, FALSE, FALSE, (guint) 0);
		_tmp15_ = NULL;
		self->priv->show_statusbar = (_tmp15_ = g_object_ref_sink ((GtkCheckButton*) gtk_check_button_new_with_mnemonic (_ ("Show _stastusbar"))), (self->priv->show_statusbar == NULL) ? NULL : (self->priv->show_statusbar = (g_object_unref (self->priv->show_statusbar), NULL)), _tmp15_);
		gtk_toggle_button_set_active ((GtkToggleButton*) self->priv->show_statusbar, xfmpc_preferences_get_show_statusbar (self->priv->preferences));
		g_signal_connect_object ((GtkToggleButton*) self->priv->show_statusbar, "toggled", (GCallback) _xfmpc_preferences_dialog_cb_show_statusbar_toggled_gtk_toggle_button_toggled, self, 0);
		gtk_box_pack_start ((GtkBox*) vbox2, (GtkWidget*) self->priv->show_statusbar, FALSE, FALSE, (guint) 0);
		_tmp16_ = NULL;
		vbox2 = (_tmp16_ = g_object_ref_sink ((GtkVBox*) gtk_vbox_new (FALSE, 6)), (vbox2 == NULL) ? NULL : (vbox2 = (g_object_unref (vbox2), NULL)), _tmp16_);
		_tmp18_ = NULL;
		_tmp17_ = NULL;
		frame = (_tmp18_ = (_tmp17_ = xfce_create_framebox_with_content (_ ("Song Format"), (GtkWidget*) vbox2), (_tmp17_ == NULL) ? NULL : g_object_ref (_tmp17_)), (frame == NULL) ? NULL : (frame = (g_object_unref (frame), NULL)), _tmp18_);
		gtk_box_pack_start ((GtkBox*) vbox, frame, FALSE, FALSE, (guint) 0);
		_tmp19_ = NULL;
		hbox = (_tmp19_ = g_object_ref_sink ((GtkHBox*) gtk_hbox_new (FALSE, 2)), (hbox == NULL) ? NULL : (hbox = (g_object_unref (hbox), NULL)), _tmp19_);
		_tmp20_ = NULL;
		label = (_tmp20_ = g_object_ref_sink ((GtkLabel*) gtk_label_new (_ ("Song Format:"))), (label == NULL) ? NULL : (label = (g_object_unref (label), NULL)), _tmp20_);
		gtk_box_pack_start ((GtkBox*) hbox, (GtkWidget*) label, FALSE, FALSE, (guint) 0);
		_tmp21_ = NULL;
		self->priv->combo_format = (_tmp21_ = g_object_ref_sink ((GtkComboBox*) gtk_combo_box_new_text ()), (self->priv->combo_format == NULL) ? NULL : (self->priv->combo_format = (g_object_unref (self->priv->combo_format), NULL)), _tmp21_);
		gtk_box_pack_start ((GtkBox*) hbox, (GtkWidget*) self->priv->combo_format, TRUE, TRUE, (guint) 0);
		gtk_combo_box_append_text (self->priv->combo_format, _ ("Title"));
		gtk_combo_box_append_text (self->priv->combo_format, _ ("Album - Title"));
		gtk_combo_box_append_text (self->priv->combo_format, _ ("Artist - Title"));
		gtk_combo_box_append_text (self->priv->combo_format, _ ("Artist - Title (Date)"));
		gtk_combo_box_append_text (self->priv->combo_format, _ ("Artist - Album - Title"));
		gtk_combo_box_append_text (self->priv->combo_format, _ ("Artist - Album - Track. Title"));
		gtk_combo_box_append_text (self->priv->combo_format, _ ("Custom..."));
		gtk_combo_box_set_active (self->priv->combo_format, (gint) xfmpc_preferences_get_song_format (self->priv->preferences));
		gtk_box_pack_start ((GtkBox*) vbox2, (GtkWidget*) hbox, TRUE, TRUE, (guint) 0);
		_tmp22_ = NULL;
		hbox = (_tmp22_ = g_object_ref_sink ((GtkHBox*) gtk_hbox_new (FALSE, 2)), (hbox == NULL) ? NULL : (hbox = (g_object_unref (hbox), NULL)), _tmp22_);
		_tmp23_ = NULL;
		label = (_tmp23_ = g_object_ref_sink ((GtkLabel*) gtk_label_new (_ ("Custom format:"))), (label == NULL) ? NULL : (label = (g_object_unref (label), NULL)), _tmp23_);
		gtk_box_pack_start ((GtkBox*) hbox, (GtkWidget*) label, FALSE, FALSE, (guint) 0);
		_tmp24_ = NULL;
		self->priv->entry_custom = (_tmp24_ = g_object_ref_sink ((GtkEntry*) gtk_entry_new ()), (self->priv->entry_custom == NULL) ? NULL : (self->priv->entry_custom = (g_object_unref (self->priv->entry_custom), NULL)), _tmp24_);
		gtk_entry_set_width_chars (self->priv->entry_custom, 15);
		gtk_entry_set_max_length (self->priv->entry_custom, 30);
		gtk_entry_set_text (self->priv->entry_custom, xfmpc_preferences_get_song_format_custom (self->priv->preferences));
		gtk_widget_set_sensitive ((GtkWidget*) self->priv->entry_custom, gtk_combo_box_get_active (self->priv->combo_format) == 6);
		gtk_box_pack_start ((GtkBox*) hbox, (GtkWidget*) self->priv->entry_custom, TRUE, TRUE, (guint) 0);
		g_signal_connect_object (self->priv->combo_format, "changed", (GCallback) _xfmpc_preferences_dialog_cb_combo_format_changed_gtk_combo_box_changed, self, 0);
		g_signal_connect_object ((GtkEditable*) self->priv->entry_custom, "changed", (GCallback) _xfmpc_preferences_dialog_cb_entry_custom_changed_gtk_editable_changed, self, 0);
		gtk_box_pack_start ((GtkBox*) vbox2, (GtkWidget*) hbox, TRUE, TRUE, (guint) 0);
		_tmp25_ = NULL;
		label = (_tmp25_ = g_object_ref_sink ((GtkLabel*) gtk_label_new (_ ("Available parameters:"))), (label == NULL) ? NULL : (label = (g_object_unref (label), NULL)), _tmp25_);
		gtk_box_pack_start ((GtkBox*) vbox2, (GtkWidget*) label, TRUE, TRUE, (guint) 0);
		table = g_object_ref_sink ((GtkTable*) gtk_table_new ((guint) 4, (guint) 6, TRUE));
		attrs = pango_attr_list_new ();
		pango_attr_list_insert (attrs, pango_attr_scale_new ((double) PANGO_SCALE_SMALL));
		_tmp26_ = NULL;
		label = (_tmp26_ = g_object_ref_sink ((GtkLabel*) gtk_label_new (_ ("%a: Artist"))), (label == NULL) ? NULL : (label = (g_object_unref (label), NULL)), _tmp26_);
		gtk_label_set_attributes (label, attrs);
		gtk_misc_set_alignment ((GtkMisc*) label, (float) 0, (float) 0.5);
		gtk_table_attach_defaults (table, (GtkWidget*) label, (guint) 1, (guint) 3, (guint) 0, (guint) 1);
		_tmp27_ = NULL;
		label = (_tmp27_ = g_object_ref_sink ((GtkLabel*) gtk_label_new (_ ("%A: Album"))), (label == NULL) ? NULL : (label = (g_object_unref (label), NULL)), _tmp27_);
		gtk_label_set_attributes (label, attrs);
		gtk_misc_set_alignment ((GtkMisc*) label, (float) 0, (float) 0.5);
		gtk_table_attach_defaults (table, (GtkWidget*) label, (guint) 4, (guint) 6, (guint) 0, (guint) 1);
		_tmp28_ = NULL;
		label = (_tmp28_ = g_object_ref_sink ((GtkLabel*) gtk_label_new (_ ("%d: Date"))), (label == NULL) ? NULL : (label = (g_object_unref (label), NULL)), _tmp28_);
		gtk_label_set_attributes (label, attrs);
		gtk_misc_set_alignment ((GtkMisc*) label, (float) 0, (float) 0.5);
		gtk_table_attach_defaults (table, (GtkWidget*) label, (guint) 1, (guint) 3, (guint) 1, (guint) 2);
		_tmp29_ = NULL;
		label = (_tmp29_ = g_object_ref_sink ((GtkLabel*) gtk_label_new (_ ("%D: Disc"))), (label == NULL) ? NULL : (label = (g_object_unref (label), NULL)), _tmp29_);
		gtk_label_set_attributes (label, attrs);
		gtk_misc_set_alignment ((GtkMisc*) label, (float) 0, (float) 0.5);
		gtk_table_attach_defaults (table, (GtkWidget*) label, (guint) 4, (guint) 6, (guint) 1, (guint) 2);
		_tmp30_ = NULL;
		label = (_tmp30_ = g_object_ref_sink ((GtkLabel*) gtk_label_new (_ ("%f: File"))), (label == NULL) ? NULL : (label = (g_object_unref (label), NULL)), _tmp30_);
		gtk_label_set_attributes (label, attrs);
		gtk_misc_set_alignment ((GtkMisc*) label, (float) 0, (float) 0.5);
		gtk_table_attach_defaults (table, (GtkWidget*) label, (guint) 1, (guint) 3, (guint) 2, (guint) 3);
		_tmp31_ = NULL;
		label = (_tmp31_ = g_object_ref_sink ((GtkLabel*) gtk_label_new (_ ("%g: Genre"))), (label == NULL) ? NULL : (label = (g_object_unref (label), NULL)), _tmp31_);
		gtk_label_set_attributes (label, attrs);
		gtk_misc_set_alignment ((GtkMisc*) label, (float) 0, (float) 0.5);
		gtk_table_attach_defaults (table, (GtkWidget*) label, (guint) 4, (guint) 6, (guint) 2, (guint) 3);
		_tmp32_ = NULL;
		label = (_tmp32_ = g_object_ref_sink ((GtkLabel*) gtk_label_new (_ ("%t: Title"))), (label == NULL) ? NULL : (label = (g_object_unref (label), NULL)), _tmp32_);
		gtk_label_set_attributes (label, attrs);
		gtk_misc_set_alignment ((GtkMisc*) label, (float) 0, (float) 0.5);
		gtk_table_attach_defaults (table, (GtkWidget*) label, (guint) 1, (guint) 3, (guint) 3, (guint) 4);
		_tmp33_ = NULL;
		label = (_tmp33_ = g_object_ref_sink ((GtkLabel*) gtk_label_new (_ ("%T: Track"))), (label == NULL) ? NULL : (label = (g_object_unref (label), NULL)), _tmp33_);
		gtk_label_set_attributes (label, attrs);
		gtk_misc_set_alignment ((GtkMisc*) label, (float) 0, (float) 0.5);
		gtk_table_attach_defaults (table, (GtkWidget*) label, (guint) 4, (guint) 6, (guint) 3, (guint) 4);
		gtk_box_pack_start ((GtkBox*) vbox2, (GtkWidget*) table, TRUE, TRUE, (guint) 0);
		gtk_dialog_add_button ((GtkDialog*) self, GTK_STOCK_CLOSE, (gint) GTK_RESPONSE_CLOSE);
		gtk_widget_show_all ((GtkWidget*) self);
		/* Signals */
		g_signal_connect_object ((GtkDialog*) self, "response", (GCallback) _xfmpc_preferences_dialog_cb_response_gtk_dialog_response, self, 0);
		(notebook == NULL) ? NULL : (notebook = (g_object_unref (notebook), NULL));
		(vbox == NULL) ? NULL : (vbox = (g_object_unref (vbox), NULL));
		(label == NULL) ? NULL : (label = (g_object_unref (label), NULL));
		(vbox2 == NULL) ? NULL : (vbox2 = (g_object_unref (vbox2), NULL));
		(frame == NULL) ? NULL : (frame = (g_object_unref (frame), NULL));
		(hbox == NULL) ? NULL : (hbox = (g_object_unref (hbox), NULL));
		(button == NULL) ? NULL : (button = (g_object_unref (button), NULL));
		(table == NULL) ? NULL : (table = (g_object_unref (table), NULL));
		(attrs == NULL) ? NULL : (attrs = (pango_attr_list_unref (attrs), NULL));
	}
	return obj;
}


static void xfmpc_preferences_dialog_class_init (XfmpcPreferencesDialogClass * klass) {
	xfmpc_preferences_dialog_parent_class = g_type_class_peek_parent (klass);
	g_type_class_add_private (klass, sizeof (XfmpcPreferencesDialogPrivate));
	G_OBJECT_CLASS (klass)->constructor = xfmpc_preferences_dialog_constructor;
	G_OBJECT_CLASS (klass)->finalize = xfmpc_preferences_dialog_finalize;
}


static void xfmpc_preferences_dialog_instance_init (XfmpcPreferencesDialog * self) {
	self->priv = XFMPC_PREFERENCES_DIALOG_GET_PRIVATE (self);
}


static void xfmpc_preferences_dialog_finalize (GObject* obj) {
	XfmpcPreferencesDialog * self;
	self = XFMPC_PREFERENCES_DIALOG (obj);
	(self->priv->entry_use_defaults == NULL) ? NULL : (self->priv->entry_use_defaults = (g_object_unref (self->priv->entry_use_defaults), NULL));
	(self->priv->entry_host == NULL) ? NULL : (self->priv->entry_host = (g_object_unref (self->priv->entry_host), NULL));
	(self->priv->entry_passwd == NULL) ? NULL : (self->priv->entry_passwd = (g_object_unref (self->priv->entry_passwd), NULL));
	(self->priv->entry_port == NULL) ? NULL : (self->priv->entry_port = (g_object_unref (self->priv->entry_port), NULL));
	(self->priv->show_statusbar == NULL) ? NULL : (self->priv->show_statusbar = (g_object_unref (self->priv->show_statusbar), NULL));
	(self->priv->combo_format == NULL) ? NULL : (self->priv->combo_format = (g_object_unref (self->priv->combo_format), NULL));
	(self->priv->entry_custom == NULL) ? NULL : (self->priv->entry_custom = (g_object_unref (self->priv->entry_custom), NULL));
	(self->priv->mpd_vbox == NULL) ? NULL : (self->priv->mpd_vbox = (g_object_unref (self->priv->mpd_vbox), NULL));
	G_OBJECT_CLASS (xfmpc_preferences_dialog_parent_class)->finalize (obj);
}


GType xfmpc_preferences_dialog_get_type (void) {
	static GType xfmpc_preferences_dialog_type_id = 0;
	if (xfmpc_preferences_dialog_type_id == 0) {
		static const GTypeInfo g_define_type_info = { sizeof (XfmpcPreferencesDialogClass), (GBaseInitFunc) NULL, (GBaseFinalizeFunc) NULL, (GClassInitFunc) xfmpc_preferences_dialog_class_init, (GClassFinalizeFunc) NULL, NULL, sizeof (XfmpcPreferencesDialog), 0, (GInstanceInitFunc) xfmpc_preferences_dialog_instance_init, NULL };
		xfmpc_preferences_dialog_type_id = g_type_register_static (XFCE_TYPE_TITLED_DIALOG, "XfmpcPreferencesDialog", &g_define_type_info, 0);
	}
	return xfmpc_preferences_dialog_type_id;
}




