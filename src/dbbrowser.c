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
#include <float.h>
#include <math.h>
#include <gdk-pixbuf/gdk-pixdata.h>
#include <pango/pango.h>
#include <gdk/gdk.h>
#include <glib/gi18n-lib.h>


#define XFMPC_TYPE_DBBROWSER (xfmpc_dbbrowser_get_type ())
#define XFMPC_DBBROWSER(obj) (G_TYPE_CHECK_INSTANCE_CAST ((obj), XFMPC_TYPE_DBBROWSER, XfmpcDbbrowser))
#define XFMPC_DBBROWSER_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST ((klass), XFMPC_TYPE_DBBROWSER, XfmpcDbbrowserClass))
#define XFMPC_IS_DBBROWSER(obj) (G_TYPE_CHECK_INSTANCE_TYPE ((obj), XFMPC_TYPE_DBBROWSER))
#define XFMPC_IS_DBBROWSER_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), XFMPC_TYPE_DBBROWSER))
#define XFMPC_DBBROWSER_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj), XFMPC_TYPE_DBBROWSER, XfmpcDbbrowserClass))

typedef struct _XfmpcDbbrowser XfmpcDbbrowser;
typedef struct _XfmpcDbbrowserClass XfmpcDbbrowserClass;
typedef struct _XfmpcDbbrowserPrivate XfmpcDbbrowserPrivate;

#define XFMPC_TYPE_PREFERENCES (xfmpc_preferences_get_type ())
#define XFMPC_PREFERENCES(obj) (G_TYPE_CHECK_INSTANCE_CAST ((obj), XFMPC_TYPE_PREFERENCES, XfmpcPreferences))
#define XFMPC_PREFERENCES_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST ((klass), XFMPC_TYPE_PREFERENCES, XfmpcPreferencesClass))
#define XFMPC_IS_PREFERENCES(obj) (G_TYPE_CHECK_INSTANCE_TYPE ((obj), XFMPC_TYPE_PREFERENCES))
#define XFMPC_IS_PREFERENCES_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), XFMPC_TYPE_PREFERENCES))
#define XFMPC_PREFERENCES_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj), XFMPC_TYPE_PREFERENCES, XfmpcPreferencesClass))

typedef struct _XfmpcPreferences XfmpcPreferences;
typedef struct _XfmpcPreferencesClass XfmpcPreferencesClass;

#define XFMPC_DBBROWSER_TYPE_COLUMNS (xfmpc_dbbrowser_columns_get_type ())

#define XFMPC_TYPE_PLAYLIST (xfmpc_playlist_get_type ())
#define XFMPC_PLAYLIST(obj) (G_TYPE_CHECK_INSTANCE_CAST ((obj), XFMPC_TYPE_PLAYLIST, XfmpcPlaylist))
#define XFMPC_PLAYLIST_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST ((klass), XFMPC_TYPE_PLAYLIST, XfmpcPlaylistClass))
#define XFMPC_IS_PLAYLIST(obj) (G_TYPE_CHECK_INSTANCE_TYPE ((obj), XFMPC_TYPE_PLAYLIST))
#define XFMPC_IS_PLAYLIST_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), XFMPC_TYPE_PLAYLIST))
#define XFMPC_PLAYLIST_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj), XFMPC_TYPE_PLAYLIST, XfmpcPlaylistClass))

typedef struct _XfmpcPlaylist XfmpcPlaylist;
typedef struct _XfmpcPlaylistClass XfmpcPlaylistClass;

struct _XfmpcDbbrowser {
	GtkVBox parent_instance;
	XfmpcDbbrowserPrivate * priv;
};

struct _XfmpcDbbrowserClass {
	GtkVBoxClass parent_class;
};

struct _XfmpcDbbrowserPrivate {
	XfmpcMpdclient* mpdclient;
	XfmpcPreferences* preferences;
	GtkListStore* store;
	GtkTreeView* treeview;
	GtkMenu* menu;
	GtkEntry* search_entry;
	GtkImageMenuItem* mi_browse;
	char* wdir;
	char* last_wdir;
	gboolean is_searching;
	guint search_timeout;
};

typedef enum  {
	XFMPC_DBBROWSER_COLUMNS_COLUMN_ID,
	XFMPC_DBBROWSER_COLUMNS_COLUMN_PIXBUF,
	XFMPC_DBBROWSER_COLUMNS_COLUMN_FILENAME,
	XFMPC_DBBROWSER_COLUMNS_COLUMN_BASENAME,
	XFMPC_DBBROWSER_COLUMNS_COLUMN_IS_DIR,
	XFMPC_DBBROWSER_COLUMNS_COLUMN_WEIGHT,
	XFMPC_DBBROWSER_COLUMNS_N_COLUMNS
} XfmpcDbbrowserColumns;


static gpointer xfmpc_dbbrowser_parent_class = NULL;

GType xfmpc_dbbrowser_get_type (void);
GType xfmpc_preferences_get_type (void);
#define XFMPC_DBBROWSER_GET_PRIVATE(o) (G_TYPE_INSTANCE_GET_PRIVATE ((o), XFMPC_TYPE_DBBROWSER, XfmpcDbbrowserPrivate))
enum  {
	XFMPC_DBBROWSER_DUMMY_PROPERTY
};
static GType xfmpc_dbbrowser_columns_get_type (void);
GType xfmpc_playlist_get_type (void);
static void xfmpc_dbbrowser_clear (XfmpcDbbrowser* self);
gboolean xfmpc_dbbrowser_wdir_is_root (XfmpcDbbrowser* self);
char* xfmpc_dbbrowser_get_parent_wdir (XfmpcDbbrowser* self);
void xfmpc_dbbrowser_append (XfmpcDbbrowser* self, const char* filename, const char* basename, gboolean is_dir, gboolean is_bold);
gboolean xfmpc_playlist_has_filename (XfmpcPlaylist* self, const char* filename, gboolean is_dir);
void xfmpc_dbbrowser_reload (XfmpcDbbrowser* self);
void xfmpc_dbbrowser_set_wdir (XfmpcDbbrowser* self, const char* dir);
static void _g_list_free_gtk_tree_path_free (GList* self);
void xfmpc_dbbrowser_add_selected_rows (XfmpcDbbrowser* self);
void xfmpc_dbbrowser_search (XfmpcDbbrowser* self, const char* query);
static void xfmpc_dbbrowser_menu_popup (XfmpcDbbrowser* self);
static void xfmpc_dbbrowser_cb_playlist_changed (XfmpcDbbrowser* self);
static gboolean xfmpc_dbbrowser_cb_popup_menu (XfmpcDbbrowser* self);
static gboolean xfmpc_dbbrowser_cb_key_pressed (XfmpcDbbrowser* self, const GdkEventKey* event);
static gboolean xfmpc_dbbrowser_cb_button_released (XfmpcDbbrowser* self, const GdkEventButton* event);
static void xfmpc_dbbrowser_cb_row_activated (XfmpcDbbrowser* self, const GtkTreePath* path, GtkTreeViewColumn* column);
static void xfmpc_dbbrowser_cb_replace_with_selected_rows (XfmpcDbbrowser* self);
static void xfmpc_dbbrowser_cb_browse (XfmpcDbbrowser* self);
static void xfmpc_dbbrowser_cb_search_entry_activated (XfmpcDbbrowser* self);
static gboolean xfmpc_dbbrowser_cb_search_entry_key_released (XfmpcDbbrowser* self, const GdkEventKey* event);
static gboolean xfmpc_dbbrowser_timeout_search (XfmpcDbbrowser* self);
static gboolean _xfmpc_dbbrowser_timeout_search_gsource_func (gpointer self);
static void xfmpc_dbbrowser_cb_search_entry_changed (XfmpcDbbrowser* self);
XfmpcDbbrowser* xfmpc_dbbrowser_new (void);
XfmpcDbbrowser* xfmpc_dbbrowser_construct (GType object_type);
XfmpcDbbrowser* xfmpc_dbbrowser_new (void);
XfmpcPreferences* xfmpc_preferences_get (void);
const char* xfmpc_preferences_get_dbbrowser_last_path (XfmpcPreferences* self);
static void _xfmpc_dbbrowser_add_selected_rows_gtk_menu_item_activate (GtkMenuItem* _sender, gpointer self);
static void _xfmpc_dbbrowser_cb_replace_with_selected_rows_gtk_menu_item_activate (GtkMenuItem* _sender, gpointer self);
static void _xfmpc_dbbrowser_cb_browse_gtk_menu_item_activate (GtkMenuItem* _sender, gpointer self);
static void _xfmpc_dbbrowser_reload_xfmpc_mpdclient_connected (XfmpcMpdclient* _sender, gpointer self);
static void _xfmpc_dbbrowser_reload_xfmpc_mpdclient_database_changed (XfmpcMpdclient* _sender, gpointer self);
static void _xfmpc_dbbrowser_cb_playlist_changed_xfmpc_mpdclient_playlist_changed (XfmpcMpdclient* _sender, gpointer self);
static void _xfmpc_dbbrowser_cb_row_activated_gtk_tree_view_row_activated (GtkTreeView* _sender, const GtkTreePath* path, GtkTreeViewColumn* column, gpointer self);
static gboolean _xfmpc_dbbrowser_cb_key_pressed_gtk_widget_key_press_event (GtkWidget* _sender, const GdkEventKey* event, gpointer self);
static gboolean _xfmpc_dbbrowser_cb_button_released_gtk_widget_button_press_event (GtkWidget* _sender, const GdkEventButton* event, gpointer self);
static gboolean _xfmpc_dbbrowser_cb_popup_menu_gtk_widget_popup_menu (GtkWidget* _sender, gpointer self);
static void _xfmpc_dbbrowser_cb_search_entry_activated_gtk_entry_activate (GtkEntry* _sender, gpointer self);
static gboolean _xfmpc_dbbrowser_cb_search_entry_key_released_gtk_widget_key_release_event (GtkWidget* _sender, const GdkEventKey* event, gpointer self);
static void _xfmpc_dbbrowser_cb_search_entry_changed_gtk_editable_changed (GtkEditable* _sender, gpointer self);
static void _xfmpc_dbbrowser_reload_g_object_notify (GObject* _sender, GParamSpec* pspec, gpointer self);
static GObject * xfmpc_dbbrowser_constructor (GType type, guint n_construct_properties, GObjectConstructParam * construct_properties);
static void xfmpc_dbbrowser_finalize (GObject* obj);
static int _vala_strcmp0 (const char * str1, const char * str2);




static GType xfmpc_dbbrowser_columns_get_type (void) {
	static GType xfmpc_dbbrowser_columns_type_id = 0;
	if (G_UNLIKELY (xfmpc_dbbrowser_columns_type_id == 0)) {
		static const GEnumValue values[] = {{XFMPC_DBBROWSER_COLUMNS_COLUMN_ID, "XFMPC_DBBROWSER_COLUMNS_COLUMN_ID", "column-id"}, {XFMPC_DBBROWSER_COLUMNS_COLUMN_PIXBUF, "XFMPC_DBBROWSER_COLUMNS_COLUMN_PIXBUF", "column-pixbuf"}, {XFMPC_DBBROWSER_COLUMNS_COLUMN_FILENAME, "XFMPC_DBBROWSER_COLUMNS_COLUMN_FILENAME", "column-filename"}, {XFMPC_DBBROWSER_COLUMNS_COLUMN_BASENAME, "XFMPC_DBBROWSER_COLUMNS_COLUMN_BASENAME", "column-basename"}, {XFMPC_DBBROWSER_COLUMNS_COLUMN_IS_DIR, "XFMPC_DBBROWSER_COLUMNS_COLUMN_IS_DIR", "column-is-dir"}, {XFMPC_DBBROWSER_COLUMNS_COLUMN_WEIGHT, "XFMPC_DBBROWSER_COLUMNS_COLUMN_WEIGHT", "column-weight"}, {XFMPC_DBBROWSER_COLUMNS_N_COLUMNS, "XFMPC_DBBROWSER_COLUMNS_N_COLUMNS", "n-columns"}, {0, NULL, NULL}};
		xfmpc_dbbrowser_columns_type_id = g_enum_register_static ("XfmpcDbbrowserColumns", values);
	}
	return xfmpc_dbbrowser_columns_type_id;
}


void xfmpc_dbbrowser_reload (XfmpcDbbrowser* self) {
	char* filename;
	char* basename;
	gboolean is_dir;
	gboolean is_bold;
	gint i;
	XfmpcPlaylist* _tmp0_;
	XfmpcPlaylist* playlist;
	g_return_if_fail (self != NULL);
	filename = g_strdup ("");
	basename = g_strdup ("");
	is_dir = FALSE;
	is_bold = FALSE;
	i = 0;
	_tmp0_ = NULL;
	playlist = (_tmp0_ = XFMPC_PLAYLIST (g_object_get_data ((GObject*) self, "XfmpcPlaylist")), (_tmp0_ == NULL) ? NULL : g_object_ref (_tmp0_));
	if (self->priv->is_searching) {
		filename = (g_free (filename), NULL);
		basename = (g_free (basename), NULL);
		(playlist == NULL) ? NULL : (playlist = (g_object_unref (playlist), NULL));
		return;
	}
	xfmpc_dbbrowser_clear (self);
	if (!xfmpc_mpdclient_is_connected (self->priv->mpdclient)) {
		filename = (g_free (filename), NULL);
		basename = (g_free (basename), NULL);
		(playlist == NULL) ? NULL : (playlist = (g_object_unref (playlist), NULL));
		return;
	}
	if (!xfmpc_dbbrowser_wdir_is_root (self)) {
		char* _tmp1_;
		_tmp1_ = NULL;
		filename = (_tmp1_ = xfmpc_dbbrowser_get_parent_wdir (self), filename = (g_free (filename), NULL), _tmp1_);
		xfmpc_dbbrowser_append (self, filename, "..", TRUE, FALSE);
		i++;
	}
	while (TRUE) {
		if (!xfmpc_mpdclient_database_read (self->priv->mpdclient, self->priv->wdir, &filename, &basename, &is_dir)) {
			break;
		}
		is_bold = xfmpc_playlist_has_filename (playlist, filename, is_dir);
		xfmpc_dbbrowser_append (self, filename, basename, is_dir, is_bold);
		if (g_utf8_collate (filename, self->priv->last_wdir) == 0) {
			GtkTreePath* path;
			path = gtk_tree_path_new_from_indices (i, -1, -1);
			gtk_tree_view_set_cursor (self->priv->treeview, path, NULL, FALSE);
			gtk_tree_view_scroll_to_cell (self->priv->treeview, path, NULL, TRUE, (float) 0.10, (float) 0);
			i = -1;
			(path == NULL) ? NULL : (path = (gtk_tree_path_free (path), NULL));
		} else {
			i++;
		}
	}
	filename = (g_free (filename), NULL);
	basename = (g_free (basename), NULL);
	(playlist == NULL) ? NULL : (playlist = (g_object_unref (playlist), NULL));
}


gboolean xfmpc_dbbrowser_wdir_is_root (XfmpcDbbrowser* self) {
	gboolean result;
	g_return_val_if_fail (self != NULL, FALSE);
	result = _vala_strcmp0 (self->priv->wdir, "") == 0;
	return result;
}


char* xfmpc_dbbrowser_get_parent_wdir (XfmpcDbbrowser* self) {
	char* result;
	const char* _tmp0_;
	char* filename;
	g_return_val_if_fail (self != NULL, NULL);
	_tmp0_ = NULL;
	filename = (_tmp0_ = strstr (self->priv->wdir, "/"), (_tmp0_ == NULL) ? NULL : g_strdup (_tmp0_));
	if (filename == NULL) {
		result = g_strdup ("");
		filename = (g_free (filename), NULL);
		return result;
	} else {
		result = g_path_get_dirname (self->priv->wdir);
		filename = (g_free (filename), NULL);
		return result;
	}
	filename = (g_free (filename), NULL);
}


void xfmpc_dbbrowser_append (XfmpcDbbrowser* self, const char* filename, const char* basename, gboolean is_dir, gboolean is_bold) {
	GtkTreeIter iter = {0};
	const char* _tmp0_;
	GdkPixbuf* pixbuf;
	PangoWeight _tmp1_;
	g_return_if_fail (self != NULL);
	g_return_if_fail (filename != NULL);
	g_return_if_fail (basename != NULL);
	_tmp0_ = NULL;
	if (is_dir) {
		_tmp0_ = GTK_STOCK_DIRECTORY;
	} else {
		_tmp0_ = GTK_STOCK_FILE;
	}
	pixbuf = gtk_widget_render_icon ((GtkWidget*) self->priv->treeview, _tmp0_, GTK_ICON_SIZE_MENU, NULL);
	gtk_list_store_append (self->priv->store, &iter);
	_tmp1_ = 0;
	if (is_bold) {
		_tmp1_ = PANGO_WEIGHT_BOLD;
	} else {
		_tmp1_ = PANGO_WEIGHT_NORMAL;
	}
	gtk_list_store_set (self->priv->store, &iter, XFMPC_DBBROWSER_COLUMNS_COLUMN_PIXBUF, pixbuf, XFMPC_DBBROWSER_COLUMNS_COLUMN_FILENAME, filename, XFMPC_DBBROWSER_COLUMNS_COLUMN_BASENAME, basename, XFMPC_DBBROWSER_COLUMNS_COLUMN_IS_DIR, is_dir, XFMPC_DBBROWSER_COLUMNS_COLUMN_WEIGHT, _tmp1_, -1, -1);
	(pixbuf == NULL) ? NULL : (pixbuf = (g_object_unref (pixbuf), NULL));
}


void xfmpc_dbbrowser_set_wdir (XfmpcDbbrowser* self, const char* dir) {
	char* _tmp1_;
	const char* _tmp0_;
	char* _tmp3_;
	const char* _tmp2_;
	g_return_if_fail (self != NULL);
	g_return_if_fail (dir != NULL);
	_tmp1_ = NULL;
	_tmp0_ = NULL;
	self->priv->last_wdir = (_tmp1_ = (_tmp0_ = self->priv->wdir, (_tmp0_ == NULL) ? NULL : g_strdup (_tmp0_)), self->priv->last_wdir = (g_free (self->priv->last_wdir), NULL), _tmp1_);
	_tmp3_ = NULL;
	_tmp2_ = NULL;
	self->priv->wdir = (_tmp3_ = (_tmp2_ = dir, (_tmp2_ == NULL) ? NULL : g_strdup (_tmp2_)), self->priv->wdir = (g_free (self->priv->wdir), NULL), _tmp3_);
}


static void _g_list_free_gtk_tree_path_free (GList* self) {
	g_list_foreach (self, (GFunc) gtk_tree_path_free, NULL);
	g_list_free (self);
}


void xfmpc_dbbrowser_add_selected_rows (XfmpcDbbrowser* self) {
	GtkTreeModel* _tmp0_;
	GtkTreeModel* model;
	GtkTreeIter iter = {0};
	char* filename;
	GtkTreeModel* _tmp4_;
	GtkTreeModel* _tmp3_;
	GList* _tmp2_;
	GtkTreeModel* _tmp1_;
	GList* list;
	g_return_if_fail (self != NULL);
	_tmp0_ = NULL;
	model = (_tmp0_ = (GtkTreeModel*) self->priv->store, (_tmp0_ == NULL) ? NULL : g_object_ref (_tmp0_));
	filename = g_strdup ("");
	_tmp4_ = NULL;
	_tmp3_ = NULL;
	_tmp2_ = NULL;
	_tmp1_ = NULL;
	list = (_tmp2_ = gtk_tree_selection_get_selected_rows (gtk_tree_view_get_selection (self->priv->treeview), &_tmp1_), model = (_tmp3_ = (_tmp4_ = _tmp1_, (_tmp4_ == NULL) ? NULL : g_object_ref (_tmp4_)), (model == NULL) ? NULL : (model = (g_object_unref (model), NULL)), _tmp3_), _tmp2_);
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
				if (gtk_tree_model_get_iter (model, &iter, path)) {
					gtk_tree_model_get (model, &iter, XFMPC_DBBROWSER_COLUMNS_COLUMN_FILENAME, &filename, -1, -1);
					xfmpc_mpdclient_queue_add (self->priv->mpdclient, filename);
				}
				(path == NULL) ? NULL : (path = (gtk_tree_path_free (path), NULL));
			}
		}
	}
	xfmpc_mpdclient_queue_commit (self->priv->mpdclient);
	(model == NULL) ? NULL : (model = (g_object_unref (model), NULL));
	filename = (g_free (filename), NULL);
	(list == NULL) ? NULL : (list = (_g_list_free_gtk_tree_path_free (list), NULL));
}


void xfmpc_dbbrowser_search (XfmpcDbbrowser* self, const char* query) {
	char* filename;
	char* basename;
	gboolean is_bold;
	gint i;
	XfmpcPlaylist* _tmp0_;
	XfmpcPlaylist* playlist;
	gboolean no_result_buf;
	gboolean no_result;
	GdkColor color = {0};
	gboolean _tmp1_;
	g_return_if_fail (self != NULL);
	g_return_if_fail (query != NULL);
	filename = g_strdup ("");
	basename = g_strdup ("");
	is_bold = FALSE;
	i = 0;
	_tmp0_ = NULL;
	playlist = (_tmp0_ = XFMPC_PLAYLIST (g_object_get_data ((GObject*) self, "XfmpcPlaylist")), (_tmp0_ == NULL) ? NULL : g_object_ref (_tmp0_));
	if (!xfmpc_mpdclient_is_connected (self->priv->mpdclient)) {
		filename = (g_free (filename), NULL);
		basename = (g_free (basename), NULL);
		(playlist == NULL) ? NULL : (playlist = (g_object_unref (playlist), NULL));
		return;
	}
	self->priv->is_searching = TRUE;
	xfmpc_dbbrowser_clear (self);
	while (TRUE) {
		if (!xfmpc_mpdclient_database_search (self->priv->mpdclient, query, &filename, &basename)) {
			break;
		}
		is_bold = xfmpc_playlist_has_filename (playlist, filename, FALSE);
		xfmpc_dbbrowser_append (self, filename, basename, FALSE, is_bold);
		i++;
	}
	no_result_buf = FALSE;
	no_result = FALSE;
	gdk_color_parse ("white", &color);
	color.red = (guint16) 0xFFFF;
	color.green = (guint16) 0x6666;
	color.blue = (guint16) 0x6666;
	if (i == 0) {
		no_result = TRUE;
	} else {
		if (no_result) {
			no_result = FALSE;
		}
	}
	_tmp1_ = FALSE;
	if (no_result == no_result_buf) {
		_tmp1_ = no_result;
	} else {
		_tmp1_ = FALSE;
	}
	if (_tmp1_) {
		gtk_widget_modify_bg ((GtkWidget*) self->priv->search_entry, GTK_STATE_NORMAL, &color);
		gtk_widget_modify_bg ((GtkWidget*) self->priv->search_entry, GTK_STATE_SELECTED, &color);
	} else {
		gboolean _tmp2_;
		_tmp2_ = FALSE;
		if (no_result == no_result_buf) {
			_tmp2_ = !no_result;
		} else {
			_tmp2_ = FALSE;
		}
		if (_tmp2_) {
			gtk_widget_modify_bg ((GtkWidget*) self->priv->search_entry, GTK_STATE_NORMAL, NULL);
			gtk_widget_modify_bg ((GtkWidget*) self->priv->search_entry, GTK_STATE_SELECTED, NULL);
		}
	}
	if (i == 0) {
		gtk_widget_modify_bg ((GtkWidget*) self->priv->search_entry, GTK_STATE_NORMAL, &color);
		gtk_widget_modify_bg ((GtkWidget*) self->priv->search_entry, GTK_STATE_SELECTED, &color);
	} else {
		if (no_result) {
			gtk_widget_modify_bg ((GtkWidget*) self->priv->search_entry, GTK_STATE_NORMAL, NULL);
			gtk_widget_modify_bg ((GtkWidget*) self->priv->search_entry, GTK_STATE_SELECTED, NULL);
		}
	}
	filename = (g_free (filename), NULL);
	basename = (g_free (basename), NULL);
	(playlist == NULL) ? NULL : (playlist = (g_object_unref (playlist), NULL));
}


static void xfmpc_dbbrowser_clear (XfmpcDbbrowser* self) {
	g_return_if_fail (self != NULL);
	gtk_list_store_clear (self->priv->store);
}


static void xfmpc_dbbrowser_menu_popup (XfmpcDbbrowser* self) {
	GtkTreeSelection* _tmp0_;
	GtkTreeSelection* selection;
	gint count;
	g_return_if_fail (self != NULL);
	_tmp0_ = NULL;
	selection = (_tmp0_ = gtk_tree_view_get_selection (self->priv->treeview), (_tmp0_ == NULL) ? NULL : g_object_ref (_tmp0_));
	count = gtk_tree_selection_count_selected_rows (selection);
	if (self->priv->is_searching) {
		gboolean _tmp1_;
		gtk_widget_show ((GtkWidget*) self->priv->mi_browse);
		_tmp1_ = FALSE;
		if (count == 1) {
			_tmp1_ = TRUE;
		} else {
			_tmp1_ = FALSE;
		}
		gtk_widget_set_sensitive ((GtkWidget*) self->priv->mi_browse, _tmp1_);
	} else {
		gtk_widget_hide ((GtkWidget*) self->priv->mi_browse);
	}
	gtk_menu_popup (self->priv->menu, NULL, NULL, NULL, NULL, (guint) 0, gtk_get_current_event_time ());
	(selection == NULL) ? NULL : (selection = (g_object_unref (selection), NULL));
}


/*
 * Signal callbacks
 */
static void xfmpc_dbbrowser_cb_playlist_changed (XfmpcDbbrowser* self) {
	GtkTreeModel* _tmp0_;
	GtkTreeModel* model;
	GtkTreeIter iter = {0};
	char* filename;
	gboolean is_bold;
	gboolean is_dir;
	XfmpcPlaylist* _tmp1_;
	XfmpcPlaylist* playlist;
	g_return_if_fail (self != NULL);
	_tmp0_ = NULL;
	model = (_tmp0_ = (GtkTreeModel*) self->priv->store, (_tmp0_ == NULL) ? NULL : g_object_ref (_tmp0_));
	filename = g_strdup ("");
	is_bold = FALSE;
	is_dir = FALSE;
	_tmp1_ = NULL;
	playlist = (_tmp1_ = XFMPC_PLAYLIST (g_object_get_data ((GObject*) self, "XfmpcPlaylist")), (_tmp1_ == NULL) ? NULL : g_object_ref (_tmp1_));
	if (!gtk_tree_model_get_iter_first (model, &iter)) {
		(model == NULL) ? NULL : (model = (g_object_unref (model), NULL));
		filename = (g_free (filename), NULL);
		(playlist == NULL) ? NULL : (playlist = (g_object_unref (playlist), NULL));
		return;
	}
	{
		gboolean _tmp2_;
		_tmp2_ = TRUE;
		while (TRUE) {
			PangoWeight _tmp3_;
			if (!_tmp2_) {
				if (!gtk_tree_model_iter_next (model, &iter)) {
					break;
				}
			}
			_tmp2_ = FALSE;
			gtk_tree_model_get (model, &iter, XFMPC_DBBROWSER_COLUMNS_COLUMN_FILENAME, &filename, XFMPC_DBBROWSER_COLUMNS_COLUMN_IS_DIR, &is_dir, -1, -1);
			is_bold = xfmpc_playlist_has_filename (playlist, filename, is_dir);
			_tmp3_ = 0;
			if (is_bold) {
				_tmp3_ = PANGO_WEIGHT_BOLD;
			} else {
				_tmp3_ = PANGO_WEIGHT_NORMAL;
			}
			gtk_list_store_set (self->priv->store, &iter, XFMPC_DBBROWSER_COLUMNS_COLUMN_WEIGHT, _tmp3_, -1, -1);
		}
	}
	(model == NULL) ? NULL : (model = (g_object_unref (model), NULL));
	filename = (g_free (filename), NULL);
	(playlist == NULL) ? NULL : (playlist = (g_object_unref (playlist), NULL));
}


static gboolean xfmpc_dbbrowser_cb_popup_menu (XfmpcDbbrowser* self) {
	gboolean result;
	g_return_val_if_fail (self != NULL, FALSE);
	xfmpc_dbbrowser_menu_popup (self);
	result = TRUE;
	return result;
}


static gboolean xfmpc_dbbrowser_cb_key_pressed (XfmpcDbbrowser* self, const GdkEventKey* event) {
	gboolean result;
	g_return_val_if_fail (self != NULL, FALSE);
	if ((*event).type != GDK_KEY_PRESS) {
		result = FALSE;
		return result;
	}
	switch ((*event).keyval) {
		case 0xff0d:
		{
			GtkTreeSelection* _tmp0_;
			GtkTreeSelection* selection;
			_tmp0_ = NULL;
			selection = (_tmp0_ = gtk_tree_view_get_selection (self->priv->treeview), (_tmp0_ == NULL) ? NULL : g_object_ref (_tmp0_));
			if (gtk_tree_selection_count_selected_rows (selection) > 1) {
				xfmpc_dbbrowser_add_selected_rows (self);
			} else {
				result = FALSE;
				(selection == NULL) ? NULL : (selection = (g_object_unref (selection), NULL));
				return result;
			}
			(selection == NULL) ? NULL : (selection = (g_object_unref (selection), NULL));
			break;
		}
		case 0xff08:
		{
			char* filename;
			filename = xfmpc_dbbrowser_get_parent_wdir (self);
			xfmpc_dbbrowser_set_wdir (self, filename);
			xfmpc_dbbrowser_reload (self);
			filename = (g_free (filename), NULL);
			break;
		}
		default:
		{
			result = FALSE;
			return result;
		}
	}
	result = TRUE;
	return result;
}


static gboolean xfmpc_dbbrowser_cb_button_released (XfmpcDbbrowser* self, const GdkEventButton* event) {
	gboolean result;
	gboolean _tmp0_;
	GtkTreePath* path;
	GtkTreeSelection* _tmp1_;
	GtkTreeSelection* selection;
	GtkTreePath* _tmp4_;
	gboolean _tmp3_;
	GtkTreePath* _tmp2_;
	g_return_val_if_fail (self != NULL, FALSE);
	_tmp0_ = FALSE;
	if ((*event).type != GDK_BUTTON_PRESS) {
		_tmp0_ = TRUE;
	} else {
		_tmp0_ = (*event).button != 3;
	}
	if (_tmp0_) {
		result = FALSE;
		return result;
	}
	path = NULL;
	_tmp1_ = NULL;
	selection = (_tmp1_ = gtk_tree_view_get_selection (self->priv->treeview), (_tmp1_ == NULL) ? NULL : g_object_ref (_tmp1_));
	if (gtk_tree_selection_count_selected_rows (selection) < 1) {
		result = TRUE;
		(path == NULL) ? NULL : (path = (gtk_tree_path_free (path), NULL));
		(selection == NULL) ? NULL : (selection = (g_object_unref (selection), NULL));
		return result;
	}
	_tmp4_ = NULL;
	_tmp2_ = NULL;
	if ((_tmp3_ = gtk_tree_view_get_path_at_pos (self->priv->treeview, (gint) (*event).x, (gint) (*event).y, &_tmp2_, NULL, NULL, NULL), path = (_tmp4_ = _tmp2_, (path == NULL) ? NULL : (path = (gtk_tree_path_free (path), NULL)), _tmp4_), _tmp3_)) {
		if (!gtk_tree_selection_path_is_selected (selection, path)) {
			gtk_tree_selection_unselect_all (selection);
			gtk_tree_selection_select_path (selection, path);
		}
	}
	xfmpc_dbbrowser_menu_popup (self);
	result = TRUE;
	(path == NULL) ? NULL : (path = (gtk_tree_path_free (path), NULL));
	(selection == NULL) ? NULL : (selection = (g_object_unref (selection), NULL));
	return result;
}


static void xfmpc_dbbrowser_cb_row_activated (XfmpcDbbrowser* self, const GtkTreePath* path, GtkTreeViewColumn* column) {
	const GtkTreePath* _tmp0_;
	GtkTreePath* ppath;
	GtkTreeModel* _tmp1_;
	GtkTreeModel* model;
	GtkTreeIter iter = {0};
	char* filename;
	gboolean is_dir;
	g_return_if_fail (self != NULL);
	g_return_if_fail (path != NULL);
	g_return_if_fail (column != NULL);
	_tmp0_ = NULL;
	ppath = (_tmp0_ = path, (_tmp0_ == NULL) ? NULL : gtk_tree_path_copy (_tmp0_));
	_tmp1_ = NULL;
	model = (_tmp1_ = (GtkTreeModel*) self->priv->store, (_tmp1_ == NULL) ? NULL : g_object_ref (_tmp1_));
	filename = g_strdup ("");
	is_dir = FALSE;
	if (!gtk_tree_model_get_iter (model, &iter, ppath)) {
		(ppath == NULL) ? NULL : (ppath = (gtk_tree_path_free (ppath), NULL));
		(model == NULL) ? NULL : (model = (g_object_unref (model), NULL));
		filename = (g_free (filename), NULL);
		return;
	}
	gtk_tree_model_get (model, &iter, XFMPC_DBBROWSER_COLUMNS_COLUMN_FILENAME, &filename, XFMPC_DBBROWSER_COLUMNS_COLUMN_IS_DIR, &is_dir, -1, -1);
	if (is_dir) {
		xfmpc_dbbrowser_set_wdir (self, filename);
		xfmpc_dbbrowser_reload (self);
	} else {
		xfmpc_mpdclient_queue_add (self->priv->mpdclient, filename);
		xfmpc_mpdclient_queue_commit (self->priv->mpdclient);
	}
	(ppath == NULL) ? NULL : (ppath = (gtk_tree_path_free (ppath), NULL));
	(model == NULL) ? NULL : (model = (g_object_unref (model), NULL));
	filename = (g_free (filename), NULL);
}


static void xfmpc_dbbrowser_cb_replace_with_selected_rows (XfmpcDbbrowser* self) {
	g_return_if_fail (self != NULL);
	xfmpc_mpdclient_queue_clear (self->priv->mpdclient);
	xfmpc_dbbrowser_add_selected_rows (self);
}


static void xfmpc_dbbrowser_cb_browse (XfmpcDbbrowser* self) {
	GtkTreeModel* _tmp0_;
	GtkTreeModel* model;
	GtkTreeIter iter = {0};
	GtkTreeSelection* _tmp1_;
	GtkTreeSelection* selection;
	GtkTreeModel* _tmp5_;
	GtkTreeModel* _tmp4_;
	GList* _tmp3_;
	GtkTreeModel* _tmp2_;
	GList* list;
	const GtkTreePath* _tmp6_;
	GtkTreePath* path;
	g_return_if_fail (self != NULL);
	_tmp0_ = NULL;
	model = (_tmp0_ = (GtkTreeModel*) self->priv->store, (_tmp0_ == NULL) ? NULL : g_object_ref (_tmp0_));
	_tmp1_ = NULL;
	selection = (_tmp1_ = gtk_tree_view_get_selection (self->priv->treeview), (_tmp1_ == NULL) ? NULL : g_object_ref (_tmp1_));
	if (gtk_tree_selection_count_selected_rows (selection) > 1) {
		(model == NULL) ? NULL : (model = (g_object_unref (model), NULL));
		(selection == NULL) ? NULL : (selection = (g_object_unref (selection), NULL));
		return;
	}
	_tmp5_ = NULL;
	_tmp4_ = NULL;
	_tmp3_ = NULL;
	_tmp2_ = NULL;
	list = (_tmp3_ = gtk_tree_selection_get_selected_rows (selection, &_tmp2_), model = (_tmp4_ = (_tmp5_ = _tmp2_, (_tmp5_ == NULL) ? NULL : g_object_ref (_tmp5_)), (model == NULL) ? NULL : (model = (g_object_unref (model), NULL)), _tmp4_), _tmp3_);
	_tmp6_ = NULL;
	path = (_tmp6_ = (const GtkTreePath*) g_list_nth_data (list, (guint) 0), (_tmp6_ == NULL) ? NULL : gtk_tree_path_copy (_tmp6_));
	if (gtk_tree_model_get_iter (model, &iter, path)) {
		char* filename;
		char* dir;
		filename = NULL;
		gtk_tree_model_get (model, &iter, XFMPC_DBBROWSER_COLUMNS_COLUMN_FILENAME, &filename, -1, -1);
		dir = g_path_get_dirname (filename);
		xfmpc_dbbrowser_set_wdir (self, dir);
		xfmpc_dbbrowser_reload (self);
		filename = (g_free (filename), NULL);
		dir = (g_free (dir), NULL);
	}
	(model == NULL) ? NULL : (model = (g_object_unref (model), NULL));
	(selection == NULL) ? NULL : (selection = (g_object_unref (selection), NULL));
	(list == NULL) ? NULL : (list = (_g_list_free_gtk_tree_path_free (list), NULL));
	(path == NULL) ? NULL : (path = (gtk_tree_path_free (path), NULL));
}


static void xfmpc_dbbrowser_cb_search_entry_activated (XfmpcDbbrowser* self) {
	const char* _tmp0_;
	char* entry_text;
	g_return_if_fail (self != NULL);
	_tmp0_ = NULL;
	entry_text = (_tmp0_ = gtk_entry_get_text (self->priv->search_entry), (_tmp0_ == NULL) ? NULL : g_strdup (_tmp0_));
	if (_vala_strcmp0 (entry_text, "") == 0) {
		self->priv->is_searching = FALSE;
		xfmpc_dbbrowser_reload (self);
		/* revert possible previous applied color */
		gtk_widget_modify_bg ((GtkWidget*) self->priv->search_entry, GTK_STATE_NORMAL, NULL);
		gtk_widget_modify_bg ((GtkWidget*) self->priv->search_entry, GTK_STATE_SELECTED, NULL);
		entry_text = (g_free (entry_text), NULL);
		return;
	}
	xfmpc_dbbrowser_search (self, entry_text);
	entry_text = (g_free (entry_text), NULL);
}


static gboolean xfmpc_dbbrowser_cb_search_entry_key_released (XfmpcDbbrowser* self, const GdkEventKey* event) {
	gboolean result;
	g_return_val_if_fail (self != NULL, FALSE);
	if ((*event).type != GDK_KEY_RELEASE) {
		result = FALSE;
		return result;
	}
	/* Escape */
	if ((*event).keyval == 0xff1b) {
		gtk_entry_set_text (self->priv->search_entry, "");
	}
	result = TRUE;
	return result;
}


static gboolean _xfmpc_dbbrowser_timeout_search_gsource_func (gpointer self) {
	return xfmpc_dbbrowser_timeout_search (self);
}


static void xfmpc_dbbrowser_cb_search_entry_changed (XfmpcDbbrowser* self) {
	g_return_if_fail (self != NULL);
	if (self->priv->search_timeout > 0) {
		g_source_remove (self->priv->search_timeout);
	}
	self->priv->search_timeout = g_timeout_add_full (G_PRIORITY_DEFAULT, (guint) 642, _xfmpc_dbbrowser_timeout_search_gsource_func, g_object_ref (self), g_object_unref);
}


static gboolean xfmpc_dbbrowser_timeout_search (XfmpcDbbrowser* self) {
	gboolean result;
	g_return_val_if_fail (self != NULL, FALSE);
	xfmpc_dbbrowser_cb_search_entry_activated (self);
	result = FALSE;
	return result;
}


XfmpcDbbrowser* xfmpc_dbbrowser_construct (GType object_type) {
	XfmpcDbbrowser * self;
	self = g_object_newv (object_type, 0, NULL);
	return self;
}


XfmpcDbbrowser* xfmpc_dbbrowser_new (void) {
	return xfmpc_dbbrowser_construct (XFMPC_TYPE_DBBROWSER);
}


static void _xfmpc_dbbrowser_add_selected_rows_gtk_menu_item_activate (GtkMenuItem* _sender, gpointer self) {
	xfmpc_dbbrowser_add_selected_rows (self);
}


static void _xfmpc_dbbrowser_cb_replace_with_selected_rows_gtk_menu_item_activate (GtkMenuItem* _sender, gpointer self) {
	xfmpc_dbbrowser_cb_replace_with_selected_rows (self);
}


static void _xfmpc_dbbrowser_cb_browse_gtk_menu_item_activate (GtkMenuItem* _sender, gpointer self) {
	xfmpc_dbbrowser_cb_browse (self);
}


static void _xfmpc_dbbrowser_reload_xfmpc_mpdclient_connected (XfmpcMpdclient* _sender, gpointer self) {
	xfmpc_dbbrowser_reload (self);
}


static void _xfmpc_dbbrowser_reload_xfmpc_mpdclient_database_changed (XfmpcMpdclient* _sender, gpointer self) {
	xfmpc_dbbrowser_reload (self);
}


static void _xfmpc_dbbrowser_cb_playlist_changed_xfmpc_mpdclient_playlist_changed (XfmpcMpdclient* _sender, gpointer self) {
	xfmpc_dbbrowser_cb_playlist_changed (self);
}


static void _xfmpc_dbbrowser_cb_row_activated_gtk_tree_view_row_activated (GtkTreeView* _sender, const GtkTreePath* path, GtkTreeViewColumn* column, gpointer self) {
	xfmpc_dbbrowser_cb_row_activated (self, path, column);
}


static gboolean _xfmpc_dbbrowser_cb_key_pressed_gtk_widget_key_press_event (GtkWidget* _sender, const GdkEventKey* event, gpointer self) {
	return xfmpc_dbbrowser_cb_key_pressed (self, event);
}


static gboolean _xfmpc_dbbrowser_cb_button_released_gtk_widget_button_press_event (GtkWidget* _sender, const GdkEventButton* event, gpointer self) {
	return xfmpc_dbbrowser_cb_button_released (self, event);
}


static gboolean _xfmpc_dbbrowser_cb_popup_menu_gtk_widget_popup_menu (GtkWidget* _sender, gpointer self) {
	return xfmpc_dbbrowser_cb_popup_menu (self);
}


static void _xfmpc_dbbrowser_cb_search_entry_activated_gtk_entry_activate (GtkEntry* _sender, gpointer self) {
	xfmpc_dbbrowser_cb_search_entry_activated (self);
}


static gboolean _xfmpc_dbbrowser_cb_search_entry_key_released_gtk_widget_key_release_event (GtkWidget* _sender, const GdkEventKey* event, gpointer self) {
	return xfmpc_dbbrowser_cb_search_entry_key_released (self, event);
}


static void _xfmpc_dbbrowser_cb_search_entry_changed_gtk_editable_changed (GtkEditable* _sender, gpointer self) {
	xfmpc_dbbrowser_cb_search_entry_changed (self);
}


static void _xfmpc_dbbrowser_reload_g_object_notify (GObject* _sender, GParamSpec* pspec, gpointer self) {
	xfmpc_dbbrowser_reload (self);
}


static GObject * xfmpc_dbbrowser_constructor (GType type, guint n_construct_properties, GObjectConstructParam * construct_properties) {
	GObject * obj;
	XfmpcDbbrowserClass * klass;
	GObjectClass * parent_class;
	XfmpcDbbrowser * self;
	klass = XFMPC_DBBROWSER_CLASS (g_type_class_peek (XFMPC_TYPE_DBBROWSER));
	parent_class = G_OBJECT_CLASS (g_type_class_peek_parent (klass));
	obj = parent_class->constructor (type, n_construct_properties, construct_properties);
	self = XFMPC_DBBROWSER (obj);
	{
		char* _tmp1_;
		const char* _tmp0_;
		GtkListStore* _tmp4_;
		GtkTreeView* _tmp5_;
		GtkCellRendererPixbuf* cell_pixbuf;
		GtkCellRendererText* cell_text;
		GtkScrolledWindow* scrolled;
		GtkMenu* _tmp6_;
		GtkImageMenuItem* mi;
		GtkImageMenuItem* _tmp7_;
		GtkImage* image;
		GtkImageMenuItem* _tmp8_;
		GtkImage* _tmp9_;
		GtkEntry* _tmp10_;
		self->priv->mpdclient = xfmpc_mpdclient_get ();
		self->priv->preferences = xfmpc_preferences_get ();
		_tmp1_ = NULL;
		_tmp0_ = NULL;
		self->priv->wdir = (_tmp1_ = (_tmp0_ = xfmpc_preferences_get_dbbrowser_last_path (self->priv->preferences), (_tmp0_ == NULL) ? NULL : g_strdup (_tmp0_)), self->priv->wdir = (g_free (self->priv->wdir), NULL), _tmp1_);
		if (self->priv->wdir != NULL) {
			char* _tmp2_;
			_tmp2_ = NULL;
			self->priv->last_wdir = (_tmp2_ = g_strndup (self->priv->wdir, (gsize) g_utf8_strlen (self->priv->wdir, -1)), self->priv->last_wdir = (g_free (self->priv->last_wdir), NULL), _tmp2_);
		} else {
			char* _tmp3_;
			_tmp3_ = NULL;
			self->priv->last_wdir = (_tmp3_ = g_strdup (""), self->priv->last_wdir = (g_free (self->priv->last_wdir), NULL), _tmp3_);
		}
		_tmp4_ = NULL;
		self->priv->store = (_tmp4_ = gtk_list_store_new ((gint) XFMPC_DBBROWSER_COLUMNS_N_COLUMNS, G_TYPE_INT, GDK_TYPE_PIXBUF, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_BOOLEAN, G_TYPE_INT, NULL), (self->priv->store == NULL) ? NULL : (self->priv->store = (g_object_unref (self->priv->store), NULL)), _tmp4_);
		_tmp5_ = NULL;
		self->priv->treeview = (_tmp5_ = g_object_ref_sink ((GtkTreeView*) gtk_tree_view_new ()), (self->priv->treeview == NULL) ? NULL : (self->priv->treeview = (g_object_unref (self->priv->treeview), NULL)), _tmp5_);
		gtk_tree_selection_set_mode (gtk_tree_view_get_selection (self->priv->treeview), GTK_SELECTION_MULTIPLE);
		gtk_tree_view_set_rubber_banding (self->priv->treeview, TRUE);
		gtk_tree_view_set_enable_search (self->priv->treeview, TRUE);
		gtk_tree_view_set_search_column (self->priv->treeview, (gint) XFMPC_DBBROWSER_COLUMNS_COLUMN_BASENAME);
		gtk_tree_view_set_headers_visible (self->priv->treeview, FALSE);
		gtk_tree_view_set_rules_hint (self->priv->treeview, TRUE);
		gtk_tree_view_set_model (self->priv->treeview, (GtkTreeModel*) self->priv->store);
		cell_pixbuf = g_object_ref_sink ((GtkCellRendererPixbuf*) gtk_cell_renderer_pixbuf_new ());
		gtk_tree_view_insert_column_with_attributes (self->priv->treeview, -1, "", (GtkCellRenderer*) cell_pixbuf, "pixbuf", XFMPC_DBBROWSER_COLUMNS_COLUMN_PIXBUF, NULL, NULL);
		cell_text = g_object_ref_sink ((GtkCellRendererText*) gtk_cell_renderer_text_new ());
		g_object_set (cell_text, "ellipsize", PANGO_ELLIPSIZE_END, NULL);
		gtk_tree_view_insert_column_with_attributes (self->priv->treeview, -1, "Filename", (GtkCellRenderer*) cell_text, "text", XFMPC_DBBROWSER_COLUMNS_COLUMN_BASENAME, "weight", XFMPC_DBBROWSER_COLUMNS_COLUMN_WEIGHT, NULL, NULL);
		scrolled = g_object_ref_sink ((GtkScrolledWindow*) gtk_scrolled_window_new (NULL, NULL));
		gtk_scrolled_window_set_policy (scrolled, GTK_POLICY_AUTOMATIC, GTK_POLICY_ALWAYS);
		_tmp6_ = NULL;
		self->priv->menu = (_tmp6_ = g_object_ref_sink ((GtkMenu*) gtk_menu_new ()), (self->priv->menu == NULL) ? NULL : (self->priv->menu = (g_object_unref (self->priv->menu), NULL)), _tmp6_);
		mi = g_object_ref_sink ((GtkImageMenuItem*) gtk_image_menu_item_new_from_stock (GTK_STOCK_ADD, NULL));
		gtk_menu_shell_append ((GtkMenuShell*) self->priv->menu, (GtkWidget*) ((GtkMenuItem*) mi));
		g_signal_connect_object ((GtkMenuItem*) mi, "activate", (GCallback) _xfmpc_dbbrowser_add_selected_rows_gtk_menu_item_activate, self, 0);
		_tmp7_ = NULL;
		mi = (_tmp7_ = g_object_ref_sink ((GtkImageMenuItem*) gtk_image_menu_item_new_with_mnemonic (_ ("Replace"))), (mi == NULL) ? NULL : (mi = (g_object_unref (mi), NULL)), _tmp7_);
		image = g_object_ref_sink ((GtkImage*) gtk_image_new_from_stock (GTK_STOCK_CUT, GTK_ICON_SIZE_MENU));
		gtk_image_menu_item_set_image (mi, (GtkWidget*) image);
		gtk_menu_shell_append ((GtkMenuShell*) self->priv->menu, (GtkWidget*) ((GtkMenuItem*) mi));
		g_signal_connect_object ((GtkMenuItem*) mi, "activate", (GCallback) _xfmpc_dbbrowser_cb_replace_with_selected_rows_gtk_menu_item_activate, self, 0);
		_tmp8_ = NULL;
		self->priv->mi_browse = (_tmp8_ = g_object_ref_sink ((GtkImageMenuItem*) gtk_image_menu_item_new_with_mnemonic (_ ("Browse"))), (self->priv->mi_browse == NULL) ? NULL : (self->priv->mi_browse = (g_object_unref (self->priv->mi_browse), NULL)), _tmp8_);
		_tmp9_ = NULL;
		image = (_tmp9_ = g_object_ref_sink ((GtkImage*) gtk_image_new_from_stock (GTK_STOCK_OPEN, GTK_ICON_SIZE_MENU)), (image == NULL) ? NULL : (image = (g_object_unref (image), NULL)), _tmp9_);
		gtk_image_menu_item_set_image (self->priv->mi_browse, (GtkWidget*) image);
		gtk_menu_shell_append ((GtkMenuShell*) self->priv->menu, (GtkWidget*) ((GtkMenuItem*) self->priv->mi_browse));
		g_signal_connect_object ((GtkMenuItem*) self->priv->mi_browse, "activate", (GCallback) _xfmpc_dbbrowser_cb_browse_gtk_menu_item_activate, self, 0);
		gtk_widget_show_all ((GtkWidget*) self->priv->menu);
		_tmp10_ = NULL;
		self->priv->search_entry = (_tmp10_ = g_object_ref_sink ((GtkEntry*) gtk_entry_new ()), (self->priv->search_entry == NULL) ? NULL : (self->priv->search_entry = (g_object_unref (self->priv->search_entry), NULL)), _tmp10_);
		gtk_container_add ((GtkContainer*) scrolled, (GtkWidget*) self->priv->treeview);
		gtk_box_pack_start ((GtkBox*) self, (GtkWidget*) scrolled, TRUE, TRUE, (guint) 0);
		gtk_box_pack_start ((GtkBox*) self, (GtkWidget*) self->priv->search_entry, FALSE, FALSE, (guint) 0);
		/* Signals */
		g_signal_connect_object (self->priv->mpdclient, "connected", (GCallback) _xfmpc_dbbrowser_reload_xfmpc_mpdclient_connected, self, 0);
		g_signal_connect_object (self->priv->mpdclient, "database-changed", (GCallback) _xfmpc_dbbrowser_reload_xfmpc_mpdclient_database_changed, self, 0);
		g_signal_connect_object (self->priv->mpdclient, "playlist-changed", (GCallback) _xfmpc_dbbrowser_cb_playlist_changed_xfmpc_mpdclient_playlist_changed, self, 0);
		g_signal_connect_object (self->priv->treeview, "row-activated", (GCallback) _xfmpc_dbbrowser_cb_row_activated_gtk_tree_view_row_activated, self, 0);
		g_signal_connect_object ((GtkWidget*) self->priv->treeview, "key-press-event", (GCallback) _xfmpc_dbbrowser_cb_key_pressed_gtk_widget_key_press_event, self, 0);
		g_signal_connect_object ((GtkWidget*) self->priv->treeview, "button-press-event", (GCallback) _xfmpc_dbbrowser_cb_button_released_gtk_widget_button_press_event, self, 0);
		g_signal_connect_object ((GtkWidget*) self->priv->treeview, "popup-menu", (GCallback) _xfmpc_dbbrowser_cb_popup_menu_gtk_widget_popup_menu, self, 0);
		g_signal_connect_object (self->priv->search_entry, "activate", (GCallback) _xfmpc_dbbrowser_cb_search_entry_activated_gtk_entry_activate, self, 0);
		g_signal_connect_object ((GtkWidget*) self->priv->search_entry, "key-release-event", (GCallback) _xfmpc_dbbrowser_cb_search_entry_key_released_gtk_widget_key_release_event, self, 0);
		g_signal_connect_object ((GtkEditable*) self->priv->search_entry, "changed", (GCallback) _xfmpc_dbbrowser_cb_search_entry_changed_gtk_editable_changed, self, 0);
		g_signal_connect_object ((GObject*) self->priv->preferences, "notify::song-format", (GCallback) _xfmpc_dbbrowser_reload_g_object_notify, self, 0);
		g_signal_connect_object ((GObject*) self->priv->preferences, "notify::song-format-custom", (GCallback) _xfmpc_dbbrowser_reload_g_object_notify, self, 0);
		(cell_pixbuf == NULL) ? NULL : (cell_pixbuf = (g_object_unref (cell_pixbuf), NULL));
		(cell_text == NULL) ? NULL : (cell_text = (g_object_unref (cell_text), NULL));
		(scrolled == NULL) ? NULL : (scrolled = (g_object_unref (scrolled), NULL));
		(mi == NULL) ? NULL : (mi = (g_object_unref (mi), NULL));
		(image == NULL) ? NULL : (image = (g_object_unref (image), NULL));
	}
	return obj;
}


static void xfmpc_dbbrowser_class_init (XfmpcDbbrowserClass * klass) {
	xfmpc_dbbrowser_parent_class = g_type_class_peek_parent (klass);
	g_type_class_add_private (klass, sizeof (XfmpcDbbrowserPrivate));
	G_OBJECT_CLASS (klass)->constructor = xfmpc_dbbrowser_constructor;
	G_OBJECT_CLASS (klass)->finalize = xfmpc_dbbrowser_finalize;
}


static void xfmpc_dbbrowser_instance_init (XfmpcDbbrowser * self) {
	self->priv = XFMPC_DBBROWSER_GET_PRIVATE (self);
}


static void xfmpc_dbbrowser_finalize (GObject* obj) {
	XfmpcDbbrowser * self;
	self = XFMPC_DBBROWSER (obj);
	(self->priv->store == NULL) ? NULL : (self->priv->store = (g_object_unref (self->priv->store), NULL));
	(self->priv->treeview == NULL) ? NULL : (self->priv->treeview = (g_object_unref (self->priv->treeview), NULL));
	(self->priv->menu == NULL) ? NULL : (self->priv->menu = (g_object_unref (self->priv->menu), NULL));
	(self->priv->search_entry == NULL) ? NULL : (self->priv->search_entry = (g_object_unref (self->priv->search_entry), NULL));
	(self->priv->mi_browse == NULL) ? NULL : (self->priv->mi_browse = (g_object_unref (self->priv->mi_browse), NULL));
	self->priv->wdir = (g_free (self->priv->wdir), NULL);
	self->priv->last_wdir = (g_free (self->priv->last_wdir), NULL);
	G_OBJECT_CLASS (xfmpc_dbbrowser_parent_class)->finalize (obj);
}


GType xfmpc_dbbrowser_get_type (void) {
	static GType xfmpc_dbbrowser_type_id = 0;
	if (xfmpc_dbbrowser_type_id == 0) {
		static const GTypeInfo g_define_type_info = { sizeof (XfmpcDbbrowserClass), (GBaseInitFunc) NULL, (GBaseFinalizeFunc) NULL, (GClassInitFunc) xfmpc_dbbrowser_class_init, (GClassFinalizeFunc) NULL, NULL, sizeof (XfmpcDbbrowser), 0, (GInstanceInitFunc) xfmpc_dbbrowser_instance_init, NULL };
		xfmpc_dbbrowser_type_id = g_type_register_static (GTK_TYPE_VBOX, "XfmpcDbbrowser", &g_define_type_info, 0);
	}
	return xfmpc_dbbrowser_type_id;
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




