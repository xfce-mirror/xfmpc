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
#include <libxfcegui4/libxfcegui4.h>
#include <gdk/gdk.h>
#include <pango/pango.h>
#include <glib/gi18n-lib.h>


#define XFMPC_TYPE_PLAYLIST (xfmpc_playlist_get_type ())
#define XFMPC_PLAYLIST(obj) (G_TYPE_CHECK_INSTANCE_CAST ((obj), XFMPC_TYPE_PLAYLIST, XfmpcPlaylist))
#define XFMPC_PLAYLIST_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST ((klass), XFMPC_TYPE_PLAYLIST, XfmpcPlaylistClass))
#define XFMPC_IS_PLAYLIST(obj) (G_TYPE_CHECK_INSTANCE_TYPE ((obj), XFMPC_TYPE_PLAYLIST))
#define XFMPC_IS_PLAYLIST_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), XFMPC_TYPE_PLAYLIST))
#define XFMPC_PLAYLIST_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj), XFMPC_TYPE_PLAYLIST, XfmpcPlaylistClass))

typedef struct _XfmpcPlaylist XfmpcPlaylist;
typedef struct _XfmpcPlaylistClass XfmpcPlaylistClass;
typedef struct _XfmpcPlaylistPrivate XfmpcPlaylistPrivate;

#define XFMPC_TYPE_PREFERENCES (xfmpc_preferences_get_type ())
#define XFMPC_PREFERENCES(obj) (G_TYPE_CHECK_INSTANCE_CAST ((obj), XFMPC_TYPE_PREFERENCES, XfmpcPreferences))
#define XFMPC_PREFERENCES_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST ((klass), XFMPC_TYPE_PREFERENCES, XfmpcPreferencesClass))
#define XFMPC_IS_PREFERENCES(obj) (G_TYPE_CHECK_INSTANCE_TYPE ((obj), XFMPC_TYPE_PREFERENCES))
#define XFMPC_IS_PREFERENCES_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), XFMPC_TYPE_PREFERENCES))
#define XFMPC_PREFERENCES_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj), XFMPC_TYPE_PREFERENCES, XfmpcPreferencesClass))

typedef struct _XfmpcPreferences XfmpcPreferences;
typedef struct _XfmpcPreferencesClass XfmpcPreferencesClass;

#define XFMPC_PLAYLIST_TYPE_COLUMNS (xfmpc_playlist_columns_get_type ())

#define XFMPC_TYPE_DBBROWSER (xfmpc_dbbrowser_get_type ())
#define XFMPC_DBBROWSER(obj) (G_TYPE_CHECK_INSTANCE_CAST ((obj), XFMPC_TYPE_DBBROWSER, XfmpcDbbrowser))
#define XFMPC_DBBROWSER_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST ((klass), XFMPC_TYPE_DBBROWSER, XfmpcDbbrowserClass))
#define XFMPC_IS_DBBROWSER(obj) (G_TYPE_CHECK_INSTANCE_TYPE ((obj), XFMPC_TYPE_DBBROWSER))
#define XFMPC_IS_DBBROWSER_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), XFMPC_TYPE_DBBROWSER))
#define XFMPC_DBBROWSER_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj), XFMPC_TYPE_DBBROWSER, XfmpcDbbrowserClass))

typedef struct _XfmpcDbbrowser XfmpcDbbrowser;
typedef struct _XfmpcDbbrowserClass XfmpcDbbrowserClass;

#define XFMPC_TYPE_EXTENDED_INTERFACE (xfmpc_extended_interface_get_type ())
#define XFMPC_EXTENDED_INTERFACE(obj) (G_TYPE_CHECK_INSTANCE_CAST ((obj), XFMPC_TYPE_EXTENDED_INTERFACE, XfmpcExtendedInterface))
#define XFMPC_EXTENDED_INTERFACE_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST ((klass), XFMPC_TYPE_EXTENDED_INTERFACE, XfmpcExtendedInterfaceClass))
#define XFMPC_IS_EXTENDED_INTERFACE(obj) (G_TYPE_CHECK_INSTANCE_TYPE ((obj), XFMPC_TYPE_EXTENDED_INTERFACE))
#define XFMPC_IS_EXTENDED_INTERFACE_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), XFMPC_TYPE_EXTENDED_INTERFACE))
#define XFMPC_EXTENDED_INTERFACE_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj), XFMPC_TYPE_EXTENDED_INTERFACE, XfmpcExtendedInterfaceClass))

typedef struct _XfmpcExtendedInterface XfmpcExtendedInterface;
typedef struct _XfmpcExtendedInterfaceClass XfmpcExtendedInterfaceClass;

#define XFMPC_EXTENDED_INTERFACE_TYPE_EXTENDED_INTERFACE_WIDGET (xfmpc_extended_interface_extended_interface_widget_get_type ())

#define XFMPC_TYPE_SONG_DIALOG (xfmpc_song_dialog_get_type ())
#define XFMPC_SONG_DIALOG(obj) (G_TYPE_CHECK_INSTANCE_CAST ((obj), XFMPC_TYPE_SONG_DIALOG, XfmpcSongDialog))
#define XFMPC_SONG_DIALOG_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST ((klass), XFMPC_TYPE_SONG_DIALOG, XfmpcSongDialogClass))
#define XFMPC_IS_SONG_DIALOG(obj) (G_TYPE_CHECK_INSTANCE_TYPE ((obj), XFMPC_TYPE_SONG_DIALOG))
#define XFMPC_IS_SONG_DIALOG_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), XFMPC_TYPE_SONG_DIALOG))
#define XFMPC_SONG_DIALOG_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj), XFMPC_TYPE_SONG_DIALOG, XfmpcSongDialogClass))

typedef struct _XfmpcSongDialog XfmpcSongDialog;
typedef struct _XfmpcSongDialogClass XfmpcSongDialogClass;

struct _XfmpcPlaylist {
	GtkVBox parent_instance;
	XfmpcPlaylistPrivate * priv;
};

struct _XfmpcPlaylistClass {
	GtkVBoxClass parent_class;
};

struct _XfmpcPlaylistPrivate {
	XfmpcMpdclient* mpdclient;
	XfmpcPreferences* preferences;
	GtkListStore* store;
	GtkTreeModelFilter* filter;
	GtkTreeView* treeview;
	GtkMenu* menu;
	GtkImageMenuItem* mi_browse;
	GtkImageMenuItem* mi_information;
	gint current;
	gboolean autocenter;
};

typedef enum  {
	XFMPC_PLAYLIST_COLUMNS_COLUMN_ID,
	XFMPC_PLAYLIST_COLUMNS_COLUMN_FILENAME,
	XFMPC_PLAYLIST_COLUMNS_COLUMN_POSITION,
	XFMPC_PLAYLIST_COLUMNS_COLUMN_SONG,
	XFMPC_PLAYLIST_COLUMNS_COLUMN_LENGTH,
	XFMPC_PLAYLIST_COLUMNS_COLUMN_WEIGHT,
	XFMPC_PLAYLIST_COLUMNS_N_COLUMNS
} XfmpcPlaylistColumns;

typedef enum  {
	XFMPC_EXTENDED_INTERFACE_EXTENDED_INTERFACE_WIDGET_PLAYLIST,
	XFMPC_EXTENDED_INTERFACE_EXTENDED_INTERFACE_WIDGET_DBBROWSER
} XfmpcExtendedInterfaceExtendedInterfaceWidget;



GType xfmpc_playlist_get_type (void);
GType xfmpc_preferences_get_type (void);
#define XFMPC_PLAYLIST_GET_PRIVATE(o) (G_TYPE_INSTANCE_GET_PRIVATE ((o), XFMPC_TYPE_PLAYLIST, XfmpcPlaylistPrivate))
enum  {
	XFMPC_PLAYLIST_DUMMY_PROPERTY
};
static GType xfmpc_playlist_columns_get_type (void);
static GtkEntry* xfmpc_playlist_filter_entry;
static GtkEntry* xfmpc_playlist_filter_entry = NULL;
static gboolean xfmpc_playlist_visible_func_filter_tree (GtkTreeModel* model, GtkTreeIter* iter);
GType xfmpc_dbbrowser_get_type (void);
GType xfmpc_extended_interface_get_type (void);
static void _g_list_free_gtk_tree_path_free (GList* self);
void xfmpc_dbbrowser_set_wdir (XfmpcDbbrowser* self, const char* dir);
void xfmpc_dbbrowser_reload (XfmpcDbbrowser* self);
GType xfmpc_extended_interface_extended_interface_widget_get_type (void);
void xfmpc_extended_interface_set_active (XfmpcExtendedInterface* self, XfmpcExtendedInterfaceExtendedInterfaceWidget active_widget);
static void xfmpc_playlist_cb_browse_selection (XfmpcPlaylist* self);
XfmpcSongDialog* xfmpc_song_dialog_new (gint song_id);
XfmpcSongDialog* xfmpc_song_dialog_construct (GType object_type, gint song_id);
GType xfmpc_song_dialog_get_type (void);
static void xfmpc_playlist_cb_info_selection (XfmpcPlaylist* self);
static void xfmpc_playlist_menu_popup (XfmpcPlaylist* self);
static gboolean xfmpc_playlist_cb_popup_menu (XfmpcPlaylist* self);
void xfmpc_playlist_refresh_current_song (XfmpcPlaylist* self);
void xfmpc_playlist_select_row (XfmpcPlaylist* self, gint i);
static void xfmpc_playlist_cb_song_changed (XfmpcPlaylist* self);
void xfmpc_playlist_append (XfmpcPlaylist* self, gint id, gint pos, const char* filename, const char* song, const char* length);
static void xfmpc_playlist_cb_playlist_changed (XfmpcPlaylist* self);
static void xfmpc_playlist_cb_row_activated (XfmpcPlaylist* self, const GtkTreePath* path, GtkTreeViewColumn* column);
void xfmpc_playlist_delete_selection (XfmpcPlaylist* self);
static gboolean xfmpc_playlist_cb_key_released (XfmpcPlaylist* self, const GdkEventKey* event);
static gboolean xfmpc_playlist_cb_button_released (XfmpcPlaylist* self, const GdkEventButton* event);
static void xfmpc_playlist_cb_filter_entry_activated (XfmpcPlaylist* self);
static gboolean xfmpc_playlist_cb_filter_entry_key_released (XfmpcPlaylist* self, const GdkEventKey* event);
static void xfmpc_playlist_cb_filter_entry_changed (XfmpcPlaylist* self);
gboolean xfmpc_playlist_has_filename (XfmpcPlaylist* self, const char* filename, gboolean is_dir);
XfmpcPlaylist* xfmpc_playlist_new (void);
XfmpcPlaylist* xfmpc_playlist_construct (GType object_type);
XfmpcPlaylist* xfmpc_playlist_new (void);
XfmpcPreferences* xfmpc_preferences_get (void);
gboolean xfmpc_preferences_get_playlist_autocenter (XfmpcPreferences* self);
static void _xfmpc_playlist_delete_selection_gtk_menu_item_activate (GtkImageMenuItem* _sender, gpointer self);
static void _xfmpc_playlist_cb_browse_selection_gtk_menu_item_activate (GtkImageMenuItem* _sender, gpointer self);
static void _xfmpc_playlist_cb_info_selection_gtk_menu_item_activate (GtkImageMenuItem* _sender, gpointer self);
static void _xfmpc_playlist_cb_song_changed_xfmpc_mpdclient_song_changed (XfmpcMpdclient* _sender, gpointer self);
static void _xfmpc_playlist_cb_playlist_changed_xfmpc_mpdclient_playlist_changed (XfmpcMpdclient* _sender, gpointer self);
static void _xfmpc_playlist_cb_row_activated_gtk_tree_view_row_activated (GtkTreeView* _sender, const GtkTreePath* path, GtkTreeViewColumn* column, gpointer self);
static gboolean _xfmpc_playlist_cb_key_released_gtk_widget_key_release_event (GtkTreeView* _sender, const GdkEventKey* event, gpointer self);
static gboolean _xfmpc_playlist_cb_button_released_gtk_widget_button_press_event (GtkTreeView* _sender, const GdkEventButton* event, gpointer self);
static gboolean _xfmpc_playlist_cb_popup_menu_gtk_widget_popup_menu (GtkTreeView* _sender, gpointer self);
static void _xfmpc_playlist_cb_filter_entry_activated_gtk_entry_activate (GtkEntry* _sender, gpointer self);
static gboolean _xfmpc_playlist_cb_filter_entry_key_released_gtk_widget_key_release_event (GtkEntry* _sender, const GdkEventKey* event, gpointer self);
static void _xfmpc_playlist_cb_filter_entry_changed_gtk_editable_changed (GtkEntry* _sender, gpointer self);
static void _xfmpc_playlist_cb_playlist_changed_g_object_notify (XfmpcPreferences* _sender, GParamSpec* pspec, gpointer self);
static GObject * xfmpc_playlist_constructor (GType type, guint n_construct_properties, GObjectConstructParam * construct_properties);
static gpointer xfmpc_playlist_parent_class = NULL;
static void xfmpc_playlist_finalize (GObject* obj);
static int _vala_strcmp0 (const char * str1, const char * str2);




static GType xfmpc_playlist_columns_get_type (void) {
	static GType xfmpc_playlist_columns_type_id = 0;
	if (G_UNLIKELY (xfmpc_playlist_columns_type_id == 0)) {
		static const GEnumValue values[] = {{XFMPC_PLAYLIST_COLUMNS_COLUMN_ID, "XFMPC_PLAYLIST_COLUMNS_COLUMN_ID", "column-id"}, {XFMPC_PLAYLIST_COLUMNS_COLUMN_FILENAME, "XFMPC_PLAYLIST_COLUMNS_COLUMN_FILENAME", "column-filename"}, {XFMPC_PLAYLIST_COLUMNS_COLUMN_POSITION, "XFMPC_PLAYLIST_COLUMNS_COLUMN_POSITION", "column-position"}, {XFMPC_PLAYLIST_COLUMNS_COLUMN_SONG, "XFMPC_PLAYLIST_COLUMNS_COLUMN_SONG", "column-song"}, {XFMPC_PLAYLIST_COLUMNS_COLUMN_LENGTH, "XFMPC_PLAYLIST_COLUMNS_COLUMN_LENGTH", "column-length"}, {XFMPC_PLAYLIST_COLUMNS_COLUMN_WEIGHT, "XFMPC_PLAYLIST_COLUMNS_COLUMN_WEIGHT", "column-weight"}, {XFMPC_PLAYLIST_COLUMNS_N_COLUMNS, "XFMPC_PLAYLIST_COLUMNS_N_COLUMNS", "n-columns"}, {0, NULL, NULL}};
		xfmpc_playlist_columns_type_id = g_enum_register_static ("XfmpcPlaylistColumns", values);
	}
	return xfmpc_playlist_columns_type_id;
}


static gboolean xfmpc_playlist_visible_func_filter_tree (GtkTreeModel* model, GtkTreeIter* iter) {
	char* song;
	char* search;
	gboolean _result_;
	char* _tmp2_;
	const char* _tmp1_;
	char* _tmp3_;
	gboolean _tmp5_;
	g_return_val_if_fail (model != NULL, FALSE);
	song = g_strdup ("");
	search = g_strdup ("");
	_result_ = TRUE;
	gtk_tree_model_get (model, &(*iter), XFMPC_PLAYLIST_COLUMNS_COLUMN_SONG, &song, -1, -1);
	if (_vala_strcmp0 (song, "") == 0) {
		gboolean _tmp0_;
		return (_tmp0_ = TRUE, song = (g_free (song), NULL), search = (g_free (search), NULL), _tmp0_);
	}
	_tmp2_ = NULL;
	_tmp1_ = NULL;
	search = (_tmp2_ = (_tmp1_ = gtk_entry_get_text (xfmpc_playlist_filter_entry), (_tmp1_ == NULL) ? NULL : g_strdup (_tmp1_)), search = (g_free (search), NULL), _tmp2_);
	_tmp3_ = NULL;
	search = (_tmp3_ = g_utf8_casefold (search, (glong) (-1)), search = (g_free (search), NULL), _tmp3_);
	if (_vala_strcmp0 (search, "") != 0) {
		char* _tmp4_;
		_tmp4_ = NULL;
		song = (_tmp4_ = g_utf8_casefold (song, (glong) (-1)), song = (g_free (song), NULL), _tmp4_);
		if (strstr (song, search) == NULL) {
			_result_ = FALSE;
		}
	}
	return (_tmp5_ = _result_, song = (g_free (song), NULL), search = (g_free (search), NULL), _tmp5_);
}


static void _g_list_free_gtk_tree_path_free (GList* self) {
	g_list_foreach (self, (GFunc) gtk_tree_path_free, NULL);
	g_list_free (self);
}


/*
 * Signal callbacks
 */
static void xfmpc_playlist_cb_browse_selection (XfmpcPlaylist* self) {
	XfmpcDbbrowser* dbbrowser;
	XfmpcExtendedInterface* extended_interface;
	GtkTreeSelection* _tmp0_;
	GtkTreeSelection* selection;
	GtkListStore* _tmp1_;
	GtkListStore* model;
	GtkListStore* _tmp5_;
	GtkListStore* _tmp4_;
	GList* _tmp3_;
	GtkTreeModel* _tmp2_;
	GList* list;
	GtkTreeIter iter = {0};
	const GtkTreePath* _tmp6_;
	GtkTreePath* path;
	g_return_if_fail (self != NULL);
	dbbrowser = NULL;
	extended_interface = NULL;
	dbbrowser = XFMPC_DBBROWSER (g_object_get_data ((GObject*) self, "XfmpcDbbrowser"));
	extended_interface = XFMPC_EXTENDED_INTERFACE (g_object_get_data ((GObject*) self, "XfmpcExtendedInterface"));
	_tmp0_ = NULL;
	selection = (_tmp0_ = gtk_tree_view_get_selection (self->priv->treeview), (_tmp0_ == NULL) ? NULL : g_object_ref (_tmp0_));
	if (gtk_tree_selection_count_selected_rows (selection) > 1) {
		(selection == NULL) ? NULL : (selection = (g_object_unref (selection), NULL));
		return;
	}
	_tmp1_ = NULL;
	model = (_tmp1_ = self->priv->store, (_tmp1_ == NULL) ? NULL : g_object_ref (_tmp1_));
	_tmp5_ = NULL;
	_tmp4_ = NULL;
	_tmp3_ = NULL;
	_tmp2_ = NULL;
	list = (_tmp3_ = gtk_tree_selection_get_selected_rows (selection, &_tmp2_), model = (_tmp4_ = (_tmp5_ = (GtkListStore*) _tmp2_, (_tmp5_ == NULL) ? NULL : g_object_ref (_tmp5_)), (model == NULL) ? NULL : (model = (g_object_unref (model), NULL)), _tmp4_), _tmp3_);
	if (g_list_length (list) == 0) {
		(selection == NULL) ? NULL : (selection = (g_object_unref (selection), NULL));
		(model == NULL) ? NULL : (model = (g_object_unref (model), NULL));
		(list == NULL) ? NULL : (list = (_g_list_free_gtk_tree_path_free (list), NULL));
		return;
	}
	_tmp6_ = NULL;
	path = (_tmp6_ = (const GtkTreePath*) g_list_nth_data (list, (guint) 0), (_tmp6_ == NULL) ? NULL : gtk_tree_path_copy (_tmp6_));
	if (gtk_tree_model_get_iter ((GtkTreeModel*) self->priv->store, &iter, path)) {
		char* filename;
		char* dir;
		char* _tmp7_;
		filename = g_strdup ("");
		dir = NULL;
		gtk_tree_model_get ((GtkTreeModel*) self->priv->store, &iter, XFMPC_PLAYLIST_COLUMNS_COLUMN_FILENAME, &filename, -1, -1);
		_tmp7_ = NULL;
		dir = (_tmp7_ = g_path_get_dirname (filename), dir = (g_free (dir), NULL), _tmp7_);
		xfmpc_dbbrowser_set_wdir (dbbrowser, dir);
		xfmpc_dbbrowser_reload (dbbrowser);
		xfmpc_extended_interface_set_active (extended_interface, XFMPC_EXTENDED_INTERFACE_EXTENDED_INTERFACE_WIDGET_DBBROWSER);
		filename = (g_free (filename), NULL);
		dir = (g_free (dir), NULL);
	}
	(selection == NULL) ? NULL : (selection = (g_object_unref (selection), NULL));
	(model == NULL) ? NULL : (model = (g_object_unref (model), NULL));
	(list == NULL) ? NULL : (list = (_g_list_free_gtk_tree_path_free (list), NULL));
	(path == NULL) ? NULL : (path = (gtk_tree_path_free (path), NULL));
}


static void xfmpc_playlist_cb_info_selection (XfmpcPlaylist* self) {
	GtkTreeIter iter = {0};
	gint id;
	GtkTreeSelection* _tmp0_;
	GtkTreeSelection* selection;
	GtkListStore* _tmp1_;
	GtkListStore* model;
	GtkListStore* _tmp5_;
	GtkListStore* _tmp4_;
	GList* _tmp3_;
	GtkTreeModel* _tmp2_;
	GList* list;
	const GtkTreePath* _tmp6_;
	GtkTreePath* path;
	g_return_if_fail (self != NULL);
	id = 0;
	_tmp0_ = NULL;
	selection = (_tmp0_ = gtk_tree_view_get_selection (self->priv->treeview), (_tmp0_ == NULL) ? NULL : g_object_ref (_tmp0_));
	if (gtk_tree_selection_count_selected_rows (selection) > 1) {
		(selection == NULL) ? NULL : (selection = (g_object_unref (selection), NULL));
		return;
	}
	_tmp1_ = NULL;
	model = (_tmp1_ = self->priv->store, (_tmp1_ == NULL) ? NULL : g_object_ref (_tmp1_));
	_tmp5_ = NULL;
	_tmp4_ = NULL;
	_tmp3_ = NULL;
	_tmp2_ = NULL;
	list = (_tmp3_ = gtk_tree_selection_get_selected_rows (selection, &_tmp2_), model = (_tmp4_ = (_tmp5_ = (GtkListStore*) _tmp2_, (_tmp5_ == NULL) ? NULL : g_object_ref (_tmp5_)), (model == NULL) ? NULL : (model = (g_object_unref (model), NULL)), _tmp4_), _tmp3_);
	if (g_list_length (list) == 0) {
		(selection == NULL) ? NULL : (selection = (g_object_unref (selection), NULL));
		(model == NULL) ? NULL : (model = (g_object_unref (model), NULL));
		(list == NULL) ? NULL : (list = (_g_list_free_gtk_tree_path_free (list), NULL));
		return;
	}
	_tmp6_ = NULL;
	path = (_tmp6_ = (const GtkTreePath*) g_list_nth_data (list, (guint) 0), (_tmp6_ == NULL) ? NULL : gtk_tree_path_copy (_tmp6_));
	if (gtk_tree_model_get_iter ((GtkTreeModel*) self->priv->store, &iter, path)) {
		XfmpcSongDialog* dialog;
		gtk_tree_model_get ((GtkTreeModel*) self->priv->store, &iter, XFMPC_PLAYLIST_COLUMNS_COLUMN_ID, &id, -1, -1);
		dialog = g_object_ref_sink (xfmpc_song_dialog_new (id));
		gtk_widget_show_all ((GtkWidget*) dialog);
		(dialog == NULL) ? NULL : (dialog = (g_object_unref (dialog), NULL));
	}
	(selection == NULL) ? NULL : (selection = (g_object_unref (selection), NULL));
	(model == NULL) ? NULL : (model = (g_object_unref (model), NULL));
	(list == NULL) ? NULL : (list = (_g_list_free_gtk_tree_path_free (list), NULL));
	(path == NULL) ? NULL : (path = (gtk_tree_path_free (path), NULL));
}


static gboolean xfmpc_playlist_cb_popup_menu (XfmpcPlaylist* self) {
	g_return_val_if_fail (self != NULL, FALSE);
	xfmpc_playlist_menu_popup (self);
	return TRUE;
}


static void xfmpc_playlist_cb_song_changed (XfmpcPlaylist* self) {
	gboolean _tmp0_;
	g_return_if_fail (self != NULL);
	xfmpc_playlist_refresh_current_song (self);
	_tmp0_ = FALSE;
	if (_vala_strcmp0 (gtk_entry_get_text (xfmpc_playlist_filter_entry), "") == 0) {
		_tmp0_ = self->priv->autocenter;
	} else {
		_tmp0_ = FALSE;
	}
	if (_tmp0_) {
		xfmpc_playlist_select_row (self, self->priv->current);
	}
}


static void xfmpc_playlist_cb_playlist_changed (XfmpcPlaylist* self) {
	char* filename;
	char* song;
	char* length;
	gint id;
	gint pos;
	gboolean _tmp0_;
	g_return_if_fail (self != NULL);
	filename = g_strdup ("");
	song = g_strdup ("");
	length = g_strdup ("");
	id = 0;
	pos = 0;
	self->priv->current = xfmpc_mpdclient_get_id (self->priv->mpdclient);
	gtk_list_store_clear (self->priv->store);
	while (xfmpc_mpdclient_playlist_read (self->priv->mpdclient, &id, &pos, &filename, &song, &length)) {
		xfmpc_playlist_append (self, id, pos, filename, song, length);
	}
	xfmpc_playlist_refresh_current_song (self);
	_tmp0_ = FALSE;
	if (_vala_strcmp0 (gtk_entry_get_text (xfmpc_playlist_filter_entry), "") != 0) {
		_tmp0_ = self->priv->autocenter;
	} else {
		_tmp0_ = FALSE;
	}
	if (_tmp0_) {
		xfmpc_playlist_select_row (self, self->priv->current);
	}
	filename = (g_free (filename), NULL);
	song = (g_free (song), NULL);
	length = (g_free (length), NULL);
}


static void xfmpc_playlist_cb_row_activated (XfmpcPlaylist* self, const GtkTreePath* path, GtkTreeViewColumn* column) {
	GtkTreeIter iter = {0};
	const GtkTreePath* _tmp0_;
	GtkTreePath* new_path;
	gint id;
	g_return_if_fail (self != NULL);
	g_return_if_fail (path != NULL);
	g_return_if_fail (column != NULL);
	_tmp0_ = NULL;
	new_path = (_tmp0_ = path, (_tmp0_ == NULL) ? NULL : gtk_tree_path_copy (_tmp0_));
	id = 0;
	if (!gtk_tree_model_get_iter ((GtkTreeModel*) self->priv->filter, &iter, new_path)) {
		(new_path == NULL) ? NULL : (new_path = (gtk_tree_path_free (new_path), NULL));
		return;
	}
	gtk_tree_model_get ((GtkTreeModel*) self->priv->filter, &iter, XFMPC_PLAYLIST_COLUMNS_COLUMN_ID, &id, -1, -1);
	xfmpc_mpdclient_set_id (self->priv->mpdclient, id);
	(new_path == NULL) ? NULL : (new_path = (gtk_tree_path_free (new_path), NULL));
}


static gboolean xfmpc_playlist_cb_key_released (XfmpcPlaylist* self, const GdkEventKey* event) {
	g_return_val_if_fail (self != NULL, FALSE);
	if ((*event).type != GDK_KEY_RELEASE) {
		return FALSE;
	}
	/* Suppr key */
	switch ((*event).keyval) {
		case 0xffff:
		{
			xfmpc_playlist_delete_selection (self);
			break;
		}
		default:
		{
			return FALSE;
		}
	}
	return TRUE;
}


static gboolean xfmpc_playlist_cb_button_released (XfmpcPlaylist* self, const GdkEventButton* event) {
	GtkTreePath* path;
	GtkTreeSelection* selection;
	gboolean _tmp0_;
	GtkTreeSelection* _tmp3_;
	GtkTreeSelection* _tmp2_;
	gboolean sensitive;
	GtkTreePath* _tmp7_;
	gboolean _tmp6_;
	GtkTreePath* _tmp5_;
	gboolean _tmp8_;
	g_return_val_if_fail (self != NULL, FALSE);
	path = NULL;
	selection = NULL;
	_tmp0_ = FALSE;
	if ((*event).type != GDK_BUTTON_PRESS) {
		_tmp0_ = TRUE;
	} else {
		_tmp0_ = (*event).button != 3;
	}
	if (_tmp0_) {
		gboolean _tmp1_;
		return (_tmp1_ = FALSE, (path == NULL) ? NULL : (path = (gtk_tree_path_free (path), NULL)), (selection == NULL) ? NULL : (selection = (g_object_unref (selection), NULL)), _tmp1_);
	}
	_tmp3_ = NULL;
	_tmp2_ = NULL;
	selection = (_tmp3_ = (_tmp2_ = gtk_tree_view_get_selection (self->priv->treeview), (_tmp2_ == NULL) ? NULL : g_object_ref (_tmp2_)), (selection == NULL) ? NULL : (selection = (g_object_unref (selection), NULL)), _tmp3_);
	if (gtk_tree_selection_count_selected_rows (selection) < 1) {
		gboolean _tmp4_;
		return (_tmp4_ = TRUE, (path == NULL) ? NULL : (path = (gtk_tree_path_free (path), NULL)), (selection == NULL) ? NULL : (selection = (g_object_unref (selection), NULL)), _tmp4_);
	}
	sensitive = gtk_tree_selection_count_selected_rows (selection) == 1;
	gtk_widget_set_sensitive ((GtkWidget*) self->priv->mi_browse, sensitive);
	gtk_widget_set_sensitive ((GtkWidget*) self->priv->mi_information, sensitive);
	_tmp7_ = NULL;
	_tmp5_ = NULL;
	if ((_tmp6_ = gtk_tree_view_get_path_at_pos (self->priv->treeview, (gint) (*event).x, (gint) (*event).y, &_tmp5_, NULL, NULL, NULL), path = (_tmp7_ = _tmp5_, (path == NULL) ? NULL : (path = (gtk_tree_path_free (path), NULL)), _tmp7_), _tmp6_)) {
		if (!gtk_tree_selection_path_is_selected (selection, path)) {
			gtk_tree_selection_unselect_all (selection);
			gtk_tree_selection_select_path (selection, path);
		}
	}
	xfmpc_playlist_menu_popup (self);
	return (_tmp8_ = TRUE, (path == NULL) ? NULL : (path = (gtk_tree_path_free (path), NULL)), (selection == NULL) ? NULL : (selection = (g_object_unref (selection), NULL)), _tmp8_);
}


static void xfmpc_playlist_menu_popup (XfmpcPlaylist* self) {
	g_return_if_fail (self != NULL);
	gtk_menu_popup (self->priv->menu, NULL, NULL, NULL, NULL, (guint) 0, gtk_get_current_event_time ());
}


static void xfmpc_playlist_cb_filter_entry_activated (XfmpcPlaylist* self) {
	GtkTreeModelFilter* _tmp3_;
	GtkTreeModelFilter* _tmp2_;
	GList* _tmp1_;
	GtkTreeModel* _tmp0_;
	GList* list;
	const GtkTreePath* _tmp4_;
	GtkTreePath* path;
	g_return_if_fail (self != NULL);
	_tmp3_ = NULL;
	_tmp2_ = NULL;
	_tmp1_ = NULL;
	_tmp0_ = NULL;
	list = (_tmp1_ = gtk_tree_selection_get_selected_rows (gtk_tree_view_get_selection (self->priv->treeview), &_tmp0_), self->priv->filter = (_tmp2_ = (_tmp3_ = (GtkTreeModelFilter*) _tmp0_, (_tmp3_ == NULL) ? NULL : g_object_ref (_tmp3_)), (self->priv->filter == NULL) ? NULL : (self->priv->filter = (g_object_unref (self->priv->filter), NULL)), _tmp2_), _tmp1_);
	_tmp4_ = NULL;
	path = (_tmp4_ = (const GtkTreePath*) g_list_nth_data (list, (guint) 0), (_tmp4_ == NULL) ? NULL : gtk_tree_path_copy (_tmp4_));
	if (g_list_length (list) > 0) {
		gtk_tree_view_row_activated (self->priv->treeview, path, gtk_tree_view_get_column (self->priv->treeview, 0));
		gtk_entry_set_text (xfmpc_playlist_filter_entry, "");
		xfmpc_playlist_select_row (self, self->priv->current);
		gtk_widget_grab_focus ((GtkWidget*) self->priv->treeview);
	}
	(list == NULL) ? NULL : (list = (_g_list_free_gtk_tree_path_free (list), NULL));
	(path == NULL) ? NULL : (path = (gtk_tree_path_free (path), NULL));
}


static gboolean xfmpc_playlist_cb_filter_entry_key_released (XfmpcPlaylist* self, const GdkEventKey* event) {
	g_return_val_if_fail (self != NULL, FALSE);
	if ((*event).type != GDK_KEY_RELEASE) {
		return FALSE;
	}
	/* Escape */
	if ((*event).keyval == 0xff1b) {
		gtk_entry_set_text (xfmpc_playlist_filter_entry, "");
		xfmpc_playlist_select_row (self, self->priv->current);
		gtk_widget_grab_focus ((GtkWidget*) self->priv->treeview);
	} else {
		if (_vala_strcmp0 (gtk_entry_get_text (xfmpc_playlist_filter_entry), "") != 0) {
			xfmpc_playlist_select_row (self, 0);
		} else {
			xfmpc_playlist_select_row (self, self->priv->current);
		}
	}
	return TRUE;
}


static void xfmpc_playlist_cb_filter_entry_changed (XfmpcPlaylist* self) {
	g_return_if_fail (self != NULL);
	gtk_tree_model_filter_refilter (self->priv->filter);
}


/*
 * Public
 */
void xfmpc_playlist_append (XfmpcPlaylist* self, gint id, gint pos, const char* filename, const char* song, const char* length) {
	GtkTreeIter iter = {0};
	g_return_if_fail (self != NULL);
	g_return_if_fail (filename != NULL);
	g_return_if_fail (song != NULL);
	g_return_if_fail (length != NULL);
	gtk_list_store_append (self->priv->store, &iter);
	gtk_list_store_set (self->priv->store, &iter, XFMPC_PLAYLIST_COLUMNS_COLUMN_ID, id, XFMPC_PLAYLIST_COLUMNS_COLUMN_FILENAME, filename, XFMPC_PLAYLIST_COLUMNS_COLUMN_POSITION, pos + 1, XFMPC_PLAYLIST_COLUMNS_COLUMN_SONG, song, XFMPC_PLAYLIST_COLUMNS_COLUMN_LENGTH, length, XFMPC_PLAYLIST_COLUMNS_COLUMN_WEIGHT, PANGO_WEIGHT_NORMAL, -1, -1);
}


void xfmpc_playlist_refresh_current_song (XfmpcPlaylist* self) {
	GtkTreeIter iter = {0};
	GtkTreePath* path;
	GtkTreePath* _tmp0_;
	GtkTreePath* _tmp1_;
	g_return_if_fail (self != NULL);
	path = NULL;
	if (self->priv->current < 0) {
		self->priv->current = 0;
	}
	_tmp0_ = NULL;
	path = (_tmp0_ = gtk_tree_path_new_from_indices (self->priv->current, -1, -1), (path == NULL) ? NULL : (path = (gtk_tree_path_free (path), NULL)), _tmp0_);
	if (gtk_tree_model_get_iter ((GtkTreeModel*) self->priv->store, &iter, path)) {
		gtk_list_store_set (self->priv->store, &iter, XFMPC_PLAYLIST_COLUMNS_COLUMN_WEIGHT, PANGO_WEIGHT_NORMAL, -1, -1);
	}
	self->priv->current = xfmpc_mpdclient_get_pos (self->priv->mpdclient);
	_tmp1_ = NULL;
	path = (_tmp1_ = gtk_tree_path_new_from_indices (self->priv->current, -1, -1), (path == NULL) ? NULL : (path = (gtk_tree_path_free (path), NULL)), _tmp1_);
	if (gtk_tree_model_get_iter ((GtkTreeModel*) self->priv->store, &iter, path)) {
		gtk_list_store_set (self->priv->store, &iter, XFMPC_PLAYLIST_COLUMNS_COLUMN_WEIGHT, PANGO_WEIGHT_BOLD, -1, -1);
	}
	(path == NULL) ? NULL : (path = (gtk_tree_path_free (path), NULL));
}


void xfmpc_playlist_select_row (XfmpcPlaylist* self, gint i) {
	GtkTreePath* path;
	g_return_if_fail (self != NULL);
	if (self->priv->current < 0) {
		return;
	}
	if (gtk_tree_model_iter_n_children ((GtkTreeModel*) self->priv->filter, NULL) == 0) {
		return;
	}
	path = gtk_tree_path_new_from_indices (i, -1, -1);
	gtk_tree_view_set_cursor (self->priv->treeview, path, NULL, FALSE);
	gtk_tree_view_scroll_to_cell (self->priv->treeview, path, NULL, TRUE, (float) 0.42, (float) 0);
	(path == NULL) ? NULL : (path = (gtk_tree_path_free (path), NULL));
}


void xfmpc_playlist_delete_selection (XfmpcPlaylist* self) {
	gint id;
	GtkTreeIter iter = {0};
	GtkListStore* _tmp0_;
	GtkListStore* model;
	GtkListStore* _tmp4_;
	GtkListStore* _tmp3_;
	GList* _tmp2_;
	GtkTreeModel* _tmp1_;
	GList* list;
	g_return_if_fail (self != NULL);
	id = 0;
	_tmp0_ = NULL;
	model = (_tmp0_ = self->priv->store, (_tmp0_ == NULL) ? NULL : g_object_ref (_tmp0_));
	_tmp4_ = NULL;
	_tmp3_ = NULL;
	_tmp2_ = NULL;
	_tmp1_ = NULL;
	list = (_tmp2_ = gtk_tree_selection_get_selected_rows (gtk_tree_view_get_selection (self->priv->treeview), &_tmp1_), model = (_tmp3_ = (_tmp4_ = (GtkListStore*) _tmp1_, (_tmp4_ == NULL) ? NULL : g_object_ref (_tmp4_)), (model == NULL) ? NULL : (model = (g_object_unref (model), NULL)), _tmp3_), _tmp2_);
	{
		GList* path_collection;
		GList* path_it;
		path_collection = list;
		for (path_it = path_collection; path_it != NULL; path_it = path_it->next) {
			const GtkTreePath* _tmp5_;
			GtkTreePath* path;
			_tmp5_ = NULL;
			path = (_tmp5_ = (const GtkTreePath*) path_it->data, (_tmp5_ == NULL) ? NULL : gtk_tree_path_copy (_tmp5_));
			{
				if (gtk_tree_model_get_iter ((GtkTreeModel*) self->priv->store, &iter, path)) {
					gtk_tree_model_get ((GtkTreeModel*) self->priv->store, &iter, XFMPC_PLAYLIST_COLUMNS_COLUMN_ID, &id, -1, -1);
					xfmpc_mpdclient_queue_remove_id (self->priv->mpdclient, id);
				}
				(path == NULL) ? NULL : (path = (gtk_tree_path_free (path), NULL));
			}
		}
	}
	xfmpc_mpdclient_queue_commit (self->priv->mpdclient);
	(model == NULL) ? NULL : (model = (g_object_unref (model), NULL));
	(list == NULL) ? NULL : (list = (_g_list_free_gtk_tree_path_free (list), NULL));
}


gboolean xfmpc_playlist_has_filename (XfmpcPlaylist* self, const char* filename, gboolean is_dir) {
	GtkTreeIter iter = {0};
	GtkTreePath* path;
	char* name;
	GtkTreePath* _tmp0_;
	gboolean _tmp3_;
	g_return_val_if_fail (self != NULL, FALSE);
	g_return_val_if_fail (filename != NULL, FALSE);
	path = NULL;
	name = g_strdup ("");
	_tmp0_ = NULL;
	path = (_tmp0_ = gtk_tree_path_new_from_indices (0, -1, -1), (path == NULL) ? NULL : (path = (gtk_tree_path_free (path), NULL)), _tmp0_);
	while (gtk_tree_model_get_iter ((GtkTreeModel*) self->priv->store, &iter, path)) {
		gtk_tree_model_get ((GtkTreeModel*) self->priv->store, &iter, XFMPC_PLAYLIST_COLUMNS_COLUMN_FILENAME, &name, -1, -1);
		if (is_dir) {
			if (g_str_has_prefix (name, filename)) {
				gboolean _tmp1_;
				return (_tmp1_ = TRUE, (path == NULL) ? NULL : (path = (gtk_tree_path_free (path), NULL)), name = (g_free (name), NULL), _tmp1_);
			}
		} else {
			if (_vala_strcmp0 (name, filename) == 0) {
				gboolean _tmp2_;
				return (_tmp2_ = TRUE, (path == NULL) ? NULL : (path = (gtk_tree_path_free (path), NULL)), name = (g_free (name), NULL), _tmp2_);
			}
		}
		gtk_tree_path_next (path);
	}
	return (_tmp3_ = FALSE, (path == NULL) ? NULL : (path = (gtk_tree_path_free (path), NULL)), name = (g_free (name), NULL), _tmp3_);
}


XfmpcPlaylist* xfmpc_playlist_construct (GType object_type) {
	XfmpcPlaylist * self;
	self = g_object_newv (object_type, 0, NULL);
	return self;
}


XfmpcPlaylist* xfmpc_playlist_new (void) {
	return xfmpc_playlist_construct (XFMPC_TYPE_PLAYLIST);
}


static void _xfmpc_playlist_delete_selection_gtk_menu_item_activate (GtkImageMenuItem* _sender, gpointer self) {
	xfmpc_playlist_delete_selection (self);
}


static void _xfmpc_playlist_cb_browse_selection_gtk_menu_item_activate (GtkImageMenuItem* _sender, gpointer self) {
	xfmpc_playlist_cb_browse_selection (self);
}


static void _xfmpc_playlist_cb_info_selection_gtk_menu_item_activate (GtkImageMenuItem* _sender, gpointer self) {
	xfmpc_playlist_cb_info_selection (self);
}


static void _xfmpc_playlist_cb_song_changed_xfmpc_mpdclient_song_changed (XfmpcMpdclient* _sender, gpointer self) {
	xfmpc_playlist_cb_song_changed (self);
}


static void _xfmpc_playlist_cb_playlist_changed_xfmpc_mpdclient_playlist_changed (XfmpcMpdclient* _sender, gpointer self) {
	xfmpc_playlist_cb_playlist_changed (self);
}


static void _xfmpc_playlist_cb_row_activated_gtk_tree_view_row_activated (GtkTreeView* _sender, const GtkTreePath* path, GtkTreeViewColumn* column, gpointer self) {
	xfmpc_playlist_cb_row_activated (self, path, column);
}


static gboolean _xfmpc_playlist_cb_key_released_gtk_widget_key_release_event (GtkTreeView* _sender, const GdkEventKey* event, gpointer self) {
	return xfmpc_playlist_cb_key_released (self, event);
}


static gboolean _xfmpc_playlist_cb_button_released_gtk_widget_button_press_event (GtkTreeView* _sender, const GdkEventButton* event, gpointer self) {
	return xfmpc_playlist_cb_button_released (self, event);
}


static gboolean _xfmpc_playlist_cb_popup_menu_gtk_widget_popup_menu (GtkTreeView* _sender, gpointer self) {
	return xfmpc_playlist_cb_popup_menu (self);
}


static void _xfmpc_playlist_cb_filter_entry_activated_gtk_entry_activate (GtkEntry* _sender, gpointer self) {
	xfmpc_playlist_cb_filter_entry_activated (self);
}


static gboolean _xfmpc_playlist_cb_filter_entry_key_released_gtk_widget_key_release_event (GtkEntry* _sender, const GdkEventKey* event, gpointer self) {
	return xfmpc_playlist_cb_filter_entry_key_released (self, event);
}


static void _xfmpc_playlist_cb_filter_entry_changed_gtk_editable_changed (GtkEntry* _sender, gpointer self) {
	xfmpc_playlist_cb_filter_entry_changed (self);
}


static void _xfmpc_playlist_cb_playlist_changed_g_object_notify (XfmpcPreferences* _sender, GParamSpec* pspec, gpointer self) {
	xfmpc_playlist_cb_playlist_changed (self);
}


static GObject * xfmpc_playlist_constructor (GType type, guint n_construct_properties, GObjectConstructParam * construct_properties) {
	GObject * obj;
	XfmpcPlaylistClass * klass;
	GObjectClass * parent_class;
	XfmpcPlaylist * self;
	klass = XFMPC_PLAYLIST_CLASS (g_type_class_peek (XFMPC_TYPE_PLAYLIST));
	parent_class = G_OBJECT_CLASS (g_type_class_peek_parent (klass));
	obj = parent_class->constructor (type, n_construct_properties, construct_properties);
	self = XFMPC_PLAYLIST (obj);
	{
		GtkListStore* _tmp0_;
		GtkTreeModelFilter* _tmp1_;
		GtkTreeView* _tmp2_;
		GtkCellRendererText* cell;
		GtkCellRendererText* _tmp3_;
		GtkTreeViewColumn* column;
		GtkCellRendererText* _tmp4_;
		GtkAdjustment* _tmp6_;
		GtkAdjustment* _tmp5_;
		GtkScrolledWindow* _tmp7_;
		GtkScrolledWindow* scrolled;
		GtkMenu* _tmp8_;
		GtkImageMenuItem* mi;
		GtkImageMenuItem* _tmp9_;
		GtkImage* image;
		GtkImageMenuItem* _tmp10_;
		GtkEntry* _tmp11_;
		self->priv->mpdclient = xfmpc_mpdclient_get ();
		self->priv->preferences = xfmpc_preferences_get ();
		self->priv->autocenter = xfmpc_preferences_get_playlist_autocenter (self->priv->preferences);
		_tmp0_ = NULL;
		self->priv->store = (_tmp0_ = gtk_list_store_new ((gint) XFMPC_PLAYLIST_COLUMNS_N_COLUMNS, G_TYPE_INT, G_TYPE_STRING, G_TYPE_INT, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_INT, NULL), (self->priv->store == NULL) ? NULL : (self->priv->store = (g_object_unref (self->priv->store), NULL)), _tmp0_);
		_tmp1_ = NULL;
		self->priv->filter = (_tmp1_ = (GtkTreeModelFilter*) gtk_tree_model_filter_new ((GtkTreeModel*) self->priv->store, NULL), (self->priv->filter == NULL) ? NULL : (self->priv->filter = (g_object_unref (self->priv->filter), NULL)), _tmp1_);
		gtk_tree_model_filter_set_visible_func (self->priv->filter, (GtkTreeModelFilterVisibleFunc) xfmpc_playlist_visible_func_filter_tree, NULL, NULL);
		_tmp2_ = NULL;
		self->priv->treeview = (_tmp2_ = g_object_ref_sink ((GtkTreeView*) gtk_tree_view_new ()), (self->priv->treeview == NULL) ? NULL : (self->priv->treeview = (g_object_unref (self->priv->treeview), NULL)), _tmp2_);
		gtk_tree_selection_set_mode (gtk_tree_view_get_selection (self->priv->treeview), GTK_SELECTION_MULTIPLE);
		gtk_tree_view_set_rubber_banding (self->priv->treeview, TRUE);
		gtk_tree_view_set_enable_search (self->priv->treeview, FALSE);
		gtk_tree_view_set_headers_visible (self->priv->treeview, FALSE);
		gtk_tree_view_set_rules_hint (self->priv->treeview, TRUE);
		gtk_tree_view_set_model (self->priv->treeview, (GtkTreeModel*) self->priv->filter);
		cell = g_object_ref_sink ((GtkCellRendererText*) gtk_cell_renderer_text_new ());
		g_object_set ((GtkCellRenderer*) cell, "xalign", (float) 1, NULL);
		gtk_tree_view_insert_column_with_attributes (self->priv->treeview, -1, "Length", (GtkCellRenderer*) cell, "text", XFMPC_PLAYLIST_COLUMNS_COLUMN_POSITION, "weight", XFMPC_PLAYLIST_COLUMNS_COLUMN_WEIGHT, NULL, NULL);
		_tmp3_ = NULL;
		cell = (_tmp3_ = g_object_ref_sink ((GtkCellRendererText*) gtk_cell_renderer_text_new ()), (cell == NULL) ? NULL : (cell = (g_object_unref (cell), NULL)), _tmp3_);
		g_object_set (cell, "ellipsize", PANGO_ELLIPSIZE_END, NULL);
		column = g_object_ref_sink (gtk_tree_view_column_new_with_attributes ("Song", (GtkCellRenderer*) cell, "text", XFMPC_PLAYLIST_COLUMNS_COLUMN_SONG, "weight", XFMPC_PLAYLIST_COLUMNS_COLUMN_WEIGHT, NULL, NULL));
		gtk_tree_view_column_set_expand (column, TRUE);
		gtk_tree_view_append_column (self->priv->treeview, column);
		_tmp4_ = NULL;
		cell = (_tmp4_ = g_object_ref_sink ((GtkCellRendererText*) gtk_cell_renderer_text_new ()), (cell == NULL) ? NULL : (cell = (g_object_unref (cell), NULL)), _tmp4_);
		g_object_set ((GtkCellRenderer*) cell, "xalign", (float) 1, NULL);
		gtk_tree_view_insert_column_with_attributes (self->priv->treeview, -1, "Length", (GtkCellRenderer*) cell, "text", XFMPC_PLAYLIST_COLUMNS_COLUMN_LENGTH, "weight", XFMPC_PLAYLIST_COLUMNS_COLUMN_WEIGHT, NULL, NULL);
		_tmp6_ = NULL;
		_tmp5_ = NULL;
		_tmp7_ = NULL;
		scrolled = (_tmp7_ = g_object_ref_sink ((GtkScrolledWindow*) gtk_scrolled_window_new (_tmp5_ = g_object_ref_sink ((GtkAdjustment*) gtk_adjustment_new ((double) 0, (double) 0, (double) 0, (double) 0, (double) 0, (double) 0)), _tmp6_ = g_object_ref_sink ((GtkAdjustment*) gtk_adjustment_new ((double) 0, (double) 0, (double) 0, (double) 0, (double) 0, (double) 0)))), (_tmp6_ == NULL) ? NULL : (_tmp6_ = (g_object_unref (_tmp6_), NULL)), (_tmp5_ == NULL) ? NULL : (_tmp5_ = (g_object_unref (_tmp5_), NULL)), _tmp7_);
		gtk_scrolled_window_set_policy (scrolled, GTK_POLICY_AUTOMATIC, GTK_POLICY_ALWAYS);
		_tmp8_ = NULL;
		self->priv->menu = (_tmp8_ = g_object_ref_sink ((GtkMenu*) gtk_menu_new ()), (self->priv->menu == NULL) ? NULL : (self->priv->menu = (g_object_unref (self->priv->menu), NULL)), _tmp8_);
		mi = g_object_ref_sink ((GtkImageMenuItem*) gtk_image_menu_item_new_from_stock (GTK_STOCK_REMOVE, NULL));
		gtk_menu_shell_append ((GtkMenuShell*) self->priv->menu, (GtkWidget*) ((GtkMenuItem*) mi));
		g_signal_connect_object ((GtkMenuItem*) mi, "activate", (GCallback) _xfmpc_playlist_delete_selection_gtk_menu_item_activate, self, 0);
		_tmp9_ = NULL;
		self->priv->mi_browse = (_tmp9_ = g_object_ref_sink ((GtkImageMenuItem*) gtk_image_menu_item_new_with_mnemonic (_ ("Browse"))), (self->priv->mi_browse == NULL) ? NULL : (self->priv->mi_browse = (g_object_unref (self->priv->mi_browse), NULL)), _tmp9_);
		image = g_object_ref_sink ((GtkImage*) gtk_image_new_from_stock (GTK_STOCK_OPEN, GTK_ICON_SIZE_MENU));
		gtk_image_menu_item_set_image (self->priv->mi_browse, (GtkWidget*) image);
		gtk_menu_shell_append ((GtkMenuShell*) self->priv->menu, (GtkWidget*) ((GtkMenuItem*) self->priv->mi_browse));
		g_signal_connect_object ((GtkMenuItem*) self->priv->mi_browse, "activate", (GCallback) _xfmpc_playlist_cb_browse_selection_gtk_menu_item_activate, self, 0);
		_tmp10_ = NULL;
		self->priv->mi_information = (_tmp10_ = g_object_ref_sink ((GtkImageMenuItem*) gtk_image_menu_item_new_from_stock (GTK_STOCK_INFO, NULL)), (self->priv->mi_information == NULL) ? NULL : (self->priv->mi_information = (g_object_unref (self->priv->mi_information), NULL)), _tmp10_);
		gtk_menu_shell_append ((GtkMenuShell*) self->priv->menu, (GtkWidget*) ((GtkMenuItem*) self->priv->mi_information));
		g_signal_connect_object ((GtkMenuItem*) self->priv->mi_information, "activate", (GCallback) _xfmpc_playlist_cb_info_selection_gtk_menu_item_activate, self, 0);
		gtk_widget_show_all ((GtkWidget*) self->priv->menu);
		_tmp11_ = NULL;
		xfmpc_playlist_filter_entry = (_tmp11_ = g_object_ref_sink ((GtkEntry*) gtk_entry_new ()), (xfmpc_playlist_filter_entry == NULL) ? NULL : (xfmpc_playlist_filter_entry = (g_object_unref (xfmpc_playlist_filter_entry), NULL)), _tmp11_);
		gtk_container_add ((GtkContainer*) scrolled, (GtkWidget*) self->priv->treeview);
		gtk_box_pack_start ((GtkBox*) self, (GtkWidget*) scrolled, TRUE, TRUE, (guint) 0);
		gtk_box_pack_start ((GtkBox*) self, (GtkWidget*) xfmpc_playlist_filter_entry, FALSE, FALSE, (guint) 0);
		/* Signals */
		g_signal_connect_object (self->priv->mpdclient, "song-changed", (GCallback) _xfmpc_playlist_cb_song_changed_xfmpc_mpdclient_song_changed, self, 0);
		g_signal_connect_object (self->priv->mpdclient, "playlist-changed", (GCallback) _xfmpc_playlist_cb_playlist_changed_xfmpc_mpdclient_playlist_changed, self, 0);
		g_signal_connect_object (self->priv->treeview, "row-activated", (GCallback) _xfmpc_playlist_cb_row_activated_gtk_tree_view_row_activated, self, 0);
		g_signal_connect_object ((GtkWidget*) self->priv->treeview, "key-release-event", (GCallback) _xfmpc_playlist_cb_key_released_gtk_widget_key_release_event, self, 0);
		g_signal_connect_object ((GtkWidget*) self->priv->treeview, "button-press-event", (GCallback) _xfmpc_playlist_cb_button_released_gtk_widget_button_press_event, self, 0);
		g_signal_connect_object ((GtkWidget*) self->priv->treeview, "popup-menu", (GCallback) _xfmpc_playlist_cb_popup_menu_gtk_widget_popup_menu, self, 0);
		g_signal_connect_object (xfmpc_playlist_filter_entry, "activate", (GCallback) _xfmpc_playlist_cb_filter_entry_activated_gtk_entry_activate, self, 0);
		g_signal_connect_object ((GtkWidget*) xfmpc_playlist_filter_entry, "key-release-event", (GCallback) _xfmpc_playlist_cb_filter_entry_key_released_gtk_widget_key_release_event, self, 0);
		g_signal_connect_object ((GtkEditable*) xfmpc_playlist_filter_entry, "changed", (GCallback) _xfmpc_playlist_cb_filter_entry_changed_gtk_editable_changed, self, 0);
		g_signal_connect_object ((GObject*) self->priv->preferences, "notify::song-format", (GCallback) _xfmpc_playlist_cb_playlist_changed_g_object_notify, self, 0);
		g_signal_connect_object ((GObject*) self->priv->preferences, "notify::song-format-custom", (GCallback) _xfmpc_playlist_cb_playlist_changed_g_object_notify, self, 0);
		(cell == NULL) ? NULL : (cell = (g_object_unref (cell), NULL));
		(column == NULL) ? NULL : (column = (g_object_unref (column), NULL));
		(scrolled == NULL) ? NULL : (scrolled = (g_object_unref (scrolled), NULL));
		(mi == NULL) ? NULL : (mi = (g_object_unref (mi), NULL));
		(image == NULL) ? NULL : (image = (g_object_unref (image), NULL));
	}
	return obj;
}


static void xfmpc_playlist_class_init (XfmpcPlaylistClass * klass) {
	xfmpc_playlist_parent_class = g_type_class_peek_parent (klass);
	g_type_class_add_private (klass, sizeof (XfmpcPlaylistPrivate));
	G_OBJECT_CLASS (klass)->constructor = xfmpc_playlist_constructor;
	G_OBJECT_CLASS (klass)->finalize = xfmpc_playlist_finalize;
}


static void xfmpc_playlist_instance_init (XfmpcPlaylist * self) {
	self->priv = XFMPC_PLAYLIST_GET_PRIVATE (self);
	self->priv->current = 0;
}


static void xfmpc_playlist_finalize (GObject* obj) {
	XfmpcPlaylist * self;
	self = XFMPC_PLAYLIST (obj);
	(self->priv->store == NULL) ? NULL : (self->priv->store = (g_object_unref (self->priv->store), NULL));
	(self->priv->filter == NULL) ? NULL : (self->priv->filter = (g_object_unref (self->priv->filter), NULL));
	(self->priv->treeview == NULL) ? NULL : (self->priv->treeview = (g_object_unref (self->priv->treeview), NULL));
	(self->priv->menu == NULL) ? NULL : (self->priv->menu = (g_object_unref (self->priv->menu), NULL));
	(self->priv->mi_browse == NULL) ? NULL : (self->priv->mi_browse = (g_object_unref (self->priv->mi_browse), NULL));
	(self->priv->mi_information == NULL) ? NULL : (self->priv->mi_information = (g_object_unref (self->priv->mi_information), NULL));
	G_OBJECT_CLASS (xfmpc_playlist_parent_class)->finalize (obj);
}


GType xfmpc_playlist_get_type (void) {
	static GType xfmpc_playlist_type_id = 0;
	if (xfmpc_playlist_type_id == 0) {
		static const GTypeInfo g_define_type_info = { sizeof (XfmpcPlaylistClass), (GBaseInitFunc) NULL, (GBaseFinalizeFunc) NULL, (GClassInitFunc) xfmpc_playlist_class_init, (GClassFinalizeFunc) NULL, NULL, sizeof (XfmpcPlaylist), 0, (GInstanceInitFunc) xfmpc_playlist_instance_init, NULL };
		xfmpc_playlist_type_id = g_type_register_static (GTK_TYPE_VBOX, "XfmpcPlaylist", &g_define_type_info, 0);
	}
	return xfmpc_playlist_type_id;
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




