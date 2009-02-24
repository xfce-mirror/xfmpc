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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <gtk/gtk.h>
#include <libxfcegui4/libxfcegui4.h>
#include <libxfce4util/libxfce4util.h>

#include "extended-interface.h"
#include "mpdclient.h"
#include "playlist.h"
#include "dbbrowser.h"
#include "statusbar.h"
#include "xfce-arrow-button.h"
#include "preferences-dialog.h"
#include "preferences.h"

#define BORDER 4

#define GET_PRIVATE(o) \
    (G_TYPE_INSTANCE_GET_PRIVATE ((o), XFMPC_TYPE_EXTENDED_INTERFACE, XfmpcExtendedInterfacePrivate))



/* List store identifiers */
enum
{
  COLUMN_STRING,
  COLUMN_POINTER,
  N_COLUMNS,
};



static void xfmpc_extended_interface_class_init               (XfmpcExtendedInterfaceClass *klass);
static void xfmpc_extended_interface_init                     (XfmpcExtendedInterface *extended_interface);
static void xfmpc_extended_interface_dispose                  (GObject *object);
static void xfmpc_extended_interface_finalize                 (GObject *object);

static void xfmpc_extended_interface_context_menu_new         (XfmpcExtendedInterface *extended_interface,
                                                               GtkWidget *attach_widget);
static void xfmpc_preferences_dialog_show                     (XfmpcExtendedInterface *extended_interface);

static void cb_interface_changed                              (GtkComboBox *widget,
                                                               XfmpcExtendedInterface *extended_interface);
static void cb_repeat_switch                                  (XfmpcExtendedInterface *extended_interface);
static void cb_random_switch                                  (XfmpcExtendedInterface *extended_interface);
static void cb_context_menu_clicked                           (GtkToggleButton *button,
                                                               XfmpcExtendedInterface *extended_interface);
static void cb_context_menu_deactivate                        (GtkMenuShell *menu,
                                                               GtkWidget *attach_widget);
static void popup_context_menu                                (XfmpcExtendedInterface *extended_interface);
static void position_context_menu                             (GtkMenu *menu,
                                                               gint *x,
                                                               gint *y,
                                                               gboolean *push_in,
                                                               GtkWidget *widget);

static void xfmpc_extended_interface_action_statusbar_changed (GtkToggleAction *action,
                                                               XfmpcExtendedInterface *extended_interface);

static void xfmpc_extended_interface_update_statusbar         (XfmpcExtendedInterface *extended_interface);

static void cb_playlist_changed                               (XfmpcExtendedInterface *extended_interface);
static void cb_show_statusbar_changed                         (XfmpcExtendedInterface *extended_interface,
                                                               GParamSpec *pspec);



static const GtkToggleActionEntry toggle_action_entries[] =
{
  { "view-statusbar", NULL, "Statusbar", NULL, "Change the visibility of the statusbar", G_CALLBACK (xfmpc_extended_interface_action_statusbar_changed), FALSE, },
};



struct _XfmpcExtendedInterfaceClass
{
  GtkVBoxClass                      parent_class;
};

struct _XfmpcExtendedInterface
{
  GtkVBox                           parent;
  XfmpcMpdclient                   *mpdclient;
  XfmpcPreferences                 *preferences;
  /*<private>*/
  XfmpcExtendedInterfacePrivate    *priv;
};

struct _XfmpcExtendedInterfacePrivate
{
  GtkActionGroup                   *action_group;
  GtkListStore                     *list_store;
  GtkWidget                        *combobox;
  GtkWidget                        *notebook;
  GtkWidget                        *context_button;
  GtkWidget                        *context_menu;
  GtkWidget                        *statusbar;
};



static GObjectClass *parent_class = NULL;



GType
xfmpc_extended_interface_get_type ()
{
  static GType xfmpc_extended_interface_type = G_TYPE_INVALID;

  if (G_UNLIKELY (xfmpc_extended_interface_type == G_TYPE_INVALID))
    {
      static const GTypeInfo xfmpc_extended_interface_info =
        {
          sizeof (XfmpcExtendedInterfaceClass),
          (GBaseInitFunc) NULL,
          (GBaseFinalizeFunc) NULL,
          (GClassInitFunc) xfmpc_extended_interface_class_init,
          (GClassFinalizeFunc) NULL,
          NULL,
          sizeof (XfmpcExtendedInterface),
          0,
          (GInstanceInitFunc) xfmpc_extended_interface_init,
          NULL
        };
      xfmpc_extended_interface_type = g_type_register_static (GTK_TYPE_VBOX, "XfmpcExtendedInterface", &xfmpc_extended_interface_info, 0);
    }

  return xfmpc_extended_interface_type;
}



static void
xfmpc_extended_interface_class_init (XfmpcExtendedInterfaceClass *klass)
{
  GObjectClass *gobject_class;

  g_type_class_add_private (klass, sizeof (XfmpcExtendedInterfacePrivate));

  parent_class = g_type_class_peek_parent (klass);

  gobject_class = G_OBJECT_CLASS (klass);
  gobject_class->dispose = xfmpc_extended_interface_dispose;
  gobject_class->finalize = xfmpc_extended_interface_finalize;
}

static void
xfmpc_extended_interface_init (XfmpcExtendedInterface *extended_interface)
{
  XfmpcExtendedInterfacePrivate *priv = extended_interface->priv = GET_PRIVATE (extended_interface);
  GtkAction *action;
  gboolean active;

  extended_interface->mpdclient = xfmpc_mpdclient_get ();
  extended_interface->preferences = xfmpc_preferences_get ();

  /* Hbox  */
  GtkWidget *hbox = gtk_hbox_new (FALSE, 2);
  gtk_box_pack_start (GTK_BOX (extended_interface), hbox, FALSE, FALSE, 2);

  /* Clear playlist */
  GtkWidget *widget = gtk_button_new ();
  gtk_widget_set_tooltip_text (widget, _("Clear Playlist"));
  gtk_box_pack_start (GTK_BOX (hbox), widget, FALSE, FALSE, 0);
  g_signal_connect_swapped (widget, "clicked",
                            G_CALLBACK (xfmpc_mpdclient_playlist_clear), extended_interface->mpdclient);

  GtkWidget *image = gtk_image_new_from_stock (GTK_STOCK_NEW, GTK_ICON_SIZE_MENU);
  gtk_button_set_image (GTK_BUTTON (widget), image);

  /* Refresh database */
  widget = gtk_button_new ();
  gtk_widget_set_tooltip_text (widget, _("Refresh Database"));
  gtk_box_pack_start (GTK_BOX (hbox), widget, FALSE, FALSE, 0);
  g_signal_connect_swapped (widget, "clicked",
                            G_CALLBACK (xfmpc_mpdclient_database_refresh), extended_interface->mpdclient);

  image = gtk_image_new_from_stock (GTK_STOCK_REFRESH, GTK_ICON_SIZE_MENU);
  gtk_button_set_image (GTK_BUTTON (widget), image);

  /* Context menu */
  priv->context_button = GTK_WIDGET (xfce_arrow_button_new (GTK_ARROW_DOWN));
  gtk_widget_set_tooltip_text (priv->context_button, _("Context Menu"));
  gtk_box_pack_start (GTK_BOX (hbox), priv->context_button, FALSE, FALSE, 0);
  g_signal_connect_swapped (priv->context_button, "pressed",
                            G_CALLBACK (popup_context_menu), extended_interface);
  g_signal_connect (priv->context_button, "clicked",
                    G_CALLBACK (cb_context_menu_clicked), extended_interface);

  /* Combo box */
  priv->list_store = gtk_list_store_new (N_COLUMNS,
                                         G_TYPE_STRING,
                                         G_TYPE_POINTER);

  priv->combobox = gtk_combo_box_new_with_model (GTK_TREE_MODEL (priv->list_store));
  gtk_box_pack_start (GTK_BOX (hbox), priv->combobox, TRUE, TRUE, 0);
  g_signal_connect (priv->combobox, "changed",
                    G_CALLBACK (cb_interface_changed), extended_interface);

  GtkCellRenderer *cell = gtk_cell_renderer_text_new ();
  gtk_cell_layout_pack_start (GTK_CELL_LAYOUT (priv->combobox), cell, TRUE);
  gtk_cell_layout_set_attributes (GTK_CELL_LAYOUT (priv->combobox),
                                  cell, "text", COLUMN_STRING,
                                  NULL);

  /* Notebook */
  priv->notebook = gtk_notebook_new ();
  gtk_box_pack_start (GTK_BOX (extended_interface), priv->notebook, TRUE, TRUE, 0);

  gtk_notebook_set_show_tabs (GTK_NOTEBOOK (priv->notebook), FALSE);

  /* Extended interface widgets */
  GtkWidget *child = xfmpc_playlist_new ();
  xfmpc_extended_interface_append_child (extended_interface, child, _("Current Playlist"));

  child = xfmpc_dbbrowser_new ();
  xfmpc_extended_interface_append_child (extended_interface, child, _("Browse database"));

  /* Action Group */
  priv->action_group = gtk_action_group_new ("XfmpxExtendedInterface");
  gtk_action_group_add_toggle_actions (priv->action_group, toggle_action_entries,
                                       G_N_ELEMENTS (toggle_action_entries), GTK_WIDGET (extended_interface));

  /* show-statusbar action */
  action = gtk_action_group_get_action (priv->action_group, "view-statusbar");
  g_object_get (G_OBJECT (extended_interface->preferences), "show-statusbar", &active, NULL);
  gtk_toggle_action_set_active (GTK_TOGGLE_ACTION (action), active);

  /* === Signals === */
  g_signal_connect_swapped (extended_interface->mpdclient, "playlist-changed",
                            G_CALLBACK (cb_playlist_changed), extended_interface);

  g_signal_connect_swapped (extended_interface->preferences, "notify::show-statusbar",
                            G_CALLBACK (cb_show_statusbar_changed), extended_interface);
}

static void
xfmpc_extended_interface_dispose (GObject *object)
{
  XfmpcExtendedInterfacePrivate *priv = XFMPC_EXTENDED_INTERFACE (object)->priv;

  if (GTK_IS_MENU (priv->context_menu))
    {
      gtk_menu_detach (GTK_MENU (priv->context_menu));
      priv->context_menu = NULL;
    }

  if (GTK_IS_WIDGET (priv->context_button))
    {
      gtk_widget_destroy (priv->context_button);
      priv->context_button = NULL;
    }

  (*G_OBJECT_CLASS (parent_class)->dispose) (object);
}

static void
xfmpc_extended_interface_finalize (GObject *object)
{
  XfmpcExtendedInterface *extended_interface = XFMPC_EXTENDED_INTERFACE (object);
  g_object_unref (G_OBJECT (extended_interface->mpdclient));
  (*G_OBJECT_CLASS (parent_class)->finalize) (object);
}



GtkWidget*
xfmpc_extended_interface_new ()
{
  return g_object_new (XFMPC_TYPE_EXTENDED_INTERFACE, NULL);
}

void
xfmpc_extended_interface_append_child (XfmpcExtendedInterface *extended_interface,
                                       GtkWidget *child,
                                       const gchar *title)
{
  XfmpcExtendedInterfacePrivate *priv = XFMPC_EXTENDED_INTERFACE (extended_interface)->priv;
  GtkTreeIter iter;

  gtk_list_store_append (priv->list_store, &iter);
  gtk_list_store_set (priv->list_store, &iter,
                      COLUMN_STRING, title,
                      COLUMN_POINTER, child,
                      -1);

  if (gtk_combo_box_get_active(GTK_COMBO_BOX (priv->combobox)) == -1)
    gtk_combo_box_set_active (GTK_COMBO_BOX (priv->combobox), 0);

  gtk_notebook_append_page (GTK_NOTEBOOK (priv->notebook), child, NULL);
  gtk_notebook_set_tab_label_packing (GTK_NOTEBOOK (priv->notebook), child, TRUE, TRUE, GTK_PACK_START);
}

static void
xfmpc_extended_interface_context_menu_new (XfmpcExtendedInterface *extended_interface,
                                           GtkWidget *attach_widget)
{
  XfmpcExtendedInterfacePrivate *priv = XFMPC_EXTENDED_INTERFACE (extended_interface)->priv;

  GtkWidget *menu = priv->context_menu = gtk_menu_new ();
  gtk_menu_set_screen (GTK_MENU (menu), gtk_widget_get_screen (GTK_WIDGET (attach_widget)));
  gtk_menu_attach_to_widget (GTK_MENU (menu), GTK_WIDGET (attach_widget), NULL);
  g_signal_connect (menu, "deactivate",
                    G_CALLBACK (cb_context_menu_deactivate), attach_widget);

  GtkWidget *mi = gtk_check_menu_item_new_with_label (_("Repeat"));
  gtk_check_menu_item_set_active (GTK_CHECK_MENU_ITEM (mi),
                                  xfmpc_mpdclient_get_repeat (extended_interface->mpdclient));
  g_signal_connect_swapped (mi, "activate",
                            G_CALLBACK (cb_repeat_switch), extended_interface);
  gtk_menu_shell_append (GTK_MENU_SHELL (menu), mi);

  mi = gtk_check_menu_item_new_with_label (_("Random"));
  gtk_check_menu_item_set_active (GTK_CHECK_MENU_ITEM (mi),
                                  xfmpc_mpdclient_get_random (extended_interface->mpdclient));
  g_signal_connect_swapped (mi, "activate",
                            G_CALLBACK (cb_random_switch), extended_interface);
  gtk_menu_shell_append (GTK_MENU_SHELL (menu), mi);

  mi = gtk_separator_menu_item_new ();
  gtk_menu_shell_append (GTK_MENU_SHELL (menu), mi);

  GtkWidget *image = gtk_image_new_from_stock (GTK_STOCK_PREFERENCES, GTK_ICON_SIZE_MENU);
  mi = gtk_image_menu_item_new_with_label (_("Preferences"));
  gtk_image_menu_item_set_image (GTK_IMAGE_MENU_ITEM (mi), image);
  g_signal_connect_swapped (mi, "activate",
                            G_CALLBACK (xfmpc_preferences_dialog_show), extended_interface);
  gtk_menu_shell_append (GTK_MENU_SHELL (menu), mi);

  gtk_widget_show_all (menu);
}

static void
xfmpc_preferences_dialog_show (XfmpcExtendedInterface *extended_interface)
{
  GtkWidget *dialog = xfmpc_preferences_dialog_new (NULL);
  gtk_widget_show (dialog);
}



static void
cb_interface_changed (GtkComboBox *widget,
                      XfmpcExtendedInterface *extended_interface)
{
  XfmpcExtendedInterfacePrivate *priv = XFMPC_EXTENDED_INTERFACE (extended_interface)->priv;
  GtkWidget            *child;
  GtkTreeIter           iter;
  gint                  i;

  if (gtk_combo_box_get_active_iter (widget, &iter) == FALSE)
    return;

  gtk_tree_model_get (GTK_TREE_MODEL (priv->list_store), &iter,
                      COLUMN_POINTER, &child,
                      -1);
  g_return_if_fail (G_LIKELY (GTK_IS_WIDGET (child)));

  i = gtk_notebook_page_num (GTK_NOTEBOOK (priv->notebook), child);
  gtk_notebook_set_current_page (GTK_NOTEBOOK (priv->notebook), i);
}

static void
cb_repeat_switch (XfmpcExtendedInterface *extended_interface)
{
  xfmpc_mpdclient_set_repeat (extended_interface->mpdclient,
                              !xfmpc_mpdclient_get_repeat (extended_interface->mpdclient));
}

static void
cb_random_switch (XfmpcExtendedInterface *extended_interface)
{
  xfmpc_mpdclient_set_random (extended_interface->mpdclient,
                              !xfmpc_mpdclient_get_random (extended_interface->mpdclient));
}

static void
cb_context_menu_clicked (GtkToggleButton *button,
                         XfmpcExtendedInterface *extended_interface)
{
  if (!gtk_toggle_button_get_active (button))
    return;

  popup_context_menu (extended_interface);
}

static void
cb_context_menu_deactivate (GtkMenuShell *menu,
                            GtkWidget *attach_widget)
{
  gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (attach_widget), FALSE);
}

static void
popup_context_menu (XfmpcExtendedInterface *extended_interface)
{
  XfmpcExtendedInterfacePrivate *priv = XFMPC_EXTENDED_INTERFACE (extended_interface)->priv;

  if (!GTK_IS_MENU (priv->context_menu))
    xfmpc_extended_interface_context_menu_new (extended_interface, priv->context_button);

  gtk_menu_popup (GTK_MENU (priv->context_menu),
                  NULL,
                  NULL,
                  (GtkMenuPositionFunc) position_context_menu,
                  GTK_WIDGET (priv->context_button),
                  0,
                  gtk_get_current_event_time ());
}

static void
position_context_menu (GtkMenu *menu,
                       gint *x,
                       gint *y,
                       gboolean *push_in,
                       GtkWidget *widget)
{
  GtkRequisition        menu_req;
  gint                  root_x;
  gint                  root_y;

  gtk_widget_size_request (GTK_WIDGET (menu), &menu_req);
  gdk_window_get_origin (widget->window, &root_x, &root_y);

  *x = root_x + widget->allocation.x;
  /* TODO find the good way to add spacing to *y */
  *y = root_y + widget->allocation.y;

  if (*y > gdk_screen_height () - menu_req.height)
    *y = gdk_screen_height () - menu_req.height;
  else if (*y < 0)
    *y = 0;

  *push_in = FALSE;
}

static void
xfmpc_extended_interface_action_statusbar_changed (GtkToggleAction *action,
                                                   XfmpcExtendedInterface *extended_interface)
{
  XfmpcExtendedInterfacePrivate *priv = XFMPC_EXTENDED_INTERFACE (extended_interface)->priv;

  gboolean active;

  active = gtk_toggle_action_get_active (action);
  
  if (!active && priv->statusbar != NULL)
    {
      gtk_widget_destroy (priv->statusbar);
      priv->statusbar = NULL;
    }
  else if (active && priv->statusbar == NULL)
    {
      priv->statusbar = xfmpc_statusbar_new ();
      gtk_widget_show (priv->statusbar);
      gtk_box_pack_start (GTK_BOX (extended_interface), priv->statusbar, FALSE, FALSE, 2);
    }
}

static void
cb_playlist_changed (XfmpcExtendedInterface *extended_interface)
{
  XfmpcExtendedInterfacePrivate *priv = XFMPC_EXTENDED_INTERFACE (extended_interface)->priv;

  if (priv->statusbar == NULL)
    return;

  xfmpc_extended_interface_update_statusbar (extended_interface);
}

static void
xfmpc_extended_interface_update_statusbar (XfmpcExtendedInterface *extended_interface)
{
  XfmpcExtendedInterfacePrivate *priv = XFMPC_EXTENDED_INTERFACE (extended_interface)->priv;
  gchar    *text;
  gint      seconds, length;

  if (priv->statusbar == NULL)
    return;

  length = xfmpc_mpdclient_playlist_get_length (extended_interface->mpdclient);
  seconds = xfmpc_mpdclient_playlist_get_total_time (extended_interface->mpdclient);

  if (seconds / 3600 > 0)
    text = g_strdup_printf (_("%d songs, %d hours and %d minutes"), length, seconds / 3600, (seconds / 60) % 60);
  else
    text = g_strdup_printf (_("%d songs, %d minutes"), length, (seconds / 60) % 60);

  g_object_set (G_OBJECT (priv->statusbar), "text", text, NULL);
  g_free (text);
}

static void
cb_show_statusbar_changed (XfmpcExtendedInterface *extended_interface,
                           GParamSpec *pspec)
{
  XfmpcExtendedInterfacePrivate *priv = extended_interface->priv;
  gboolean active;
  GtkAction *action;

  action = gtk_action_group_get_action (priv->action_group, "view-statusbar");
  g_object_get (extended_interface->preferences, "show-statusbar", &active, NULL);

  gtk_toggle_action_set_active (GTK_TOGGLE_ACTION (action), active);
  xfmpc_extended_interface_update_statusbar (extended_interface);
}

