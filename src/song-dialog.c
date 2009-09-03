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
#include <mpdclient.h>
#include <gtk/gtk.h>
#include <pango/pango.h>
#include <glib/gi18n-lib.h>


#define XFMPC_TYPE_SONG_DIALOG (xfmpc_song_dialog_get_type ())
#define XFMPC_SONG_DIALOG(obj) (G_TYPE_CHECK_INSTANCE_CAST ((obj), XFMPC_TYPE_SONG_DIALOG, XfmpcSongDialog))
#define XFMPC_SONG_DIALOG_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST ((klass), XFMPC_TYPE_SONG_DIALOG, XfmpcSongDialogClass))
#define XFMPC_IS_SONG_DIALOG(obj) (G_TYPE_CHECK_INSTANCE_TYPE ((obj), XFMPC_TYPE_SONG_DIALOG))
#define XFMPC_IS_SONG_DIALOG_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), XFMPC_TYPE_SONG_DIALOG))
#define XFMPC_SONG_DIALOG_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj), XFMPC_TYPE_SONG_DIALOG, XfmpcSongDialogClass))

typedef struct _XfmpcSongDialog XfmpcSongDialog;
typedef struct _XfmpcSongDialogClass XfmpcSongDialogClass;
typedef struct _XfmpcSongDialogPrivate XfmpcSongDialogPrivate;

struct _XfmpcSongDialog {
	XfceTitledDialog parent_instance;
	XfmpcSongDialogPrivate * priv;
};

struct _XfmpcSongDialogClass {
	XfceTitledDialogClass parent_class;
};

struct _XfmpcSongDialogPrivate {
	XfmpcMpdclient* mpdclient;
	XfmpcSongInfo* song;
};


static gpointer xfmpc_song_dialog_parent_class = NULL;

GType xfmpc_song_dialog_get_type (void);
#define XFMPC_SONG_DIALOG_GET_PRIVATE(o) (G_TYPE_INSTANCE_GET_PRIVATE ((o), XFMPC_TYPE_SONG_DIALOG, XfmpcSongDialogPrivate))
enum  {
	XFMPC_SONG_DIALOG_DUMMY_PROPERTY
};
static void xfmpc_song_dialog_cb_response (XfmpcSongDialog* self, gint response);
static void _xfmpc_song_dialog_cb_response_gtk_dialog_response (GtkDialog* _sender, gint response_id, gpointer self);
XfmpcSongDialog* xfmpc_song_dialog_new (gint song_id);
XfmpcSongDialog* xfmpc_song_dialog_construct (GType object_type, gint song_id);
static void xfmpc_song_dialog_finalize (GObject* obj);



static void _xfmpc_song_dialog_cb_response_gtk_dialog_response (GtkDialog* _sender, gint response_id, gpointer self) {
	xfmpc_song_dialog_cb_response (self, response_id);
}


XfmpcSongDialog* xfmpc_song_dialog_construct (GType object_type, gint song_id) {
	XfmpcSongDialog * self;
	GtkVBox* vbox2;
	GtkWidget* _tmp0_;
	GtkWidget* frame;
	PangoAttrList* attrs;
	GtkHBox* hbox;
	GtkLabel* label;
	GtkLabel* _tmp2_;
	char* _tmp1_;
	GtkHBox* _tmp3_;
	GtkLabel* _tmp4_;
	GtkLabel* _tmp5_;
	GtkHBox* _tmp6_;
	GtkLabel* _tmp7_;
	GtkLabel* _tmp8_;
	GtkHBox* _tmp9_;
	GtkLabel* _tmp10_;
	GtkLabel* _tmp11_;
	GtkHBox* _tmp12_;
	GtkLabel* _tmp13_;
	GtkLabel* _tmp14_;
	GtkLabel* _tmp15_;
	GtkLabel* _tmp16_;
	GtkHBox* _tmp17_;
	GtkLabel* _tmp18_;
	GtkLabel* _tmp19_;
	self = g_object_newv (object_type, 0, NULL);
	gtk_dialog_set_has_separator ((GtkDialog*) self, TRUE);
	gtk_window_set_skip_taskbar_hint ((GtkWindow*) self, TRUE);
	gtk_window_set_icon_name ((GtkWindow*) self, "stock_volume");
	gtk_window_set_resizable ((GtkWindow*) self, FALSE);
	self->priv->mpdclient = xfmpc_mpdclient_get_default ();
	self->priv->song = xfmpc_mpdclient_get_song_info (self->priv->mpdclient, song_id);
	gtk_window_set_title ((GtkWindow*) self, self->priv->song->title);
	gtk_box_set_spacing ((GtkBox*) ((GtkDialog*) self)->vbox, 0);
	vbox2 = g_object_ref_sink ((GtkVBox*) gtk_vbox_new (FALSE, 0));
	_tmp0_ = NULL;
	frame = (_tmp0_ = xfce_create_framebox_with_content ("", (GtkWidget*) vbox2), (_tmp0_ == NULL) ? NULL : g_object_ref (_tmp0_));
	attrs = pango_attr_list_new ();
	pango_attr_list_insert (attrs, pango_attr_weight_new (PANGO_WEIGHT_BOLD));
	hbox = g_object_ref_sink ((GtkHBox*) gtk_hbox_new (FALSE, 0));
	label = g_object_ref_sink ((GtkLabel*) gtk_label_new (_ ("File")));
	gtk_label_set_attributes (label, attrs);
	gtk_box_pack_start ((GtkBox*) hbox, (GtkWidget*) label, FALSE, FALSE, (guint) 5);
	_tmp2_ = NULL;
	_tmp1_ = NULL;
	label = (_tmp2_ = g_object_ref_sink ((GtkLabel*) gtk_label_new (_tmp1_ = g_path_get_basename (self->priv->song->filename))), (label == NULL) ? NULL : (label = (g_object_unref (label), NULL)), _tmp2_);
	_tmp1_ = (g_free (_tmp1_), NULL);
	gtk_box_pack_start ((GtkBox*) hbox, (GtkWidget*) label, FALSE, FALSE, (guint) 5);
	gtk_box_pack_start ((GtkBox*) vbox2, (GtkWidget*) hbox, FALSE, FALSE, (guint) 6);
	_tmp3_ = NULL;
	hbox = (_tmp3_ = g_object_ref_sink ((GtkHBox*) gtk_hbox_new (FALSE, 0)), (hbox == NULL) ? NULL : (hbox = (g_object_unref (hbox), NULL)), _tmp3_);
	_tmp4_ = NULL;
	label = (_tmp4_ = g_object_ref_sink ((GtkLabel*) gtk_label_new (_ ("Artist"))), (label == NULL) ? NULL : (label = (g_object_unref (label), NULL)), _tmp4_);
	gtk_label_set_attributes (label, attrs);
	gtk_box_pack_start ((GtkBox*) hbox, (GtkWidget*) label, FALSE, FALSE, (guint) 5);
	_tmp5_ = NULL;
	label = (_tmp5_ = g_object_ref_sink ((GtkLabel*) gtk_label_new (self->priv->song->artist)), (label == NULL) ? NULL : (label = (g_object_unref (label), NULL)), _tmp5_);
	gtk_box_pack_start ((GtkBox*) hbox, (GtkWidget*) label, FALSE, FALSE, (guint) 5);
	gtk_box_pack_start ((GtkBox*) vbox2, (GtkWidget*) hbox, FALSE, FALSE, (guint) 6);
	_tmp6_ = NULL;
	hbox = (_tmp6_ = g_object_ref_sink ((GtkHBox*) gtk_hbox_new (FALSE, 0)), (hbox == NULL) ? NULL : (hbox = (g_object_unref (hbox), NULL)), _tmp6_);
	_tmp7_ = NULL;
	label = (_tmp7_ = g_object_ref_sink ((GtkLabel*) gtk_label_new (_ ("Title"))), (label == NULL) ? NULL : (label = (g_object_unref (label), NULL)), _tmp7_);
	gtk_label_set_attributes (label, attrs);
	gtk_box_pack_start ((GtkBox*) hbox, (GtkWidget*) label, FALSE, FALSE, (guint) 5);
	_tmp8_ = NULL;
	label = (_tmp8_ = g_object_ref_sink ((GtkLabel*) gtk_label_new (self->priv->song->title)), (label == NULL) ? NULL : (label = (g_object_unref (label), NULL)), _tmp8_);
	gtk_box_pack_start ((GtkBox*) hbox, (GtkWidget*) label, FALSE, FALSE, (guint) 5);
	gtk_box_pack_start ((GtkBox*) vbox2, (GtkWidget*) hbox, FALSE, FALSE, (guint) 6);
	_tmp9_ = NULL;
	hbox = (_tmp9_ = g_object_ref_sink ((GtkHBox*) gtk_hbox_new (FALSE, 0)), (hbox == NULL) ? NULL : (hbox = (g_object_unref (hbox), NULL)), _tmp9_);
	_tmp10_ = NULL;
	label = (_tmp10_ = g_object_ref_sink ((GtkLabel*) gtk_label_new (_ ("Album"))), (label == NULL) ? NULL : (label = (g_object_unref (label), NULL)), _tmp10_);
	gtk_label_set_attributes (label, attrs);
	gtk_box_pack_start ((GtkBox*) hbox, (GtkWidget*) label, FALSE, FALSE, (guint) 5);
	_tmp11_ = NULL;
	label = (_tmp11_ = g_object_ref_sink ((GtkLabel*) gtk_label_new (self->priv->song->album)), (label == NULL) ? NULL : (label = (g_object_unref (label), NULL)), _tmp11_);
	gtk_box_pack_start ((GtkBox*) hbox, (GtkWidget*) label, FALSE, FALSE, (guint) 5);
	gtk_box_pack_start ((GtkBox*) vbox2, (GtkWidget*) hbox, FALSE, FALSE, (guint) 6);
	_tmp12_ = NULL;
	hbox = (_tmp12_ = g_object_ref_sink ((GtkHBox*) gtk_hbox_new (FALSE, 0)), (hbox == NULL) ? NULL : (hbox = (g_object_unref (hbox), NULL)), _tmp12_);
	_tmp13_ = NULL;
	label = (_tmp13_ = g_object_ref_sink ((GtkLabel*) gtk_label_new (_ ("Date"))), (label == NULL) ? NULL : (label = (g_object_unref (label), NULL)), _tmp13_);
	gtk_label_set_attributes (label, attrs);
	gtk_box_pack_start ((GtkBox*) hbox, (GtkWidget*) label, FALSE, FALSE, (guint) 5);
	_tmp14_ = NULL;
	label = (_tmp14_ = g_object_ref_sink ((GtkLabel*) gtk_label_new (self->priv->song->date)), (label == NULL) ? NULL : (label = (g_object_unref (label), NULL)), _tmp14_);
	gtk_box_pack_start ((GtkBox*) hbox, (GtkWidget*) label, FALSE, FALSE, (guint) 5);
	_tmp15_ = NULL;
	label = (_tmp15_ = g_object_ref_sink ((GtkLabel*) gtk_label_new (_ ("Track"))), (label == NULL) ? NULL : (label = (g_object_unref (label), NULL)), _tmp15_);
	gtk_label_set_attributes (label, attrs);
	gtk_box_pack_start ((GtkBox*) hbox, (GtkWidget*) label, FALSE, FALSE, (guint) 15);
	_tmp16_ = NULL;
	label = (_tmp16_ = g_object_ref_sink ((GtkLabel*) gtk_label_new (self->priv->song->track)), (label == NULL) ? NULL : (label = (g_object_unref (label), NULL)), _tmp16_);
	gtk_box_pack_start ((GtkBox*) hbox, (GtkWidget*) label, FALSE, FALSE, (guint) 5);
	gtk_box_pack_start ((GtkBox*) vbox2, (GtkWidget*) hbox, FALSE, FALSE, (guint) 6);
	_tmp17_ = NULL;
	hbox = (_tmp17_ = g_object_ref_sink ((GtkHBox*) gtk_hbox_new (FALSE, 0)), (hbox == NULL) ? NULL : (hbox = (g_object_unref (hbox), NULL)), _tmp17_);
	_tmp18_ = NULL;
	label = (_tmp18_ = g_object_ref_sink ((GtkLabel*) gtk_label_new (_ ("Genre"))), (label == NULL) ? NULL : (label = (g_object_unref (label), NULL)), _tmp18_);
	gtk_label_set_attributes (label, attrs);
	gtk_box_pack_start ((GtkBox*) hbox, (GtkWidget*) label, FALSE, FALSE, (guint) 5);
	_tmp19_ = NULL;
	label = (_tmp19_ = g_object_ref_sink ((GtkLabel*) gtk_label_new (self->priv->song->genre)), (label == NULL) ? NULL : (label = (g_object_unref (label), NULL)), _tmp19_);
	gtk_box_pack_start ((GtkBox*) hbox, (GtkWidget*) label, FALSE, FALSE, (guint) 5);
	gtk_box_pack_start ((GtkBox*) vbox2, (GtkWidget*) hbox, FALSE, FALSE, (guint) 6);
	gtk_box_pack_start ((GtkBox*) ((GtkDialog*) self)->vbox, frame, TRUE, TRUE, (guint) 0);
	gtk_dialog_add_button ((GtkDialog*) self, GTK_STOCK_CLOSE, (gint) GTK_RESPONSE_CLOSE);
	gtk_widget_show_all ((GtkWidget*) self);
	g_signal_connect_object ((GtkDialog*) self, "response", (GCallback) _xfmpc_song_dialog_cb_response_gtk_dialog_response, self, 0);
	(vbox2 == NULL) ? NULL : (vbox2 = (g_object_unref (vbox2), NULL));
	(frame == NULL) ? NULL : (frame = (g_object_unref (frame), NULL));
	(attrs == NULL) ? NULL : (attrs = (pango_attr_list_unref (attrs), NULL));
	(hbox == NULL) ? NULL : (hbox = (g_object_unref (hbox), NULL));
	(label == NULL) ? NULL : (label = (g_object_unref (label), NULL));
	return self;
}


XfmpcSongDialog* xfmpc_song_dialog_new (gint song_id) {
	return xfmpc_song_dialog_construct (XFMPC_TYPE_SONG_DIALOG, song_id);
}


/*
 * Signal callbacks
 */
static void xfmpc_song_dialog_cb_response (XfmpcSongDialog* self, gint response) {
	g_return_if_fail (self != NULL);
	switch (response) {
		case GTK_RESPONSE_CLOSE:
		{
			gtk_object_destroy ((GtkObject*) self);
			break;
		}
	}
}


static void xfmpc_song_dialog_class_init (XfmpcSongDialogClass * klass) {
	xfmpc_song_dialog_parent_class = g_type_class_peek_parent (klass);
	g_type_class_add_private (klass, sizeof (XfmpcSongDialogPrivate));
	G_OBJECT_CLASS (klass)->finalize = xfmpc_song_dialog_finalize;
}


static void xfmpc_song_dialog_instance_init (XfmpcSongDialog * self) {
	self->priv = XFMPC_SONG_DIALOG_GET_PRIVATE (self);
}


static void xfmpc_song_dialog_finalize (GObject* obj) {
	XfmpcSongDialog * self;
	self = XFMPC_SONG_DIALOG (obj);
	G_OBJECT_CLASS (xfmpc_song_dialog_parent_class)->finalize (obj);
}


GType xfmpc_song_dialog_get_type (void) {
	static GType xfmpc_song_dialog_type_id = 0;
	if (xfmpc_song_dialog_type_id == 0) {
		static const GTypeInfo g_define_type_info = { sizeof (XfmpcSongDialogClass), (GBaseInitFunc) NULL, (GBaseFinalizeFunc) NULL, (GClassInitFunc) xfmpc_song_dialog_class_init, (GClassFinalizeFunc) NULL, NULL, sizeof (XfmpcSongDialog), 0, (GInstanceInitFunc) xfmpc_song_dialog_instance_init, NULL };
		xfmpc_song_dialog_type_id = g_type_register_static (XFCE_TYPE_TITLED_DIALOG, "XfmpcSongDialog", &g_define_type_info, 0);
	}
	return xfmpc_song_dialog_type_id;
}




