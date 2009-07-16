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
#include <xfce-arrow-button.h>
#include <stdlib.h>
#include <string.h>
#include <gdk/gdk.h>
#include <glib/gi18n-lib.h>
#include <libxfcegui4/libxfcegui4.h>
#include <libxfce4util/libxfce4util.h>
#include <config.h>


#define XFMPC_TYPE_EXTENDED_INTERFACE (xfmpc_extended_interface_get_type ())
#define XFMPC_EXTENDED_INTERFACE(obj) (G_TYPE_CHECK_INSTANCE_CAST ((obj), XFMPC_TYPE_EXTENDED_INTERFACE, XfmpcExtendedInterface))
#define XFMPC_EXTENDED_INTERFACE_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST ((klass), XFMPC_TYPE_EXTENDED_INTERFACE, XfmpcExtendedInterfaceClass))
#define XFMPC_IS_EXTENDED_INTERFACE(obj) (G_TYPE_CHECK_INSTANCE_TYPE ((obj), XFMPC_TYPE_EXTENDED_INTERFACE))
#define XFMPC_IS_EXTENDED_INTERFACE_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), XFMPC_TYPE_EXTENDED_INTERFACE))
#define XFMPC_EXTENDED_INTERFACE_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj), XFMPC_TYPE_EXTENDED_INTERFACE, XfmpcExtendedInterfaceClass))

typedef struct _XfmpcExtendedInterface XfmpcExtendedInterface;
typedef struct _XfmpcExtendedInterfaceClass XfmpcExtendedInterfaceClass;
typedef struct _XfmpcExtendedInterfacePrivate XfmpcExtendedInterfacePrivate;

#define XFMPC_TYPE_PREFERENCES (xfmpc_preferences_get_type ())
#define XFMPC_PREFERENCES(obj) (G_TYPE_CHECK_INSTANCE_CAST ((obj), XFMPC_TYPE_PREFERENCES, XfmpcPreferences))
#define XFMPC_PREFERENCES_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST ((klass), XFMPC_TYPE_PREFERENCES, XfmpcPreferencesClass))
#define XFMPC_IS_PREFERENCES(obj) (G_TYPE_CHECK_INSTANCE_TYPE ((obj), XFMPC_TYPE_PREFERENCES))
#define XFMPC_IS_PREFERENCES_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), XFMPC_TYPE_PREFERENCES))
#define XFMPC_PREFERENCES_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj), XFMPC_TYPE_PREFERENCES, XfmpcPreferencesClass))

typedef struct _XfmpcPreferences XfmpcPreferences;
typedef struct _XfmpcPreferencesClass XfmpcPreferencesClass;

#define XFMPC_EXTENDED_INTERFACE_TYPE_COLUMNS (xfmpc_extended_interface_columns_get_type ())

#define XFMPC_EXTENDED_INTERFACE_TYPE_EXTENDED_INTERFACE_WIDGET (xfmpc_extended_interface_extended_interface_widget_get_type ())

#define XFMPC_TYPE_PREFERENCES_DIALOG (xfmpc_preferences_dialog_get_type ())
#define XFMPC_PREFERENCES_DIALOG(obj) (G_TYPE_CHECK_INSTANCE_CAST ((obj), XFMPC_TYPE_PREFERENCES_DIALOG, XfmpcPreferencesDialog))
#define XFMPC_PREFERENCES_DIALOG_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST ((klass), XFMPC_TYPE_PREFERENCES_DIALOG, XfmpcPreferencesDialogClass))
#define XFMPC_IS_PREFERENCES_DIALOG(obj) (G_TYPE_CHECK_INSTANCE_TYPE ((obj), XFMPC_TYPE_PREFERENCES_DIALOG))
#define XFMPC_IS_PREFERENCES_DIALOG_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), XFMPC_TYPE_PREFERENCES_DIALOG))
#define XFMPC_PREFERENCES_DIALOG_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj), XFMPC_TYPE_PREFERENCES_DIALOG, XfmpcPreferencesDialogClass))

typedef struct _XfmpcPreferencesDialog XfmpcPreferencesDialog;
typedef struct _XfmpcPreferencesDialogClass XfmpcPreferencesDialogClass;

#define XFMPC_TYPE_PLAYLIST (xfmpc_playlist_get_type ())
#define XFMPC_PLAYLIST(obj) (G_TYPE_CHECK_INSTANCE_CAST ((obj), XFMPC_TYPE_PLAYLIST, XfmpcPlaylist))
#define XFMPC_PLAYLIST_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST ((klass), XFMPC_TYPE_PLAYLIST, XfmpcPlaylistClass))
#define XFMPC_IS_PLAYLIST(obj) (G_TYPE_CHECK_INSTANCE_TYPE ((obj), XFMPC_TYPE_PLAYLIST))
#define XFMPC_IS_PLAYLIST_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), XFMPC_TYPE_PLAYLIST))
#define XFMPC_PLAYLIST_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj), XFMPC_TYPE_PLAYLIST, XfmpcPlaylistClass))

typedef struct _XfmpcPlaylist XfmpcPlaylist;
typedef struct _XfmpcPlaylistClass XfmpcPlaylistClass;

#define XFMPC_TYPE_DBBROWSER (xfmpc_dbbrowser_get_type ())
#define XFMPC_DBBROWSER(obj) (G_TYPE_CHECK_INSTANCE_CAST ((obj), XFMPC_TYPE_DBBROWSER, XfmpcDbbrowser))
#define XFMPC_DBBROWSER_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST ((klass), XFMPC_TYPE_DBBROWSER, XfmpcDbbrowserClass))
#define XFMPC_IS_DBBROWSER(obj) (G_TYPE_CHECK_INSTANCE_TYPE ((obj), XFMPC_TYPE_DBBROWSER))
#define XFMPC_IS_DBBROWSER_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), XFMPC_TYPE_DBBROWSER))
#define XFMPC_DBBROWSER_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj), XFMPC_TYPE_DBBROWSER, XfmpcDbbrowserClass))

typedef struct _XfmpcDbbrowser XfmpcDbbrowser;
typedef struct _XfmpcDbbrowserClass XfmpcDbbrowserClass;

struct _XfmpcExtendedInterface {
	GtkVBox parent_instance;
	XfmpcExtendedInterfacePrivate * priv;
};

struct _XfmpcExtendedInterfaceClass {
	GtkVBoxClass parent_class;
};

struct _XfmpcExtendedInterfacePrivate {
	XfmpcMpdclient* mpdclient;
	XfmpcPreferences* preferences;
	GtkListStore* list_store;
	GtkComboBox* combobox;
	GtkNotebook* notebook;
	GtkMenu* context_menu;
};

typedef enum  {
	XFMPC_EXTENDED_INTERFACE_COLUMNS_COLUMN_STRING,
	XFMPC_EXTENDED_INTERFACE_COLUMNS_COLUMN_POINTER,
	XFMPC_EXTENDED_INTERFACE_COLUMNS_N_COLUMNS
} XfmpcExtendedInterfaceColumns;

typedef enum  {
	XFMPC_EXTENDED_INTERFACE_EXTENDED_INTERFACE_WIDGET_PLAYLIST,
	XFMPC_EXTENDED_INTERFACE_EXTENDED_INTERFACE_WIDGET_DBBROWSER
} XfmpcExtendedInterfaceExtendedInterfaceWidget;


static XfceArrowButton* xfmpc_extended_interface_context_button;
static XfceArrowButton* xfmpc_extended_interface_context_button = NULL;
static gpointer xfmpc_extended_interface_parent_class = NULL;

GType xfmpc_extended_interface_get_type (void);
GType xfmpc_preferences_get_type (void);
#define XFMPC_EXTENDED_INTERFACE_GET_PRIVATE(o) (G_TYPE_INSTANCE_GET_PRIVATE ((o), XFMPC_TYPE_EXTENDED_INTERFACE, XfmpcExtendedInterfacePrivate))
enum  {
	XFMPC_EXTENDED_INTERFACE_DUMMY_PROPERTY
};
static GType xfmpc_extended_interface_columns_get_type (void);
GType xfmpc_extended_interface_extended_interface_widget_get_type (void);
void xfmpc_extended_interface_set_active (XfmpcExtendedInterface* self, XfmpcExtendedInterfaceExtendedInterfaceWidget active_widget);
static void xfmpc_extended_interface_append_child (XfmpcExtendedInterface* self, GtkWidget* child, const char* title);
static void xfmpc_extended_interface_context_menu_new (XfmpcExtendedInterface* self, GtkWidget* attach_widget);
static void xfmpc_extended_interface_position_context_menu (GtkMenu* menu, gint x, gint y, gboolean push_in);
static void xfmpc_extended_interface_popup_context_menu (XfmpcExtendedInterface* self);
static void xfmpc_extended_interface_menu_detach (XfmpcExtendedInterface* self, GtkWidget* attach_widget, GtkMenu* menu);
static void xfmpc_extended_interface_cb_context_menu_deactivate (XfmpcExtendedInterface* self);
static void _xfmpc_extended_interface_cb_context_menu_deactivate_gtk_menu_shell_deactivate (GtkMenuShell* _sender, gpointer self);
static void xfmpc_extended_interface_cb_repeat_switch (XfmpcExtendedInterface* self);
static void _xfmpc_extended_interface_cb_repeat_switch_gtk_menu_item_activate (GtkMenuItem* _sender, gpointer self);
static void xfmpc_extended_interface_cb_random_switch (XfmpcExtendedInterface* self);
static void _xfmpc_extended_interface_cb_random_switch_gtk_menu_item_activate (GtkMenuItem* _sender, gpointer self);
static void xfmpc_extended_interface_cb_preferences (XfmpcExtendedInterface* self);
static void _xfmpc_extended_interface_cb_preferences_gtk_menu_item_activate (GtkMenuItem* _sender, gpointer self);
static void xfmpc_extended_interface_cb_about (XfmpcExtendedInterface* self);
static void _xfmpc_extended_interface_cb_about_gtk_menu_item_activate (GtkMenuItem* _sender, gpointer self);
static void xfmpc_extended_interface_cb_playlist_clear (XfmpcExtendedInterface* self);
static void xfmpc_extended_interface_cb_database_refresh (XfmpcExtendedInterface* self);
static void xfmpc_extended_interface_cb_interface_changed (XfmpcExtendedInterface* self);
static void xfmpc_extended_interface_cb_context_menu_clicked (XfmpcExtendedInterface* self);
XfmpcPreferencesDialog* xfmpc_preferences_dialog_new (void);
XfmpcPreferencesDialog* xfmpc_preferences_dialog_construct (GType object_type);
GType xfmpc_preferences_dialog_get_type (void);
XfmpcExtendedInterface* xfmpc_extended_interface_new (void);
XfmpcExtendedInterface* xfmpc_extended_interface_construct (GType object_type);
XfmpcExtendedInterface* xfmpc_extended_interface_new (void);
XfmpcPreferences* xfmpc_preferences_get (void);
static void _xfmpc_extended_interface_cb_playlist_clear_gtk_button_clicked (GtkButton* _sender, gpointer self);
static void _xfmpc_extended_interface_cb_database_refresh_gtk_button_clicked (GtkButton* _sender, gpointer self);
static void _xfmpc_extended_interface_popup_context_menu_gtk_button_pressed (GtkButton* _sender, gpointer self);
static void _xfmpc_extended_interface_cb_context_menu_clicked_gtk_button_clicked (GtkButton* _sender, gpointer self);
static void _xfmpc_extended_interface_cb_interface_changed_gtk_combo_box_changed (GtkComboBox* _sender, gpointer self);
XfmpcPlaylist* xfmpc_playlist_new (void);
XfmpcPlaylist* xfmpc_playlist_construct (GType object_type);
GType xfmpc_playlist_get_type (void);
XfmpcDbbrowser* xfmpc_dbbrowser_new (void);
XfmpcDbbrowser* xfmpc_dbbrowser_construct (GType object_type);
GType xfmpc_dbbrowser_get_type (void);
static GObject * xfmpc_extended_interface_constructor (GType type, guint n_construct_properties, GObjectConstructParam * construct_properties);
static void xfmpc_extended_interface_finalize (GObject* obj);
static void _vala_array_destroy (gpointer array, gint array_length, GDestroyNotify destroy_func);
static void _vala_array_free (gpointer array, gint array_length, GDestroyNotify destroy_func);




static GType xfmpc_extended_interface_columns_get_type (void) {
	static GType xfmpc_extended_interface_columns_type_id = 0;
	if (G_UNLIKELY (xfmpc_extended_interface_columns_type_id == 0)) {
		static const GEnumValue values[] = {{XFMPC_EXTENDED_INTERFACE_COLUMNS_COLUMN_STRING, "XFMPC_EXTENDED_INTERFACE_COLUMNS_COLUMN_STRING", "column-string"}, {XFMPC_EXTENDED_INTERFACE_COLUMNS_COLUMN_POINTER, "XFMPC_EXTENDED_INTERFACE_COLUMNS_COLUMN_POINTER", "column-pointer"}, {XFMPC_EXTENDED_INTERFACE_COLUMNS_N_COLUMNS, "XFMPC_EXTENDED_INTERFACE_COLUMNS_N_COLUMNS", "n-columns"}, {0, NULL, NULL}};
		xfmpc_extended_interface_columns_type_id = g_enum_register_static ("XfmpcExtendedInterfaceColumns", values);
	}
	return xfmpc_extended_interface_columns_type_id;
}



GType xfmpc_extended_interface_extended_interface_widget_get_type (void) {
	static GType xfmpc_extended_interface_extended_interface_widget_type_id = 0;
	if (G_UNLIKELY (xfmpc_extended_interface_extended_interface_widget_type_id == 0)) {
		static const GEnumValue values[] = {{XFMPC_EXTENDED_INTERFACE_EXTENDED_INTERFACE_WIDGET_PLAYLIST, "XFMPC_EXTENDED_INTERFACE_EXTENDED_INTERFACE_WIDGET_PLAYLIST", "playlist"}, {XFMPC_EXTENDED_INTERFACE_EXTENDED_INTERFACE_WIDGET_DBBROWSER, "XFMPC_EXTENDED_INTERFACE_EXTENDED_INTERFACE_WIDGET_DBBROWSER", "dbbrowser"}, {0, NULL, NULL}};
		xfmpc_extended_interface_extended_interface_widget_type_id = g_enum_register_static ("XfmpcExtendedInterfaceExtendedInterfaceWidget", values);
	}
	return xfmpc_extended_interface_extended_interface_widget_type_id;
}


void xfmpc_extended_interface_set_active (XfmpcExtendedInterface* self, XfmpcExtendedInterfaceExtendedInterfaceWidget active_widget) {
	g_return_if_fail (self != NULL);
	gtk_combo_box_set_active (self->priv->combobox, (gint) active_widget);
}


static void xfmpc_extended_interface_append_child (XfmpcExtendedInterface* self, GtkWidget* child, const char* title) {
	GtkTreeIter iter = {0};
	g_return_if_fail (self != NULL);
	g_return_if_fail (child != NULL);
	g_return_if_fail (title != NULL);
	gtk_list_store_append (self->priv->list_store, &iter);
	gtk_list_store_set (self->priv->list_store, &iter, XFMPC_EXTENDED_INTERFACE_COLUMNS_COLUMN_STRING, title, XFMPC_EXTENDED_INTERFACE_COLUMNS_COLUMN_POINTER, child, -1, -1);
	if (gtk_combo_box_get_active (self->priv->combobox) == (-1)) {
		gtk_combo_box_set_active (self->priv->combobox, 0);
	}
	gtk_notebook_append_page (self->priv->notebook, child, NULL);
	gtk_notebook_set_tab_label_packing (self->priv->notebook, child, TRUE, TRUE, GTK_PACK_START);
}


static void xfmpc_extended_interface_popup_context_menu (XfmpcExtendedInterface* self) {
	g_return_if_fail (self != NULL);
	if (self->priv->context_menu == NULL) {
		xfmpc_extended_interface_context_menu_new (self, GTK_WIDGET (xfmpc_extended_interface_context_button));
	}
	gtk_menu_popup (self->priv->context_menu, NULL, NULL, (GtkMenuPositionFunc) xfmpc_extended_interface_position_context_menu, NULL, (guint) 0, gtk_get_current_event_time ());
}


static void xfmpc_extended_interface_position_context_menu (GtkMenu* menu, gint x, gint y, gboolean push_in) {
	GtkRequisition menu_req = {0};
	gint root_x;
	gint root_y;
	g_return_if_fail (menu != NULL);
	root_x = 0;
	root_y = 0;
	gtk_widget_size_request ((GtkWidget*) menu, &menu_req);
	gdk_window_get_origin (GTK_WIDGET (xfmpc_extended_interface_context_button)->window, &root_x, &root_y);
	x = root_x + GTK_WIDGET (xfmpc_extended_interface_context_button)->allocation.x;
	y = root_y + GTK_WIDGET (xfmpc_extended_interface_context_button)->allocation.y;
	if (y > (gdk_screen_height () - menu_req.height)) {
		y = gdk_screen_height () - menu_req.height;
	} else {
		if (y < 0) {
			y = 0;
		}
	}
	push_in = FALSE;
}


static void _xfmpc_extended_interface_cb_context_menu_deactivate_gtk_menu_shell_deactivate (GtkMenuShell* _sender, gpointer self) {
	xfmpc_extended_interface_cb_context_menu_deactivate (self);
}


static void _xfmpc_extended_interface_cb_repeat_switch_gtk_menu_item_activate (GtkMenuItem* _sender, gpointer self) {
	xfmpc_extended_interface_cb_repeat_switch (self);
}


static void _xfmpc_extended_interface_cb_random_switch_gtk_menu_item_activate (GtkMenuItem* _sender, gpointer self) {
	xfmpc_extended_interface_cb_random_switch (self);
}


static void _xfmpc_extended_interface_cb_preferences_gtk_menu_item_activate (GtkMenuItem* _sender, gpointer self) {
	xfmpc_extended_interface_cb_preferences (self);
}


static void _xfmpc_extended_interface_cb_about_gtk_menu_item_activate (GtkMenuItem* _sender, gpointer self) {
	xfmpc_extended_interface_cb_about (self);
}


static void xfmpc_extended_interface_context_menu_new (XfmpcExtendedInterface* self, GtkWidget* attach_widget) {
	GtkMenu* _tmp0_;
	GtkCheckMenuItem* mi;
	GtkCheckMenuItem* _tmp1_;
	GtkSeparatorMenuItem* separator;
	GtkImageMenuItem* imi;
	GtkImageMenuItem* _tmp2_;
	g_return_if_fail (self != NULL);
	g_return_if_fail (attach_widget != NULL);
	_tmp0_ = NULL;
	self->priv->context_menu = (_tmp0_ = g_object_ref_sink ((GtkMenu*) gtk_menu_new ()), (self->priv->context_menu == NULL) ? NULL : (self->priv->context_menu = (g_object_unref (self->priv->context_menu), NULL)), _tmp0_);
	gtk_menu_set_screen (self->priv->context_menu, gtk_widget_get_screen (attach_widget));
	gtk_menu_attach_to_widget (self->priv->context_menu, attach_widget, (GtkMenuDetachFunc) xfmpc_extended_interface_menu_detach);
	g_signal_connect_object ((GtkMenuShell*) self->priv->context_menu, "deactivate", (GCallback) _xfmpc_extended_interface_cb_context_menu_deactivate_gtk_menu_shell_deactivate, self, 0);
	mi = g_object_ref_sink ((GtkCheckMenuItem*) gtk_check_menu_item_new_with_label (_ ("Repeat")));
	gtk_check_menu_item_set_active (mi, xfmpc_mpdclient_get_repeat (self->priv->mpdclient));
	g_signal_connect_object ((GtkMenuItem*) mi, "activate", (GCallback) _xfmpc_extended_interface_cb_repeat_switch_gtk_menu_item_activate, self, 0);
	gtk_menu_shell_append ((GtkMenuShell*) self->priv->context_menu, (GtkWidget*) ((GtkMenuItem*) mi));
	_tmp1_ = NULL;
	mi = (_tmp1_ = g_object_ref_sink ((GtkCheckMenuItem*) gtk_check_menu_item_new_with_label (_ ("Random"))), (mi == NULL) ? NULL : (mi = (g_object_unref (mi), NULL)), _tmp1_);
	gtk_check_menu_item_set_active (mi, xfmpc_mpdclient_get_random (self->priv->mpdclient));
	g_signal_connect_object ((GtkMenuItem*) mi, "activate", (GCallback) _xfmpc_extended_interface_cb_random_switch_gtk_menu_item_activate, self, 0);
	gtk_menu_shell_append ((GtkMenuShell*) self->priv->context_menu, (GtkWidget*) ((GtkMenuItem*) mi));
	separator = g_object_ref_sink ((GtkSeparatorMenuItem*) gtk_separator_menu_item_new ());
	gtk_menu_shell_append ((GtkMenuShell*) self->priv->context_menu, (GtkWidget*) ((GtkMenuItem*) separator));
	imi = g_object_ref_sink ((GtkImageMenuItem*) gtk_image_menu_item_new_from_stock (GTK_STOCK_PREFERENCES, NULL));
	g_signal_connect_object ((GtkMenuItem*) imi, "activate", (GCallback) _xfmpc_extended_interface_cb_preferences_gtk_menu_item_activate, self, 0);
	gtk_menu_shell_append ((GtkMenuShell*) self->priv->context_menu, (GtkWidget*) ((GtkMenuItem*) imi));
	_tmp2_ = NULL;
	imi = (_tmp2_ = g_object_ref_sink ((GtkImageMenuItem*) gtk_image_menu_item_new_from_stock (GTK_STOCK_ABOUT, NULL)), (imi == NULL) ? NULL : (imi = (g_object_unref (imi), NULL)), _tmp2_);
	g_signal_connect_object ((GtkMenuItem*) imi, "activate", (GCallback) _xfmpc_extended_interface_cb_about_gtk_menu_item_activate, self, 0);
	gtk_menu_shell_append ((GtkMenuShell*) self->priv->context_menu, (GtkWidget*) ((GtkMenuItem*) imi));
	gtk_widget_show_all ((GtkWidget*) self->priv->context_menu);
	(mi == NULL) ? NULL : (mi = (g_object_unref (mi), NULL));
	(separator == NULL) ? NULL : (separator = (g_object_unref (separator), NULL));
	(imi == NULL) ? NULL : (imi = (g_object_unref (imi), NULL));
}


static void xfmpc_extended_interface_menu_detach (XfmpcExtendedInterface* self, GtkWidget* attach_widget, GtkMenu* menu) {
	g_return_if_fail (self != NULL);
	g_return_if_fail (attach_widget != NULL);
	g_return_if_fail (menu != NULL);
}


/*
 * Signal callbacks
 */
static void xfmpc_extended_interface_cb_playlist_clear (XfmpcExtendedInterface* self) {
	g_return_if_fail (self != NULL);
	xfmpc_mpdclient_playlist_clear (self->priv->mpdclient);
}


static void xfmpc_extended_interface_cb_database_refresh (XfmpcExtendedInterface* self) {
	g_return_if_fail (self != NULL);
	xfmpc_mpdclient_database_refresh (self->priv->mpdclient);
}


static void xfmpc_extended_interface_cb_interface_changed (XfmpcExtendedInterface* self) {
	void* child;
	GtkTreeIter iter = {0};
	gint i;
	g_return_if_fail (self != NULL);
	child = NULL;
	i = 0;
	if (gtk_combo_box_get_active_iter (self->priv->combobox, &iter) == FALSE) {
		return;
	}
	gtk_tree_model_get (GTK_TREE_MODEL (self->priv->list_store), &iter, XFMPC_EXTENDED_INTERFACE_COLUMNS_COLUMN_POINTER, &child, -1, -1);
	i = gtk_notebook_page_num (self->priv->notebook, GTK_WIDGET (child));
	gtk_notebook_set_current_page (self->priv->notebook, i);
}


static void xfmpc_extended_interface_cb_context_menu_clicked (XfmpcExtendedInterface* self) {
	g_return_if_fail (self != NULL);
	if (!gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (xfmpc_extended_interface_context_button))) {
		return;
	}
	xfmpc_extended_interface_popup_context_menu (self);
}


static void xfmpc_extended_interface_cb_context_menu_deactivate (XfmpcExtendedInterface* self) {
	g_return_if_fail (self != NULL);
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (xfmpc_extended_interface_context_button), FALSE);
}


static void xfmpc_extended_interface_cb_repeat_switch (XfmpcExtendedInterface* self) {
	g_return_if_fail (self != NULL);
	xfmpc_mpdclient_set_repeat (self->priv->mpdclient, !xfmpc_mpdclient_get_repeat (self->priv->mpdclient));
}


static void xfmpc_extended_interface_cb_random_switch (XfmpcExtendedInterface* self) {
	g_return_if_fail (self != NULL);
	xfmpc_mpdclient_set_random (self->priv->mpdclient, !xfmpc_mpdclient_get_random (self->priv->mpdclient));
}


static void xfmpc_extended_interface_cb_preferences (XfmpcExtendedInterface* self) {
	XfmpcPreferencesDialog* dialog;
	g_return_if_fail (self != NULL);
	dialog = g_object_ref_sink (xfmpc_preferences_dialog_new ());
	gtk_widget_show ((GtkWidget*) dialog);
	(dialog == NULL) ? NULL : (dialog = (g_object_unref (dialog), NULL));
}


static void xfmpc_extended_interface_cb_about (XfmpcExtendedInterface* self) {
	char** _tmp1_;
	gint authors_size;
	gint authors_length1;
	char** _tmp0_;
	char** authors;
	g_return_if_fail (self != NULL);
	_tmp1_ = NULL;
	_tmp0_ = NULL;
	authors = (_tmp1_ = (_tmp0_ = g_new0 (char*, 2 + 1), _tmp0_[0] = g_strdup ("Mike Massonnet <mmassonnet@xfce.org>"), _tmp0_[1] = g_strdup ("Vincent Legout <vincent@xfce.org>"), _tmp0_), authors_length1 = 2, authors_size = authors_length1, _tmp1_);
	gtk_show_about_dialog (GTK_WINDOW (gtk_widget_get_toplevel (GTK_WIDGET (self))), "artists", NULL, "authors", authors, "comments", _ ("MPD client written in GTK+ for Xfce"), "copyright", "Copyright \302\251 2008-2009 Mike Massonnet, Vincent Legout", "documenters", NULL, "license", xfce_get_license_text (XFCE_LICENSE_TEXT_GPL), "translator-credits", _ ("translator-credits"), "version", PACKAGE_VERSION, "website", "http://goodies.xfce.org/projects/applications/xfmpc", NULL, NULL);
	authors = (_vala_array_free (authors, authors_length1, (GDestroyNotify) g_free), NULL);
}


XfmpcExtendedInterface* xfmpc_extended_interface_construct (GType object_type) {
	XfmpcExtendedInterface * self;
	self = g_object_newv (object_type, 0, NULL);
	return self;
}


XfmpcExtendedInterface* xfmpc_extended_interface_new (void) {
	return xfmpc_extended_interface_construct (XFMPC_TYPE_EXTENDED_INTERFACE);
}


static void _xfmpc_extended_interface_cb_playlist_clear_gtk_button_clicked (GtkButton* _sender, gpointer self) {
	xfmpc_extended_interface_cb_playlist_clear (self);
}


static void _xfmpc_extended_interface_cb_database_refresh_gtk_button_clicked (GtkButton* _sender, gpointer self) {
	xfmpc_extended_interface_cb_database_refresh (self);
}


static void _xfmpc_extended_interface_popup_context_menu_gtk_button_pressed (GtkButton* _sender, gpointer self) {
	xfmpc_extended_interface_popup_context_menu (self);
}


static void _xfmpc_extended_interface_cb_context_menu_clicked_gtk_button_clicked (GtkButton* _sender, gpointer self) {
	xfmpc_extended_interface_cb_context_menu_clicked (self);
}


static void _xfmpc_extended_interface_cb_interface_changed_gtk_combo_box_changed (GtkComboBox* _sender, gpointer self) {
	xfmpc_extended_interface_cb_interface_changed (self);
}


static GObject * xfmpc_extended_interface_constructor (GType type, guint n_construct_properties, GObjectConstructParam * construct_properties) {
	GObject * obj;
	XfmpcExtendedInterfaceClass * klass;
	GObjectClass * parent_class;
	XfmpcExtendedInterface * self;
	klass = XFMPC_EXTENDED_INTERFACE_CLASS (g_type_class_peek (XFMPC_TYPE_EXTENDED_INTERFACE));
	parent_class = G_OBJECT_CLASS (g_type_class_peek_parent (klass));
	obj = parent_class->constructor (type, n_construct_properties, construct_properties);
	self = XFMPC_EXTENDED_INTERFACE (obj);
	{
		GtkHBox* hbox;
		GtkButton* button;
		GtkImage* image;
		GtkButton* _tmp0_;
		GtkImage* _tmp1_;
		XfceArrowButton* _tmp2_;
		GtkListStore* _tmp3_;
		GtkComboBox* _tmp4_;
		GtkCellRendererText* cell;
		GtkNotebook* _tmp5_;
		GtkWidget* playlist;
		GtkWidget* dbbrowser;
		self->priv->mpdclient = xfmpc_mpdclient_get ();
		self->priv->preferences = xfmpc_preferences_get ();
		hbox = g_object_ref_sink ((GtkHBox*) gtk_hbox_new (FALSE, 2));
		gtk_box_pack_start ((GtkBox*) self, (GtkWidget*) hbox, FALSE, FALSE, (guint) 2);
		button = g_object_ref_sink ((GtkButton*) gtk_button_new ());
		gtk_widget_set_tooltip_text ((GtkWidget*) button, _ ("Clear Playlist"));
		g_signal_connect_object (button, "clicked", (GCallback) _xfmpc_extended_interface_cb_playlist_clear_gtk_button_clicked, self, 0);
		gtk_box_pack_start ((GtkBox*) hbox, (GtkWidget*) button, FALSE, FALSE, (guint) 0);
		image = g_object_ref_sink ((GtkImage*) gtk_image_new_from_stock (GTK_STOCK_NEW, GTK_ICON_SIZE_MENU));
		gtk_button_set_image (button, (GtkWidget*) image);
		_tmp0_ = NULL;
		button = (_tmp0_ = g_object_ref_sink ((GtkButton*) gtk_button_new ()), (button == NULL) ? NULL : (button = (g_object_unref (button), NULL)), _tmp0_);
		gtk_widget_set_tooltip_text ((GtkWidget*) button, _ ("Refresh Database"));
		g_signal_connect_object (button, "clicked", (GCallback) _xfmpc_extended_interface_cb_database_refresh_gtk_button_clicked, self, 0);
		gtk_box_pack_start ((GtkBox*) hbox, (GtkWidget*) button, FALSE, FALSE, (guint) 0);
		_tmp1_ = NULL;
		image = (_tmp1_ = g_object_ref_sink ((GtkImage*) gtk_image_new_from_stock (GTK_STOCK_REFRESH, GTK_ICON_SIZE_MENU)), (image == NULL) ? NULL : (image = (g_object_unref (image), NULL)), _tmp1_);
		gtk_button_set_image (button, (GtkWidget*) image);
		_tmp2_ = NULL;
		xfmpc_extended_interface_context_button = (_tmp2_ = (XfceArrowButton*) xfce_arrow_button_new (GTK_ARROW_DOWN), (xfmpc_extended_interface_context_button == NULL) ? NULL : (xfmpc_extended_interface_context_button = ( (xfmpc_extended_interface_context_button), NULL)), _tmp2_);
		gtk_widget_set_tooltip_text (GTK_WIDGET (xfmpc_extended_interface_context_button), _ ("Context Menu"));
		g_signal_connect_object (GTK_BUTTON (xfmpc_extended_interface_context_button), "pressed", (GCallback) _xfmpc_extended_interface_popup_context_menu_gtk_button_pressed, self, 0);
		g_signal_connect_object (GTK_BUTTON (xfmpc_extended_interface_context_button), "clicked", (GCallback) _xfmpc_extended_interface_cb_context_menu_clicked_gtk_button_clicked, self, 0);
		gtk_box_pack_start ((GtkBox*) hbox, GTK_WIDGET (xfmpc_extended_interface_context_button), FALSE, FALSE, (guint) 0);
		_tmp3_ = NULL;
		self->priv->list_store = (_tmp3_ = gtk_list_store_new ((gint) XFMPC_EXTENDED_INTERFACE_COLUMNS_N_COLUMNS, G_TYPE_STRING, G_TYPE_POINTER, NULL), (self->priv->list_store == NULL) ? NULL : (self->priv->list_store = (g_object_unref (self->priv->list_store), NULL)), _tmp3_);
		_tmp4_ = NULL;
		self->priv->combobox = (_tmp4_ = g_object_ref_sink ((GtkComboBox*) gtk_combo_box_new_with_model ((GtkTreeModel*) self->priv->list_store)), (self->priv->combobox == NULL) ? NULL : (self->priv->combobox = (g_object_unref (self->priv->combobox), NULL)), _tmp4_);
		gtk_box_pack_start ((GtkBox*) hbox, (GtkWidget*) self->priv->combobox, TRUE, TRUE, (guint) 0);
		g_signal_connect_object (self->priv->combobox, "changed", (GCallback) _xfmpc_extended_interface_cb_interface_changed_gtk_combo_box_changed, self, 0);
		cell = g_object_ref_sink ((GtkCellRendererText*) gtk_cell_renderer_text_new ());
		gtk_cell_layout_pack_start ((GtkCellLayout*) self->priv->combobox, (GtkCellRenderer*) cell, TRUE);
		gtk_cell_layout_set_attributes ((GtkCellLayout*) self->priv->combobox, (GtkCellRenderer*) cell, "text", XFMPC_EXTENDED_INTERFACE_COLUMNS_COLUMN_STRING, NULL, NULL);
		_tmp5_ = NULL;
		self->priv->notebook = (_tmp5_ = g_object_ref_sink ((GtkNotebook*) gtk_notebook_new ()), (self->priv->notebook == NULL) ? NULL : (self->priv->notebook = (g_object_unref (self->priv->notebook), NULL)), _tmp5_);
		gtk_notebook_set_show_tabs (self->priv->notebook, FALSE);
		gtk_box_pack_start ((GtkBox*) self, (GtkWidget*) self->priv->notebook, TRUE, TRUE, (guint) 0);
		playlist = GTK_WIDGET (g_object_ref_sink (xfmpc_playlist_new ()));
		xfmpc_extended_interface_append_child (self, playlist, _ ("Current Playlist"));
		dbbrowser = GTK_WIDGET (g_object_ref_sink (xfmpc_dbbrowser_new ()));
		xfmpc_extended_interface_append_child (self, dbbrowser, _ ("Browse database"));
		g_object_set_data ((GObject*) playlist, "XfmpcDbbrowser", dbbrowser);
		g_object_set_data ((GObject*) dbbrowser, "XfmpcPlaylist", playlist);
		g_object_set_data ((GObject*) playlist, "XfmpcExtendedInterface", self);
		(hbox == NULL) ? NULL : (hbox = (g_object_unref (hbox), NULL));
		(button == NULL) ? NULL : (button = (g_object_unref (button), NULL));
		(image == NULL) ? NULL : (image = (g_object_unref (image), NULL));
		(cell == NULL) ? NULL : (cell = (g_object_unref (cell), NULL));
		(playlist == NULL) ? NULL : (playlist = (g_object_unref (playlist), NULL));
		(dbbrowser == NULL) ? NULL : (dbbrowser = (g_object_unref (dbbrowser), NULL));
	}
	return obj;
}


static void xfmpc_extended_interface_class_init (XfmpcExtendedInterfaceClass * klass) {
	xfmpc_extended_interface_parent_class = g_type_class_peek_parent (klass);
	g_type_class_add_private (klass, sizeof (XfmpcExtendedInterfacePrivate));
	G_OBJECT_CLASS (klass)->constructor = xfmpc_extended_interface_constructor;
	G_OBJECT_CLASS (klass)->finalize = xfmpc_extended_interface_finalize;
}


static void xfmpc_extended_interface_instance_init (XfmpcExtendedInterface * self) {
	self->priv = XFMPC_EXTENDED_INTERFACE_GET_PRIVATE (self);
}


static void xfmpc_extended_interface_finalize (GObject* obj) {
	XfmpcExtendedInterface * self;
	self = XFMPC_EXTENDED_INTERFACE (obj);
	(self->priv->list_store == NULL) ? NULL : (self->priv->list_store = (g_object_unref (self->priv->list_store), NULL));
	(self->priv->combobox == NULL) ? NULL : (self->priv->combobox = (g_object_unref (self->priv->combobox), NULL));
	(self->priv->notebook == NULL) ? NULL : (self->priv->notebook = (g_object_unref (self->priv->notebook), NULL));
	(self->priv->context_menu == NULL) ? NULL : (self->priv->context_menu = (g_object_unref (self->priv->context_menu), NULL));
	G_OBJECT_CLASS (xfmpc_extended_interface_parent_class)->finalize (obj);
}


GType xfmpc_extended_interface_get_type (void) {
	static GType xfmpc_extended_interface_type_id = 0;
	if (xfmpc_extended_interface_type_id == 0) {
		static const GTypeInfo g_define_type_info = { sizeof (XfmpcExtendedInterfaceClass), (GBaseInitFunc) NULL, (GBaseFinalizeFunc) NULL, (GClassInitFunc) xfmpc_extended_interface_class_init, (GClassFinalizeFunc) NULL, NULL, sizeof (XfmpcExtendedInterface), 0, (GInstanceInitFunc) xfmpc_extended_interface_instance_init, NULL };
		xfmpc_extended_interface_type_id = g_type_register_static (GTK_TYPE_VBOX, "XfmpcExtendedInterface", &g_define_type_info, 0);
	}
	return xfmpc_extended_interface_type_id;
}


static void _vala_array_destroy (gpointer array, gint array_length, GDestroyNotify destroy_func) {
	if ((array != NULL) && (destroy_func != NULL)) {
		int i;
		for (i = 0; i < array_length; i = i + 1) {
			if (((gpointer*) array)[i] != NULL) {
				destroy_func (((gpointer*) array)[i]);
			}
		}
	}
}


static void _vala_array_free (gpointer array, gint array_length, GDestroyNotify destroy_func) {
	_vala_array_destroy (array, array_length, destroy_func);
	g_free (array);
}




