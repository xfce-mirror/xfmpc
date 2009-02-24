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

#ifdef HAVE_STDLIB_H
#include <stdlib.h>
#endif

#ifdef HAVE_STRING_H
#include <string.h>
#endif

#include <gtk/gtk.h>
#include <libxfcegui4/libxfcegui4.h>
#include <libxfce4util/libxfce4util.h>

#include "preferences.h"
#include "mpdclient.h"
#include "main-ui.h"
#include "interface.h"
#include "extended-interface.h"

#define BORDER 4



static gboolean         cb_window_state_event                       (GtkWidget *window,
                                                                     GdkEventWindowState *event);
static gboolean         cb_window_closed                            (GtkWidget *window,
                                                                     GdkEvent *event);
static void             action_close                                (GtkAction *action,
                                                                     GtkWidget *window);
static void             action_previous                             (GtkAction *action,
                                                                     GtkWidget *window);
static void             action_pp                                   (GtkAction *action,
                                                                     GtkWidget *window);
static void             action_stop                                 (GtkAction *action,
                                                                     GtkWidget *window);
static void             action_next                                 (GtkAction *action,
                                                                     GtkWidget *window);
static void             action_volume                               (GtkAction *action,
                                                                     GtkWidget *window);



static const GtkActionEntry action_entries[] =
{
  { "quit", NULL, "", "<control>q", NULL, G_CALLBACK (action_close), },

  { "previous", NULL, "", "<control>b", NULL, G_CALLBACK (action_previous), },
  { "pp", NULL, "", "<control>p", NULL, G_CALLBACK (action_pp), },
  { "stop", NULL, "", "<control>s", NULL, G_CALLBACK (action_stop), },
  { "next", NULL, "", "<control>f", NULL, G_CALLBACK (action_next), },
  { "volume", NULL, "", "<control>v", NULL, G_CALLBACK (action_volume), },
};



static void
transform_string_to_int (const GValue *src,
                         GValue *dst)
{
  g_value_set_int (dst, (gint) strtol (g_value_get_string (src), NULL, 10));
}

static void
transform_string_to_boolean (const GValue *src,
                             GValue *dst)
{
  g_value_set_boolean (dst, (gboolean) strcmp (g_value_get_string (src), "FALSE") != 0);
}



int
main (int argc, char *argv[])
{
  xfce_textdomain (GETTEXT_PACKAGE, PACKAGE_LOCALE_DIR, "UTF-8");

  gtk_init (&argc, &argv);

  g_value_register_transform_func (G_TYPE_STRING, G_TYPE_INT, transform_string_to_int);
  g_value_register_transform_func (G_TYPE_STRING, G_TYPE_BOOLEAN, transform_string_to_boolean);

  gtk_window_set_default_icon_name ("xfmpc");

  /* Read window preferences */
  gint posx, posy;
  gint width, height;
  gboolean sticky;

  XfmpcPreferences *preferences = xfmpc_preferences_get ();
  g_object_get (G_OBJECT (preferences),
                "last-window-posx", &posx,
                "last-window-posy", &posy,
                "last-window-width", &width,
                "last-window-height", &height,
                "last-window-state-sticky", &sticky,
                NULL);

  /* Window */
  GtkWidget *window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  gtk_window_set_icon_name (GTK_WINDOW (window), "stock_volume");
  gtk_window_set_title (GTK_WINDOW (window), _("Xfmpc"));
  gtk_window_set_default_size (GTK_WINDOW (window), 330, 330);
  g_signal_connect (G_OBJECT (window), "delete-event", G_CALLBACK (cb_window_closed), NULL);
  g_signal_connect (G_OBJECT (window), "window-state-event", G_CALLBACK (cb_window_state_event), NULL);

  GtkWidget *vbox = gtk_vbox_new (FALSE, 0);
  gtk_container_add (GTK_CONTAINER (window), vbox);

  if (G_LIKELY (posx != -1 && posy != -1))
    gtk_window_move (GTK_WINDOW (window), posx, posy);
  if (G_LIKELY (width != -1 && height != -1))
    gtk_window_set_default_size (GTK_WINDOW (window), width, height);
  if (sticky == TRUE)
    gtk_window_stick (GTK_WINDOW (window));

  /* Interface */
  GtkWidget *interface = xfmpc_interface_new ();
  g_object_set_data (G_OBJECT (window), "XfmpcInterface", interface);
  gtk_box_pack_start (GTK_BOX (vbox), interface, FALSE, FALSE, BORDER);

  /* Separator */
  GtkWidget *separator = gtk_hseparator_new ();
  gtk_box_pack_start (GTK_BOX (vbox), separator, FALSE, FALSE, 0);

  gtk_widget_show_all (window);

  /* ExtendedInterface */
  GtkWidget *extended_interface = xfmpc_extended_interface_new ();
  gtk_box_pack_start (GTK_BOX (vbox), extended_interface, TRUE, TRUE, 0);

  gtk_widget_show_all (window);

  /* Accelerators */
  GtkUIManager *ui_manager = gtk_ui_manager_new ();

  GtkActionGroup *action_group = gtk_action_group_new ("Main");
  gtk_action_group_add_actions (action_group, action_entries,
                                G_N_ELEMENTS (action_entries),
                                GTK_WIDGET (window));
  gtk_ui_manager_insert_action_group (ui_manager, action_group, 0);
  gtk_ui_manager_add_ui_from_string (ui_manager, main_ui, main_ui_length, NULL);

  GtkAccelGroup *accel_group = gtk_ui_manager_get_accel_group (ui_manager);
  gtk_window_add_accel_group (GTK_WINDOW (window), accel_group);

  /* Start teh app */
  g_object_unref (preferences);
  gtk_main ();

  return 0;
}



static gboolean
cb_window_state_event (GtkWidget *window,
                       GdkEventWindowState *event)
{
  if (G_UNLIKELY (event->type != GDK_WINDOW_STATE))
    return FALSE;

  /**
   * Hiding the top level window will unstick it too, and send a
   * window-state-event signal, so here we take the value only if
   * the window is visible
   **/
  if (event->changed_mask & GDK_WINDOW_STATE_STICKY && GTK_WIDGET_VISIBLE (window))
    {
      gboolean sticky = ((gboolean) event->new_window_state & GDK_WINDOW_STATE_STICKY) == FALSE ? FALSE : TRUE;
      XfmpcPreferences *preferences = xfmpc_preferences_get ();
      g_object_set (G_OBJECT (preferences),
                    "last-window-state-sticky", sticky,
                    NULL);
      g_object_unref (preferences);
    }

  return FALSE;
}

static gboolean
cb_window_closed (GtkWidget *window,
                  GdkEvent *event)
{
  gint posx, posy;
  gint width, height;
  gtk_window_get_position (GTK_WINDOW (window), &posx, &posy);
  gtk_window_get_size (GTK_WINDOW (window), &width, &height);

  XfmpcPreferences *preferences = xfmpc_preferences_get ();
  g_object_set (G_OBJECT (preferences),
                "last-window-posx", posx,
                "last-window-posy", posy,
                "last-window-width", width,
                "last-window-height", height,
                NULL);
  g_object_unref (preferences);

  gtk_main_quit ();
  return FALSE;
}



static void
action_close (GtkAction *action,
              GtkWidget *window)
{
  cb_window_closed (window, NULL);
}

static void
action_previous (GtkAction *action,
                 GtkWidget *window)
{
  XfmpcInterface *interface = g_object_get_data (G_OBJECT (window), "XfmpcInterface");
  xfmpc_mpdclient_previous (interface->mpdclient);
}

static void
action_pp (GtkAction *action,
           GtkWidget *window)
{
  XfmpcInterface *interface = g_object_get_data (G_OBJECT (window), "XfmpcInterface");
  xfmpc_interface_pp_clicked (interface);
}

static void
action_stop (GtkAction *action,
             GtkWidget *window)
{
  XfmpcInterface *interface = g_object_get_data (G_OBJECT (window), "XfmpcInterface");
  xfmpc_mpdclient_stop (interface->mpdclient);
}

static void
action_next (GtkAction *action,
             GtkWidget *window)
{
  XfmpcInterface *interface = g_object_get_data (G_OBJECT (window), "XfmpcInterface");
  xfmpc_mpdclient_next (interface->mpdclient);
}

static void
action_volume (GtkAction *action,
               GtkWidget *window)
{
  XfmpcInterface *interface = g_object_get_data (G_OBJECT (window), "XfmpcInterface");
  xfmpc_interface_popup_volume (interface);
}

