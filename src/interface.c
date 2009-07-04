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
#include <gtk/gtk.h>
#include <mpdclient.h>
#include <stdlib.h>
#include <string.h>
#include <gdk/gdk.h>
#include <float.h>
#include <math.h>
#include <glib/gi18n-lib.h>
#include <config.h>
#include <pango/pango.h>


#define XFMPC_TYPE_INTERFACE (xfmpc_interface_get_type ())
#define XFMPC_INTERFACE(obj) (G_TYPE_CHECK_INSTANCE_CAST ((obj), XFMPC_TYPE_INTERFACE, XfmpcInterface))
#define XFMPC_INTERFACE_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST ((klass), XFMPC_TYPE_INTERFACE, XfmpcInterfaceClass))
#define XFMPC_IS_INTERFACE(obj) (G_TYPE_CHECK_INSTANCE_TYPE ((obj), XFMPC_TYPE_INTERFACE))
#define XFMPC_IS_INTERFACE_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), XFMPC_TYPE_INTERFACE))
#define XFMPC_INTERFACE_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj), XFMPC_TYPE_INTERFACE, XfmpcInterfaceClass))

typedef struct _XfmpcInterface XfmpcInterface;
typedef struct _XfmpcInterfaceClass XfmpcInterfaceClass;
typedef struct _XfmpcInterfacePrivate XfmpcInterfacePrivate;

#define XFMPC_TYPE_PREFERENCES (xfmpc_preferences_get_type ())
#define XFMPC_PREFERENCES(obj) (G_TYPE_CHECK_INSTANCE_CAST ((obj), XFMPC_TYPE_PREFERENCES, XfmpcPreferences))
#define XFMPC_PREFERENCES_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST ((klass), XFMPC_TYPE_PREFERENCES, XfmpcPreferencesClass))
#define XFMPC_IS_PREFERENCES(obj) (G_TYPE_CHECK_INSTANCE_TYPE ((obj), XFMPC_TYPE_PREFERENCES))
#define XFMPC_IS_PREFERENCES_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), XFMPC_TYPE_PREFERENCES))
#define XFMPC_PREFERENCES_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj), XFMPC_TYPE_PREFERENCES, XfmpcPreferencesClass))

typedef struct _XfmpcPreferences XfmpcPreferences;
typedef struct _XfmpcPreferencesClass XfmpcPreferencesClass;

struct _XfmpcInterface {
	GtkVBox parent_instance;
	XfmpcInterfacePrivate * priv;
};

struct _XfmpcInterfaceClass {
	GtkVBoxClass parent_class;
};

struct _XfmpcInterfacePrivate {
	XfmpcMpdclient* mpdclient;
	XfmpcPreferences* preferences;
	GtkButton* button_prev;
	GtkButton* button_pp;
	GtkButton* button_next;
	GtkVolumeButton* button_volume;
	GtkProgressBar* progress_bar;
	GtkLabel* title;
	GtkLabel* subtitle;
	gboolean refresh_title;
};



GType xfmpc_interface_get_type (void);
GType xfmpc_preferences_get_type (void);
#define XFMPC_INTERFACE_GET_PRIVATE(o) (G_TYPE_INSTANCE_GET_PRIVATE ((o), XFMPC_TYPE_INTERFACE, XfmpcInterfacePrivate))
enum  {
	XFMPC_INTERFACE_DUMMY_PROPERTY
};
void xfmpc_interface_set_title (XfmpcInterface* self, const char* title);
void xfmpc_interface_set_subtitle (XfmpcInterface* self, const char* subtitle);
void xfmpc_interface_set_pp (XfmpcInterface* self, gboolean play);
void xfmpc_interface_pp_clicked (XfmpcInterface* self);
static gboolean xfmpc_interface_cb_progress_box_release_event (XfmpcInterface* self, const GdkEventButton* event);
void xfmpc_interface_volume_changed (XfmpcInterface* self, double value);
void xfmpc_interface_set_volume (XfmpcInterface* self, gint volume);
void xfmpc_interface_popup_volume (XfmpcInterface* self);
void xfmpc_interface_set_time (XfmpcInterface* self, gint time, gint time_total);
static void xfmpc_interface_reconnect (XfmpcInterface* self);
static gboolean xfmpc_interface_refresh (XfmpcInterface* self);
static void xfmpc_interface_cb_song_changed (XfmpcInterface* self);
static void xfmpc_interface_cb_stopped (XfmpcInterface* self);
static gboolean _xfmpc_interface_refresh_gsource_func (gpointer self);
static void xfmpc_interface_cb_pp_changed (XfmpcInterface* self, gboolean is_playing);
static void xfmpc_interface_cb_time_changed (XfmpcInterface* self, gint time, gint total_time);
static void xfmpc_interface_cb_volume_changed (XfmpcInterface* self, gint volume);
static void xfmpc_interface_cb_mpdclient_previous (XfmpcInterface* self);
static void xfmpc_interface_cb_mpdclient_next (XfmpcInterface* self);
XfmpcInterface* xfmpc_interface_new (void);
XfmpcInterface* xfmpc_interface_construct (GType object_type);
XfmpcInterface* xfmpc_interface_new (void);
XfmpcPreferences* xfmpc_preferences_get (void);
static void _xfmpc_interface_cb_mpdclient_previous_gtk_button_clicked (GtkButton* _sender, gpointer self);
static void _xfmpc_interface_pp_clicked_gtk_button_clicked (GtkButton* _sender, gpointer self);
static void _xfmpc_interface_cb_mpdclient_next_gtk_button_clicked (GtkButton* _sender, gpointer self);
static void _xfmpc_interface_volume_changed_gtk_scale_button_value_changed (GtkScaleButton* _sender, double value, gpointer self);
static gboolean _xfmpc_interface_cb_progress_box_release_event_gtk_widget_button_release_event (GtkWidget* _sender, const GdkEventButton* event, gpointer self);
static void _xfmpc_interface_reconnect_xfmpc_mpdclient_connected (XfmpcMpdclient* _sender, gpointer self);
static void _xfmpc_interface_cb_song_changed_xfmpc_mpdclient_song_changed (XfmpcMpdclient* _sender, gpointer self);
static void _xfmpc_interface_cb_pp_changed_xfmpc_mpdclient_pp_changed (XfmpcMpdclient* _sender, gboolean is_playing, gpointer self);
static void _xfmpc_interface_cb_time_changed_xfmpc_mpdclient_time_changed (XfmpcMpdclient* _sender, gint time, gint total_time, gpointer self);
static void _xfmpc_interface_cb_volume_changed_xfmpc_mpdclient_volume_changed (XfmpcMpdclient* _sender, gint volume, gpointer self);
static void _xfmpc_interface_cb_stopped_xfmpc_mpdclient_stopped (XfmpcMpdclient* _sender, gpointer self);
static GObject * xfmpc_interface_constructor (GType type, guint n_construct_properties, GObjectConstructParam * construct_properties);
static gpointer xfmpc_interface_parent_class = NULL;
static void xfmpc_interface_finalize (GObject* obj);



void xfmpc_interface_set_title (XfmpcInterface* self, const char* title) {
	g_return_if_fail (self != NULL);
	g_return_if_fail (title != NULL);
	gtk_label_set_text (self->priv->title, title);
}


void xfmpc_interface_set_subtitle (XfmpcInterface* self, const char* subtitle) {
	g_return_if_fail (self != NULL);
	g_return_if_fail (subtitle != NULL);
	gtk_label_set_text (self->priv->subtitle, subtitle);
}


void xfmpc_interface_pp_clicked (XfmpcInterface* self) {
	g_return_if_fail (self != NULL);
	if (!xfmpc_mpdclient_pp (self->priv->mpdclient)) {
		return;
	}
	xfmpc_interface_set_pp (self, xfmpc_mpdclient_is_playing (self->priv->mpdclient));
}


void xfmpc_interface_set_pp (XfmpcInterface* self, gboolean play) {
	GtkImage* _tmp0_;
	GtkImage* image;
	g_return_if_fail (self != NULL);
	_tmp0_ = NULL;
	image = (_tmp0_ = GTK_IMAGE (gtk_bin_get_child ((GtkBin*) self->priv->button_pp)), (_tmp0_ == NULL) ? NULL : g_object_ref (_tmp0_));
	if (play == TRUE) {
		gtk_image_set_from_stock (image, GTK_STOCK_MEDIA_PAUSE, GTK_ICON_SIZE_BUTTON);
	} else {
		gtk_image_set_from_stock (image, GTK_STOCK_MEDIA_PLAY, GTK_ICON_SIZE_BUTTON);
	}
	(image == NULL) ? NULL : (image = (g_object_unref (image), NULL));
}


static gboolean xfmpc_interface_cb_progress_box_release_event (XfmpcInterface* self, const GdkEventButton* event) {
	gboolean _tmp0_;
	gint time_total;
	double time;
	g_return_val_if_fail (self != NULL, FALSE);
	_tmp0_ = FALSE;
	if ((*event).type != GDK_BUTTON_RELEASE) {
		_tmp0_ = TRUE;
	} else {
		_tmp0_ = (*event).button != 1;
	}
	if (_tmp0_) {
		return FALSE;
	}
	time_total = xfmpc_mpdclient_get_total_time (self->priv->mpdclient);
	if (time_total < 0) {
		return FALSE;
	}
	time = (*event).x / ((GtkWidget*) self->priv->progress_bar)->allocation.width;
	time = time * ((double) time_total);
	xfmpc_mpdclient_set_song_time (self->priv->mpdclient, (guint) ((gint) time));
	return TRUE;
}


void xfmpc_interface_volume_changed (XfmpcInterface* self, double value) {
	g_return_if_fail (self != NULL);
	xfmpc_mpdclient_set_volume (self->priv->mpdclient, (guchar) ((gchar) value));
}


void xfmpc_interface_set_volume (XfmpcInterface* self, gint volume) {
	g_return_if_fail (self != NULL);
	gtk_scale_button_set_value ((GtkScaleButton*) self->priv->button_volume, (double) volume);
}


void xfmpc_interface_popup_volume (XfmpcInterface* self) {
	g_return_if_fail (self != NULL);
	g_signal_emit_by_name (self->priv->button_volume, "popup", NULL, NULL);
}


void xfmpc_interface_set_time (XfmpcInterface* self, gint time, gint time_total) {
	gint min;
	gint sec;
	gint min_total;
	gint sec_total;
	double fraction;
	GString* text;
	double _tmp0_;
	g_return_if_fail (self != NULL);
	min = 0;
	sec = 0;
	min_total = 0;
	sec_total = 0;
	fraction = 1.0;
	min = time / 60;
	sec = time % 60;
	min_total = time_total / 60;
	sec_total = time_total % 60;
	text = g_string_new ("");
	g_string_append_printf (text, "%d:%02d / %d:%02d", min, sec, min_total, sec_total);
	gtk_progress_bar_set_text (self->priv->progress_bar, text->str);
	if (time_total > 0) {
		fraction = (double) (((float) time) / ((float) time_total));
	}
	_tmp0_ = 0.0;
	if (fraction <= 1.0) {
		_tmp0_ = fraction;
	} else {
		_tmp0_ = 1.0;
	}
	gtk_progress_bar_set_fraction (self->priv->progress_bar, _tmp0_);
	(text == NULL) ? NULL : (text = (g_string_free (text, TRUE), NULL));
}


static gboolean xfmpc_interface_refresh (XfmpcInterface* self) {
	g_return_val_if_fail (self != NULL, FALSE);
	if (xfmpc_mpdclient_connect (self->priv->mpdclient) == FALSE) {
		g_warning ("interface.vala:208: Failed to connect to MPD");
		xfmpc_mpdclient_disconnect (self->priv->mpdclient);
		xfmpc_interface_set_pp (self, FALSE);
		xfmpc_interface_set_time (self, 0, 0);
		xfmpc_interface_set_volume (self, 0);
		xfmpc_interface_set_title (self, _ ("Not connected"));
		xfmpc_interface_set_subtitle (self, PACKAGE_STRING);
		g_timeout_add ((guint) 15000, (GSourceFunc) xfmpc_interface_reconnect, NULL);
		return FALSE;
	}
	xfmpc_mpdclient_update_status (self->priv->mpdclient);
	return TRUE;
}


static gboolean _xfmpc_interface_refresh_gsource_func (gpointer self) {
	return xfmpc_interface_refresh (self);
}


static void xfmpc_interface_reconnect (XfmpcInterface* self) {
	g_return_if_fail (self != NULL);
	if (xfmpc_mpdclient_connect (self->priv->mpdclient) == FALSE) {
		return;
	}
	/* Refresh title/subtitle (bug #4975) */
	self->priv->refresh_title = TRUE;
	if (xfmpc_mpdclient_is_playing (self->priv->mpdclient)) {
		xfmpc_interface_cb_song_changed (self);
	} else {
		xfmpc_interface_cb_stopped (self);
	}
	/* Return FALSE to kill the reconnection timeout and start a refresh timeout */
	g_timeout_add ((guint) 1000, _xfmpc_interface_refresh_gsource_func, self);
}


static void xfmpc_interface_cb_song_changed (XfmpcInterface* self) {
	GString* text;
	g_return_if_fail (self != NULL);
	/* title */
	xfmpc_interface_set_title (self, xfmpc_mpdclient_get_title (self->priv->mpdclient));
	/* subtitle "by \"artist\" from \"album\" (year)" */
	text = g_string_new ("");
	g_string_append_printf (text, _ ("by \"%s\" from \"%s\" (%s)"), xfmpc_mpdclient_get_artist (self->priv->mpdclient), xfmpc_mpdclient_get_album (self->priv->mpdclient), xfmpc_mpdclient_get_date (self->priv->mpdclient));
	/* text = xfmpc_interface_get_subtitle (interface); to avoid "n/a" values, so far I don't care */
	xfmpc_interface_set_subtitle (self, text->str);
	(text == NULL) ? NULL : (text = (g_string_free (text, TRUE), NULL));
}


static void xfmpc_interface_cb_pp_changed (XfmpcInterface* self, gboolean is_playing) {
	g_return_if_fail (self != NULL);
	xfmpc_interface_set_pp (self, is_playing);
	if (self->priv->refresh_title) {
		xfmpc_interface_cb_song_changed (self);
		self->priv->refresh_title = FALSE;
	}
}


static void xfmpc_interface_cb_time_changed (XfmpcInterface* self, gint time, gint total_time) {
	g_return_if_fail (self != NULL);
	xfmpc_interface_set_time (self, time, total_time);
}


static void xfmpc_interface_cb_volume_changed (XfmpcInterface* self, gint volume) {
	g_return_if_fail (self != NULL);
	xfmpc_interface_set_volume (self, volume);
}


static void xfmpc_interface_cb_stopped (XfmpcInterface* self) {
	g_return_if_fail (self != NULL);
	xfmpc_interface_set_pp (self, FALSE);
	xfmpc_interface_set_time (self, 0, 0);
	xfmpc_interface_set_title (self, _ ("Stopped"));
	xfmpc_interface_set_subtitle (self, PACKAGE_STRING);
	self->priv->refresh_title = TRUE;
}


static void xfmpc_interface_cb_mpdclient_previous (XfmpcInterface* self) {
	g_return_if_fail (self != NULL);
	xfmpc_mpdclient_previous (self->priv->mpdclient);
}


static void xfmpc_interface_cb_mpdclient_next (XfmpcInterface* self) {
	g_return_if_fail (self != NULL);
	xfmpc_mpdclient_next (self->priv->mpdclient);
}


XfmpcInterface* xfmpc_interface_construct (GType object_type) {
	XfmpcInterface * self;
	self = g_object_newv (object_type, 0, NULL);
	return self;
}


XfmpcInterface* xfmpc_interface_new (void) {
	return xfmpc_interface_construct (XFMPC_TYPE_INTERFACE);
}


static void _xfmpc_interface_cb_mpdclient_previous_gtk_button_clicked (GtkButton* _sender, gpointer self) {
	xfmpc_interface_cb_mpdclient_previous (self);
}


static void _xfmpc_interface_pp_clicked_gtk_button_clicked (GtkButton* _sender, gpointer self) {
	xfmpc_interface_pp_clicked (self);
}


static void _xfmpc_interface_cb_mpdclient_next_gtk_button_clicked (GtkButton* _sender, gpointer self) {
	xfmpc_interface_cb_mpdclient_next (self);
}


static void _xfmpc_interface_volume_changed_gtk_scale_button_value_changed (GtkScaleButton* _sender, double value, gpointer self) {
	xfmpc_interface_volume_changed (self, value);
}


static gboolean _xfmpc_interface_cb_progress_box_release_event_gtk_widget_button_release_event (GtkWidget* _sender, const GdkEventButton* event, gpointer self) {
	return xfmpc_interface_cb_progress_box_release_event (self, event);
}


static void _xfmpc_interface_reconnect_xfmpc_mpdclient_connected (XfmpcMpdclient* _sender, gpointer self) {
	xfmpc_interface_reconnect (self);
}


static void _xfmpc_interface_cb_song_changed_xfmpc_mpdclient_song_changed (XfmpcMpdclient* _sender, gpointer self) {
	xfmpc_interface_cb_song_changed (self);
}


static void _xfmpc_interface_cb_pp_changed_xfmpc_mpdclient_pp_changed (XfmpcMpdclient* _sender, gboolean is_playing, gpointer self) {
	xfmpc_interface_cb_pp_changed (self, is_playing);
}


static void _xfmpc_interface_cb_time_changed_xfmpc_mpdclient_time_changed (XfmpcMpdclient* _sender, gint time, gint total_time, gpointer self) {
	xfmpc_interface_cb_time_changed (self, time, total_time);
}


static void _xfmpc_interface_cb_volume_changed_xfmpc_mpdclient_volume_changed (XfmpcMpdclient* _sender, gint volume, gpointer self) {
	xfmpc_interface_cb_volume_changed (self, volume);
}


static void _xfmpc_interface_cb_stopped_xfmpc_mpdclient_stopped (XfmpcMpdclient* _sender, gpointer self) {
	xfmpc_interface_cb_stopped (self);
}


static GObject * xfmpc_interface_constructor (GType type, guint n_construct_properties, GObjectConstructParam * construct_properties) {
	GObject * obj;
	XfmpcInterfaceClass * klass;
	GObjectClass * parent_class;
	XfmpcInterface * self;
	klass = XFMPC_INTERFACE_CLASS (g_type_class_peek (XFMPC_TYPE_INTERFACE));
	parent_class = G_OBJECT_CLASS (g_type_class_peek_parent (klass));
	obj = parent_class->constructor (type, n_construct_properties, construct_properties);
	self = XFMPC_INTERFACE (obj);
	{
		GtkImage* image;
		GtkButton* _tmp0_;
		GtkImage* _tmp1_;
		GtkButton* _tmp2_;
		GtkImage* _tmp3_;
		GtkButton* _tmp4_;
		GtkVolumeButton* _tmp5_;
		GtkAdjustment* _tmp6_;
		GtkAdjustment* adjustment;
		GtkEventBox* progress_box;
		GtkProgressBar* _tmp7_;
		PangoAttrList* attrs;
		PangoAttribute* attr;
		GtkLabel* _tmp8_;
		PangoAttrList* _tmp9_;
		GtkLabel* _tmp10_;
		GtkHBox* box;
		GtkVBox* vbox;
		self->priv->mpdclient = xfmpc_mpdclient_get ();
		self->priv->preferences = xfmpc_preferences_get ();
		gtk_container_set_border_width ((GtkContainer*) self, (guint) 4);
		image = g_object_ref_sink ((GtkImage*) gtk_image_new_from_stock (GTK_STOCK_MEDIA_PREVIOUS, GTK_ICON_SIZE_BUTTON));
		_tmp0_ = NULL;
		self->priv->button_prev = (_tmp0_ = g_object_ref_sink ((GtkButton*) gtk_button_new ()), (self->priv->button_prev == NULL) ? NULL : (self->priv->button_prev = (g_object_unref (self->priv->button_prev), NULL)), _tmp0_);
		gtk_button_set_relief (self->priv->button_prev, GTK_RELIEF_NONE);
		gtk_container_add ((GtkContainer*) self->priv->button_prev, (GtkWidget*) image);
		_tmp1_ = NULL;
		image = (_tmp1_ = g_object_ref_sink ((GtkImage*) gtk_image_new_from_stock (GTK_STOCK_MEDIA_PLAY, GTK_ICON_SIZE_BUTTON)), (image == NULL) ? NULL : (image = (g_object_unref (image), NULL)), _tmp1_);
		_tmp2_ = NULL;
		self->priv->button_pp = (_tmp2_ = g_object_ref_sink ((GtkButton*) gtk_button_new ()), (self->priv->button_pp == NULL) ? NULL : (self->priv->button_pp = (g_object_unref (self->priv->button_pp), NULL)), _tmp2_);
		gtk_button_set_relief (self->priv->button_pp, GTK_RELIEF_NONE);
		gtk_container_add ((GtkContainer*) self->priv->button_pp, (GtkWidget*) image);
		_tmp3_ = NULL;
		image = (_tmp3_ = g_object_ref_sink ((GtkImage*) gtk_image_new_from_stock (GTK_STOCK_MEDIA_NEXT, GTK_ICON_SIZE_BUTTON)), (image == NULL) ? NULL : (image = (g_object_unref (image), NULL)), _tmp3_);
		_tmp4_ = NULL;
		self->priv->button_next = (_tmp4_ = g_object_ref_sink ((GtkButton*) gtk_button_new ()), (self->priv->button_next == NULL) ? NULL : (self->priv->button_next = (g_object_unref (self->priv->button_next), NULL)), _tmp4_);
		gtk_button_set_relief (self->priv->button_next, GTK_RELIEF_NONE);
		gtk_container_add ((GtkContainer*) self->priv->button_next, (GtkWidget*) image);
		_tmp5_ = NULL;
		self->priv->button_volume = (_tmp5_ = g_object_ref_sink ((GtkVolumeButton*) gtk_volume_button_new ()), (self->priv->button_volume == NULL) ? NULL : (self->priv->button_volume = (g_object_unref (self->priv->button_volume), NULL)), _tmp5_);
		gtk_button_set_relief ((GtkButton*) self->priv->button_volume, GTK_RELIEF_NONE);
		_tmp6_ = NULL;
		adjustment = (_tmp6_ = gtk_scale_button_get_adjustment ((GtkScaleButton*) self->priv->button_volume), (_tmp6_ == NULL) ? NULL : g_object_ref (_tmp6_));
		gtk_adjustment_set_upper (adjustment, gtk_adjustment_get_upper (adjustment) * ((double) 100));
		gtk_adjustment_set_step_increment (adjustment, gtk_adjustment_get_step_increment (adjustment) * ((double) 100));
		gtk_adjustment_set_page_increment (adjustment, gtk_adjustment_get_page_increment (adjustment) * ((double) 100));
		progress_box = g_object_ref_sink ((GtkEventBox*) gtk_event_box_new ());
		_tmp7_ = NULL;
		self->priv->progress_bar = (_tmp7_ = g_object_ref_sink ((GtkProgressBar*) gtk_progress_bar_new ()), (self->priv->progress_bar == NULL) ? NULL : (self->priv->progress_bar = (g_object_unref (self->priv->progress_bar), NULL)), _tmp7_);
		gtk_progress_bar_set_text (self->priv->progress_bar, "0:00 / 0:00");
		gtk_progress_bar_set_fraction (self->priv->progress_bar, 1.0);
		gtk_container_add ((GtkContainer*) progress_box, (GtkWidget*) self->priv->progress_bar);
		/* Title */
		attrs = pango_attr_list_new ();
		attr = pango_attr_weight_new (PANGO_WEIGHT_BOLD);
		attr->start_index = (guint) 0;
		attr->end_index = (guint) (-1);
		pango_attr_list_insert (attrs, attr);
		attr = pango_attr_scale_new ((double) PANGO_SCALE_X_LARGE);
		attr->start_index = (guint) 0;
		attr->end_index = (guint) (-1);
		pango_attr_list_insert (attrs, attr);
		_tmp8_ = NULL;
		self->priv->title = (_tmp8_ = g_object_ref_sink ((GtkLabel*) gtk_label_new (_ ("Not connected"))), (self->priv->title == NULL) ? NULL : (self->priv->title = (g_object_unref (self->priv->title), NULL)), _tmp8_);
		gtk_label_set_attributes (self->priv->title, attrs);
		gtk_label_set_selectable (self->priv->title, TRUE);
		gtk_label_set_ellipsize (self->priv->title, PANGO_ELLIPSIZE_END);
		gtk_misc_set_alignment ((GtkMisc*) self->priv->title, (float) 0, (float) 0.5);
		/* Subtitle */
		_tmp9_ = NULL;
		attrs = (_tmp9_ = pango_attr_list_new (), (attrs == NULL) ? NULL : (attrs = (pango_attr_list_unref (attrs), NULL)), _tmp9_);
		attr = pango_attr_scale_new ((double) PANGO_SCALE_SMALL);
		attr->start_index = (guint) 0;
		attr->end_index = (guint) (-1);
		pango_attr_list_insert (attrs, attr);
		_tmp10_ = NULL;
		self->priv->subtitle = (_tmp10_ = g_object_ref_sink ((GtkLabel*) gtk_label_new (PACKAGE_STRING)), (self->priv->subtitle == NULL) ? NULL : (self->priv->subtitle = (g_object_unref (self->priv->subtitle), NULL)), _tmp10_);
		gtk_label_set_attributes (self->priv->subtitle, attrs);
		gtk_label_set_selectable (self->priv->subtitle, TRUE);
		gtk_label_set_ellipsize (self->priv->subtitle, PANGO_ELLIPSIZE_END);
		gtk_misc_set_alignment ((GtkMisc*) self->priv->subtitle, (float) 0, (float) 0.5);
		/* === Containers === */
		box = g_object_ref_sink ((GtkHBox*) gtk_hbox_new (FALSE, 0));
		gtk_box_pack_start ((GtkBox*) self, (GtkWidget*) box, FALSE, FALSE, (guint) 0);
		gtk_box_pack_start ((GtkBox*) box, (GtkWidget*) self->priv->button_prev, FALSE, FALSE, (guint) 0);
		gtk_box_pack_start ((GtkBox*) box, (GtkWidget*) self->priv->button_pp, FALSE, FALSE, (guint) 0);
		gtk_box_pack_start ((GtkBox*) box, (GtkWidget*) self->priv->button_next, FALSE, FALSE, (guint) 0);
		gtk_box_pack_start ((GtkBox*) box, (GtkWidget*) progress_box, TRUE, TRUE, (guint) 4);
		gtk_box_pack_start ((GtkBox*) box, (GtkWidget*) self->priv->button_volume, FALSE, FALSE, (guint) 0);
		vbox = g_object_ref_sink ((GtkVBox*) gtk_vbox_new (FALSE, 0));
		gtk_box_pack_start ((GtkBox*) self, (GtkWidget*) vbox, FALSE, TRUE, (guint) 0);
		gtk_container_add ((GtkContainer*) vbox, (GtkWidget*) self->priv->title);
		gtk_container_add ((GtkContainer*) vbox, (GtkWidget*) self->priv->subtitle);
		/* === Signals === */
		g_signal_connect_object (self->priv->button_prev, "clicked", (GCallback) _xfmpc_interface_cb_mpdclient_previous_gtk_button_clicked, self, 0);
		g_signal_connect_object (self->priv->button_pp, "clicked", (GCallback) _xfmpc_interface_pp_clicked_gtk_button_clicked, self, 0);
		g_signal_connect_object (self->priv->button_next, "clicked", (GCallback) _xfmpc_interface_cb_mpdclient_next_gtk_button_clicked, self, 0);
		g_signal_connect_object ((GtkScaleButton*) self->priv->button_volume, "value-changed", (GCallback) _xfmpc_interface_volume_changed_gtk_scale_button_value_changed, self, 0);
		g_signal_connect_object ((GtkWidget*) progress_box, "button-release-event", (GCallback) _xfmpc_interface_cb_progress_box_release_event_gtk_widget_button_release_event, self, 0);
		g_signal_connect_object (self->priv->mpdclient, "connected", (GCallback) _xfmpc_interface_reconnect_xfmpc_mpdclient_connected, self, 0);
		g_signal_connect_object (self->priv->mpdclient, "song-changed", (GCallback) _xfmpc_interface_cb_song_changed_xfmpc_mpdclient_song_changed, self, 0);
		g_signal_connect_object (self->priv->mpdclient, "pp-changed", (GCallback) _xfmpc_interface_cb_pp_changed_xfmpc_mpdclient_pp_changed, self, 0);
		g_signal_connect_object (self->priv->mpdclient, "time-changed", (GCallback) _xfmpc_interface_cb_time_changed_xfmpc_mpdclient_time_changed, self, 0);
		g_signal_connect_object (self->priv->mpdclient, "volume-changed", (GCallback) _xfmpc_interface_cb_volume_changed_xfmpc_mpdclient_volume_changed, self, 0);
		g_signal_connect_object (self->priv->mpdclient, "stopped", (GCallback) _xfmpc_interface_cb_stopped_xfmpc_mpdclient_stopped, self, 0);
		/* === Timeout === */
		g_timeout_add ((guint) 1000, _xfmpc_interface_refresh_gsource_func, self);
		(image == NULL) ? NULL : (image = (g_object_unref (image), NULL));
		(adjustment == NULL) ? NULL : (adjustment = (g_object_unref (adjustment), NULL));
		(progress_box == NULL) ? NULL : (progress_box = (g_object_unref (progress_box), NULL));
		(attrs == NULL) ? NULL : (attrs = (pango_attr_list_unref (attrs), NULL));
		(box == NULL) ? NULL : (box = (g_object_unref (box), NULL));
		(vbox == NULL) ? NULL : (vbox = (g_object_unref (vbox), NULL));
	}
	return obj;
}


static void xfmpc_interface_class_init (XfmpcInterfaceClass * klass) {
	xfmpc_interface_parent_class = g_type_class_peek_parent (klass);
	g_type_class_add_private (klass, sizeof (XfmpcInterfacePrivate));
	G_OBJECT_CLASS (klass)->constructor = xfmpc_interface_constructor;
	G_OBJECT_CLASS (klass)->finalize = xfmpc_interface_finalize;
}


static void xfmpc_interface_instance_init (XfmpcInterface * self) {
	self->priv = XFMPC_INTERFACE_GET_PRIVATE (self);
}


static void xfmpc_interface_finalize (GObject* obj) {
	XfmpcInterface * self;
	self = XFMPC_INTERFACE (obj);
	(self->priv->button_prev == NULL) ? NULL : (self->priv->button_prev = (g_object_unref (self->priv->button_prev), NULL));
	(self->priv->button_pp == NULL) ? NULL : (self->priv->button_pp = (g_object_unref (self->priv->button_pp), NULL));
	(self->priv->button_next == NULL) ? NULL : (self->priv->button_next = (g_object_unref (self->priv->button_next), NULL));
	(self->priv->button_volume == NULL) ? NULL : (self->priv->button_volume = (g_object_unref (self->priv->button_volume), NULL));
	(self->priv->progress_bar == NULL) ? NULL : (self->priv->progress_bar = (g_object_unref (self->priv->progress_bar), NULL));
	(self->priv->title == NULL) ? NULL : (self->priv->title = (g_object_unref (self->priv->title), NULL));
	(self->priv->subtitle == NULL) ? NULL : (self->priv->subtitle = (g_object_unref (self->priv->subtitle), NULL));
	G_OBJECT_CLASS (xfmpc_interface_parent_class)->finalize (obj);
}


GType xfmpc_interface_get_type (void) {
	static GType xfmpc_interface_type_id = 0;
	if (xfmpc_interface_type_id == 0) {
		static const GTypeInfo g_define_type_info = { sizeof (XfmpcInterfaceClass), (GBaseInitFunc) NULL, (GBaseFinalizeFunc) NULL, (GClassInitFunc) xfmpc_interface_class_init, (GClassFinalizeFunc) NULL, NULL, sizeof (XfmpcInterface), 0, (GInstanceInitFunc) xfmpc_interface_instance_init, NULL };
		xfmpc_interface_type_id = g_type_register_static (GTK_TYPE_VBOX, "XfmpcInterface", &g_define_type_info, 0);
	}
	return xfmpc_interface_type_id;
}




