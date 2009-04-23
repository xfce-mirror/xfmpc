
#ifndef __XFMPC_H__
#define __XFMPC_H__

#include <glib.h>
#include <libxfcegui4/libxfcegui4.h>
#include <gtk/gtk.h>
#include <stdlib.h>
#include <string.h>

G_BEGIN_DECLS


#define XFMPC_TYPE_PREFERENCES_DIALOG (xfmpc_preferences_dialog_get_type ())
#define XFMPC_PREFERENCES_DIALOG(obj) (G_TYPE_CHECK_INSTANCE_CAST ((obj), XFMPC_TYPE_PREFERENCES_DIALOG, XfmpcPreferencesDialog))
#define XFMPC_PREFERENCES_DIALOG_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST ((klass), XFMPC_TYPE_PREFERENCES_DIALOG, XfmpcPreferencesDialogClass))
#define XFMPC_IS_PREFERENCES_DIALOG(obj) (G_TYPE_CHECK_INSTANCE_TYPE ((obj), XFMPC_TYPE_PREFERENCES_DIALOG))
#define XFMPC_IS_PREFERENCES_DIALOG_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), XFMPC_TYPE_PREFERENCES_DIALOG))
#define XFMPC_PREFERENCES_DIALOG_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj), XFMPC_TYPE_PREFERENCES_DIALOG, XfmpcPreferencesDialogClass))

typedef struct _XfmpcPreferencesDialog XfmpcPreferencesDialog;
typedef struct _XfmpcPreferencesDialogClass XfmpcPreferencesDialogClass;
typedef struct _XfmpcPreferencesDialogPrivate XfmpcPreferencesDialogPrivate;

#define XFMPC_TYPE_SONG_DIALOG (xfmpc_song_dialog_get_type ())
#define XFMPC_SONG_DIALOG(obj) (G_TYPE_CHECK_INSTANCE_CAST ((obj), XFMPC_TYPE_SONG_DIALOG, XfmpcSongDialog))
#define XFMPC_SONG_DIALOG_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST ((klass), XFMPC_TYPE_SONG_DIALOG, XfmpcSongDialogClass))
#define XFMPC_IS_SONG_DIALOG(obj) (G_TYPE_CHECK_INSTANCE_TYPE ((obj), XFMPC_TYPE_SONG_DIALOG))
#define XFMPC_IS_SONG_DIALOG_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), XFMPC_TYPE_SONG_DIALOG))
#define XFMPC_SONG_DIALOG_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj), XFMPC_TYPE_SONG_DIALOG, XfmpcSongDialogClass))

typedef struct _XfmpcSongDialog XfmpcSongDialog;
typedef struct _XfmpcSongDialogClass XfmpcSongDialogClass;
typedef struct _XfmpcSongDialogPrivate XfmpcSongDialogPrivate;

#define XFMPC_TYPE_STATUSBAR (xfmpc_statusbar_get_type ())
#define XFMPC_STATUSBAR(obj) (G_TYPE_CHECK_INSTANCE_CAST ((obj), XFMPC_TYPE_STATUSBAR, XfmpcStatusbar))
#define XFMPC_STATUSBAR_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST ((klass), XFMPC_TYPE_STATUSBAR, XfmpcStatusbarClass))
#define XFMPC_IS_STATUSBAR(obj) (G_TYPE_CHECK_INSTANCE_TYPE ((obj), XFMPC_TYPE_STATUSBAR))
#define XFMPC_IS_STATUSBAR_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), XFMPC_TYPE_STATUSBAR))
#define XFMPC_STATUSBAR_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj), XFMPC_TYPE_STATUSBAR, XfmpcStatusbarClass))

typedef struct _XfmpcStatusbar XfmpcStatusbar;
typedef struct _XfmpcStatusbarClass XfmpcStatusbarClass;
typedef struct _XfmpcStatusbarPrivate XfmpcStatusbarPrivate;

#define XFMPC_TYPE_DBBROWSER (xfmpc_dbbrowser_get_type ())
#define XFMPC_DBBROWSER(obj) (G_TYPE_CHECK_INSTANCE_CAST ((obj), XFMPC_TYPE_DBBROWSER, XfmpcDbbrowser))
#define XFMPC_DBBROWSER_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST ((klass), XFMPC_TYPE_DBBROWSER, XfmpcDbbrowserClass))
#define XFMPC_IS_DBBROWSER(obj) (G_TYPE_CHECK_INSTANCE_TYPE ((obj), XFMPC_TYPE_DBBROWSER))
#define XFMPC_IS_DBBROWSER_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), XFMPC_TYPE_DBBROWSER))
#define XFMPC_DBBROWSER_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj), XFMPC_TYPE_DBBROWSER, XfmpcDbbrowserClass))

typedef struct _XfmpcDbbrowser XfmpcDbbrowser;
typedef struct _XfmpcDbbrowserClass XfmpcDbbrowserClass;
typedef struct _XfmpcDbbrowserPrivate XfmpcDbbrowserPrivate;

#define XFMPC_TYPE_EXTENDED_INTERFACE (xfmpc_extended_interface_get_type ())
#define XFMPC_EXTENDED_INTERFACE(obj) (G_TYPE_CHECK_INSTANCE_CAST ((obj), XFMPC_TYPE_EXTENDED_INTERFACE, XfmpcExtendedInterface))
#define XFMPC_EXTENDED_INTERFACE_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST ((klass), XFMPC_TYPE_EXTENDED_INTERFACE, XfmpcExtendedInterfaceClass))
#define XFMPC_IS_EXTENDED_INTERFACE(obj) (G_TYPE_CHECK_INSTANCE_TYPE ((obj), XFMPC_TYPE_EXTENDED_INTERFACE))
#define XFMPC_IS_EXTENDED_INTERFACE_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), XFMPC_TYPE_EXTENDED_INTERFACE))
#define XFMPC_EXTENDED_INTERFACE_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj), XFMPC_TYPE_EXTENDED_INTERFACE, XfmpcExtendedInterfaceClass))

typedef struct _XfmpcExtendedInterface XfmpcExtendedInterface;
typedef struct _XfmpcExtendedInterfaceClass XfmpcExtendedInterfaceClass;
typedef struct _XfmpcExtendedInterfacePrivate XfmpcExtendedInterfacePrivate;

#define XFMPC_EXTENDED_INTERFACE_TYPE_EXTENDED_INTERFACE_WIDGET (xfmpc_extended_interface_extended_interface_widget_get_type ())

#define XFMPC_TYPE_PLAYLIST (xfmpc_playlist_get_type ())
#define XFMPC_PLAYLIST(obj) (G_TYPE_CHECK_INSTANCE_CAST ((obj), XFMPC_TYPE_PLAYLIST, XfmpcPlaylist))
#define XFMPC_PLAYLIST_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST ((klass), XFMPC_TYPE_PLAYLIST, XfmpcPlaylistClass))
#define XFMPC_IS_PLAYLIST(obj) (G_TYPE_CHECK_INSTANCE_TYPE ((obj), XFMPC_TYPE_PLAYLIST))
#define XFMPC_IS_PLAYLIST_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), XFMPC_TYPE_PLAYLIST))
#define XFMPC_PLAYLIST_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj), XFMPC_TYPE_PLAYLIST, XfmpcPlaylistClass))

typedef struct _XfmpcPlaylist XfmpcPlaylist;
typedef struct _XfmpcPlaylistClass XfmpcPlaylistClass;
typedef struct _XfmpcPlaylistPrivate XfmpcPlaylistPrivate;

struct _XfmpcPreferencesDialog {
	XfceTitledDialog parent_instance;
	XfmpcPreferencesDialogPrivate * priv;
};

struct _XfmpcPreferencesDialogClass {
	XfceTitledDialogClass parent_class;
};

struct _XfmpcSongDialog {
	XfceTitledDialog parent_instance;
	XfmpcSongDialogPrivate * priv;
};

struct _XfmpcSongDialogClass {
	XfceTitledDialogClass parent_class;
};

struct _XfmpcStatusbar {
	GtkStatusbar parent_instance;
	XfmpcStatusbarPrivate * priv;
};

struct _XfmpcStatusbarClass {
	GtkStatusbarClass parent_class;
};

struct _XfmpcDbbrowser {
	GtkVBox parent_instance;
	XfmpcDbbrowserPrivate * priv;
};

struct _XfmpcDbbrowserClass {
	GtkVBoxClass parent_class;
};

struct _XfmpcExtendedInterface {
	GtkVBox parent_instance;
	XfmpcExtendedInterfacePrivate * priv;
};

struct _XfmpcExtendedInterfaceClass {
	GtkVBoxClass parent_class;
};

typedef enum  {
	XFMPC_EXTENDED_INTERFACE_EXTENDED_INTERFACE_WIDGET_PLAYLIST,
	XFMPC_EXTENDED_INTERFACE_EXTENDED_INTERFACE_WIDGET_DBBROWSER
} XfmpcExtendedInterfaceExtendedInterfaceWidget;

struct _XfmpcPlaylist {
	GtkVBox parent_instance;
	XfmpcPlaylistPrivate * priv;
};

struct _XfmpcPlaylistClass {
	GtkVBoxClass parent_class;
};


GType xfmpc_preferences_dialog_get_type (void);
XfmpcPreferencesDialog* xfmpc_preferences_dialog_new (void);
XfmpcPreferencesDialog* xfmpc_preferences_dialog_construct (GType object_type);
GType xfmpc_song_dialog_get_type (void);
XfmpcSongDialog* xfmpc_song_dialog_new (gint song_id);
XfmpcSongDialog* xfmpc_song_dialog_construct (GType object_type, gint song_id);
GType xfmpc_statusbar_get_type (void);
XfmpcStatusbar* xfmpc_statusbar_new (void);
XfmpcStatusbar* xfmpc_statusbar_construct (GType object_type);
void xfmpc_statusbar_set_text (XfmpcStatusbar* self, const char* value);
GType xfmpc_dbbrowser_get_type (void);
void xfmpc_dbbrowser_free (XfmpcDbbrowser* self);
void xfmpc_dbbrowser_reload (XfmpcDbbrowser* self);
gboolean xfmpc_dbbrowser_wdir_is_root (XfmpcDbbrowser* self);
char* xfmpc_dbbrowser_get_parent_wdir (XfmpcDbbrowser* self);
void xfmpc_dbbrowser_append (XfmpcDbbrowser* self, const char* filename, const char* basename, gboolean is_dir, gboolean is_bold);
void xfmpc_dbbrowser_set_wdir (XfmpcDbbrowser* self, const char* dir);
void xfmpc_dbbrowser_add_selected_rows (XfmpcDbbrowser* self);
void xfmpc_dbbrowser_search (XfmpcDbbrowser* self, const char* query);
XfmpcDbbrowser* xfmpc_dbbrowser_new (void);
XfmpcDbbrowser* xfmpc_dbbrowser_construct (GType object_type);
GType xfmpc_extended_interface_get_type (void);
GType xfmpc_extended_interface_extended_interface_widget_get_type (void);
void xfmpc_extended_interface_set_active (XfmpcExtendedInterface* self, XfmpcExtendedInterfaceExtendedInterfaceWidget active_widget);
XfmpcExtendedInterface* xfmpc_extended_interface_new (void);
XfmpcExtendedInterface* xfmpc_extended_interface_construct (GType object_type);
GType xfmpc_playlist_get_type (void);
void xfmpc_playlist_append (XfmpcPlaylist* self, gint id, gint pos, const char* filename, const char* song, const char* length);
void xfmpc_playlist_refresh_current_song (XfmpcPlaylist* self);
void xfmpc_playlist_select_row (XfmpcPlaylist* self, gint i);
void xfmpc_playlist_delete_selection (XfmpcPlaylist* self);
XfmpcPlaylist* xfmpc_playlist_new (void);
XfmpcPlaylist* xfmpc_playlist_construct (GType object_type);


G_END_DECLS

#endif
