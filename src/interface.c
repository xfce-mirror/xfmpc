/*
 *  Copyright (c) 2008 Mike Massonnet <mmassonnet@xfce.org>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Library General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <gtk/gtk.h>
#include <libxfcegui4/libxfcegui4.h>
#include <libxfce4util/libxfce4util.h>

#include "interface.h"
#include "interface-ui.h"
#include "extended-interface.h"
#include "preferences.h"
#include "mpdclient.h"

#define BORDER 4

#define XFMPC_INTERFACE_GET_PRIVATE(o) \
    (G_TYPE_INSTANCE_GET_PRIVATE ((o), XFMPC_TYPE_INTERFACE, XfmpcInterfacePrivate))



static void             xfmpc_interface_class_init              (XfmpcInterfaceClass *klass);
static void             xfmpc_interface_init                    (XfmpcInterface *interface);
static void             xfmpc_interface_dispose                 (GObject *object);
static void             xfmpc_interface_finalize                (GObject *object);

static gboolean         xfmpc_interface_refresh                 (XfmpcInterface *interface);

static gboolean         xfmpc_interface_reconnect               (XfmpcInterface *interface);

static gboolean         xfmpc_interface_state_event             (XfmpcInterface *interface,
                                                                 GdkEventWindowState *event);
static gboolean         xfmpc_interface_closed                  (XfmpcInterface *interface,
                                                                 GdkEvent *event);
static void             xfmpc_interface_action_previous         (GtkAction *action,
                                                                 XfmpcInterface *interface);
static void             xfmpc_interface_action_pp               (GtkAction *action,
                                                                 XfmpcInterface *interface);
static void             xfmpc_interface_action_stop             (GtkAction *action,
                                                                 XfmpcInterface *interface);
static void             xfmpc_interface_action_next             (GtkAction *action,
                                                                 XfmpcInterface *interface);
static void             xfmpc_interface_action_volume           (GtkAction *action,
                                                                 XfmpcInterface *interface);
static void             cb_song_changed                         (XfmpcInterface *interface);

static void             cb_pp_changed                           (XfmpcInterface *interface,
                                                                 gboolean is_playing);
static void             cb_time_changed                         (XfmpcInterface *interface,
                                                                 gint time,
                                                                 gint total_time);
static void             cb_volume_changed                       (XfmpcInterface *interface,
                                                                 gint volume);
static void             cb_stopped                              (XfmpcInterface *interface);




struct _XfmpcInterfaceClass
{
  GtkWindowClass            parent_class;
};

struct _XfmpcInterface
{
  GtkWindow                 parent;
  XfmpcInterfacePrivate    *priv;
  GtkWidget                *extended_interface;
  XfmpcPreferences         *preferences;
  XfmpcMpdclient           *mpdclient;
};

struct _XfmpcInterfacePrivate
{
  GtkWidget                *button_prev;
  GtkWidget                *button_pp; /* play/pause */
  GtkWidget                *button_next;
  GtkWidget                *button_volume;
  GtkWidget                *progress_bar; /* position in song */
  GtkWidget                *title;
  GtkWidget                *subtitle;
  gboolean                  refresh_title;
};



static const GtkActionEntry action_entries[] =
{
  { "previous", NULL, "", "<control>b", NULL, G_CALLBACK (xfmpc_interface_action_previous), },
  { "pp", NULL, "", "<control>p", NULL, G_CALLBACK (xfmpc_interface_action_pp), },
  { "stop", NULL, "", "<control>s", NULL, G_CALLBACK (xfmpc_interface_action_stop), },
  { "next", NULL, "", "<control>f", NULL, G_CALLBACK (xfmpc_interface_action_next), },
  { "volume", NULL, "", "<control>v", NULL, G_CALLBACK (xfmpc_interface_action_volume), },
  { "quit", NULL, "", "<control>q", NULL, G_CALLBACK (gtk_main_quit), },
};



static GObjectClass *parent_class = NULL;



GType
xfmpc_interface_get_type ()
{
  static GType xfmpc_interface_type = G_TYPE_INVALID;

  if (G_UNLIKELY (xfmpc_interface_type == G_TYPE_INVALID))
    {
      static const GTypeInfo xfmpc_interface_info =
        {
          sizeof (XfmpcInterfaceClass),
          (GBaseInitFunc) NULL,
          (GBaseFinalizeFunc) NULL,
          (GClassInitFunc) xfmpc_interface_class_init,
          (GClassFinalizeFunc) NULL,
          NULL,
          sizeof (XfmpcInterface),
          0,
          (GInstanceInitFunc) xfmpc_interface_init,
          NULL
        };
      xfmpc_interface_type = g_type_register_static (GTK_TYPE_WINDOW, "XfmpcInterface", &xfmpc_interface_info, 0);
    }

  return xfmpc_interface_type;
}



static void
xfmpc_interface_class_init (XfmpcInterfaceClass *klass)
{
  GObjectClass *gobject_class;

  g_type_class_add_private (klass, sizeof (XfmpcInterfacePrivate));

  parent_class = g_type_class_peek_parent (klass);

  gobject_class = G_OBJECT_CLASS (klass);
  gobject_class->dispose = xfmpc_interface_dispose;
  gobject_class->finalize = xfmpc_interface_finalize;
}

static void
xfmpc_interface_init (XfmpcInterface *interface)
{
  XfmpcInterfacePrivate *priv = XFMPC_INTERFACE_GET_PRIVATE (interface);

  interface->preferences = xfmpc_preferences_get ();
  interface->mpdclient = xfmpc_mpdclient_new ();

  /* === Window === */
  gtk_window_set_icon_name (GTK_WINDOW (interface), "stock_volume");
  gtk_window_set_title (GTK_WINDOW (interface), _("Xfmpc"));
  gtk_container_set_border_width (GTK_CONTAINER (interface), BORDER);
  g_signal_connect (G_OBJECT (interface), "delete-event", G_CALLBACK (xfmpc_interface_closed), NULL);
  g_signal_connect (G_OBJECT (interface), "window-state-event", G_CALLBACK (xfmpc_interface_state_event), NULL);

  gint posx, posy;
  gint width, height;
  gboolean sticky;
  g_object_get (G_OBJECT (interface->preferences),
                "last-window-posx", &posx,
                "last-window-posy", &posy,
                "last-window-width", &width,
                "last-window-height", &height,
                "last-window-state-sticky", &sticky,
                NULL);
  if (G_LIKELY (posx != -1 && posy != -1))
    gtk_window_move (GTK_WINDOW (interface), posx, posy);
  if (G_LIKELY (width != -1 && height != -1))
    gtk_window_set_default_size (GTK_WINDOW (interface), width, height);
  if (sticky == TRUE)
    gtk_window_stick (GTK_WINDOW (interface));

  /* === Interface widgets === */
  GtkWidget *image = gtk_image_new_from_stock (GTK_STOCK_MEDIA_PREVIOUS, GTK_ICON_SIZE_BUTTON);
  GtkWidget *control = priv->button_prev = gtk_button_new ();
  gtk_button_set_relief (GTK_BUTTON (control), GTK_RELIEF_NONE);
  gtk_container_add (GTK_CONTAINER (control), image);

  image = gtk_image_new_from_stock (GTK_STOCK_MEDIA_PLAY, GTK_ICON_SIZE_BUTTON);
  control = priv->button_pp = gtk_button_new ();
  gtk_button_set_relief (GTK_BUTTON (control), GTK_RELIEF_NONE);
  gtk_container_add (GTK_CONTAINER (control), image);

  image = gtk_image_new_from_stock (GTK_STOCK_MEDIA_NEXT, GTK_ICON_SIZE_BUTTON);
  control = priv->button_next = gtk_button_new ();
  gtk_button_set_relief (GTK_BUTTON (control), GTK_RELIEF_NONE);
  gtk_container_add (GTK_CONTAINER (control), image);

  control = priv->button_volume = gtk_volume_button_new ();
  gtk_button_set_relief (GTK_BUTTON (control), GTK_RELIEF_NONE);

  GtkWidget *progress_box = gtk_event_box_new ();
  control = priv->progress_bar = gtk_progress_bar_new ();
  gtk_progress_bar_set_text (GTK_PROGRESS_BAR (control), "0:00 / 0:00");
  gtk_progress_bar_set_fraction (GTK_PROGRESS_BAR (control), 1.0);
  gtk_container_add (GTK_CONTAINER (progress_box), control);

  interface->extended_interface = xfmpc_extended_interface_new ();

  /* Title */
  PangoAttrList* attrs = pango_attr_list_new ();
  PangoAttribute* attr = pango_attr_weight_new (PANGO_WEIGHT_BOLD);
  attr->start_index = 0;
  attr->end_index = -1;
  pango_attr_list_insert (attrs, attr);

  attr = pango_attr_scale_new (PANGO_SCALE_X_LARGE);
  attr->start_index = 0;
  attr->end_index = -1;
  pango_attr_list_insert (attrs, attr);

  GtkWidget *label = priv->title = gtk_label_new (_("Not connected"));
  gtk_label_set_attributes (GTK_LABEL (label), attrs);
  gtk_label_set_selectable (GTK_LABEL (label), TRUE);
  gtk_label_set_ellipsize (GTK_LABEL (label), PANGO_ELLIPSIZE_END);
  gtk_misc_set_alignment (GTK_MISC (label), 0, 0.5);

  /* Subtitle */
  attrs = pango_attr_list_new ();
  attr = pango_attr_scale_new (PANGO_SCALE_SMALL);
  attr->start_index = 0;
  attr->end_index = -1;
  pango_attr_list_insert (attrs, attr);

  label = priv->subtitle = gtk_label_new (PACKAGE_STRING);
  gtk_label_set_attributes (GTK_LABEL (label), attrs);
  gtk_label_set_selectable (GTK_LABEL (label), TRUE);
  gtk_label_set_ellipsize (GTK_LABEL (label), PANGO_ELLIPSIZE_END);
  gtk_misc_set_alignment (GTK_MISC (label), 0, 0.5);

  /* === Containers === */
  GtkWidget *vbox = gtk_vbox_new (FALSE, BORDER);
  gtk_container_add (GTK_CONTAINER (interface), vbox);

  GtkWidget *box = gtk_hbox_new (FALSE, 0);
  gtk_box_pack_start (GTK_BOX (vbox), box, FALSE, FALSE, 0);
  gtk_box_pack_start (GTK_BOX (box), priv->button_prev, FALSE, FALSE, 0);
  gtk_box_pack_start (GTK_BOX (box), priv->button_pp, FALSE, FALSE, 0);
  gtk_box_pack_start (GTK_BOX (box), priv->button_next, FALSE, FALSE, 0);
  gtk_box_pack_start (GTK_BOX (box), progress_box, TRUE, TRUE, BORDER);
  gtk_box_pack_start (GTK_BOX (box), priv->button_volume, FALSE, FALSE, 0);

  box = gtk_vbox_new (FALSE, 0);
  gtk_box_pack_start (GTK_BOX (vbox), box, FALSE, TRUE, 0);
  gtk_container_add (GTK_CONTAINER (box), priv->title);
  gtk_container_add (GTK_CONTAINER (box), priv->subtitle);

  gtk_box_pack_start (GTK_BOX (vbox), interface->extended_interface, TRUE, TRUE, 0);

  /* === Accelerators === */
  GtkActionGroup *action_group = gtk_action_group_new ("XfmpcInterface");
  gtk_action_group_add_actions (action_group, action_entries, G_N_ELEMENTS (action_entries), GTK_WINDOW (interface));

  GtkUIManager *ui_manager = gtk_ui_manager_new ();
  gtk_ui_manager_insert_action_group (ui_manager, action_group, 0);
  gtk_ui_manager_add_ui_from_string (ui_manager, xfmpc_interface_ui, xfmpc_interface_ui_length, NULL);

  GtkAccelGroup *accel_group = gtk_ui_manager_get_accel_group (ui_manager);
  gtk_window_add_accel_group (GTK_WINDOW (interface), accel_group);

  /* === Signals === */
  g_signal_connect_swapped (priv->button_prev, "clicked",
                            G_CALLBACK (xfmpc_mpdclient_previous), interface->mpdclient);
  g_signal_connect_swapped (priv->button_pp, "clicked",
                            G_CALLBACK (xfmpc_interface_pp_clicked), interface);
  g_signal_connect_swapped (priv->button_next, "clicked",
                            G_CALLBACK (xfmpc_mpdclient_next), interface->mpdclient);
  g_signal_connect_swapped (priv->button_volume, "value-changed",
                            G_CALLBACK (xfmpc_interface_volume_changed), interface);
  g_signal_connect_swapped (progress_box, "button-press-event",
                            G_CALLBACK (xfmpc_interface_progress_box_press_event), interface);

  g_signal_connect_swapped (interface->mpdclient, "song-changed",
                            G_CALLBACK (cb_song_changed), interface);
  g_signal_connect_swapped (interface->mpdclient, "pp-changed",
                            G_CALLBACK (cb_pp_changed), interface);
  g_signal_connect_swapped (interface->mpdclient, "time-changed",
                            G_CALLBACK (cb_time_changed), interface);
  g_signal_connect_swapped (interface->mpdclient, "volume-changed",
                            G_CALLBACK (cb_volume_changed), interface);
  g_signal_connect_swapped (interface->mpdclient, "stopped",
                            G_CALLBACK (cb_stopped), interface);

  /* === Timeout === */
  g_timeout_add (1000, (GSourceFunc)xfmpc_interface_refresh, interface);
}

static void
xfmpc_interface_dispose (GObject *object)
{
  (*G_OBJECT_CLASS (parent_class)->dispose) (object);
}

static void
xfmpc_interface_finalize (GObject *object)
{
  XfmpcInterface *interface = XFMPC_INTERFACE (object);
  g_object_unref (G_OBJECT (interface->preferences));
  g_object_unref (G_OBJECT (interface->mpdclient));
  (*G_OBJECT_CLASS (parent_class)->finalize) (object);
}



GtkWidget*
xfmpc_interface_new ()
{
  return g_object_new (XFMPC_TYPE_INTERFACE, NULL);
}

void
xfmpc_interface_set_title (XfmpcInterface *interface,
                           const gchar *title)
{
  XfmpcInterfacePrivate *priv = XFMPC_INTERFACE_GET_PRIVATE (interface);

  gtk_label_set_text (GTK_LABEL (priv->title), title);
}

void
xfmpc_interface_set_subtitle (XfmpcInterface *interface,
                              const gchar *subtitle)
{
  XfmpcInterfacePrivate *priv = XFMPC_INTERFACE_GET_PRIVATE (interface);

  gtk_label_set_text (GTK_LABEL (priv->subtitle), subtitle);
}

void
xfmpc_interface_pp_clicked (XfmpcInterface *interface)
{
  if (G_UNLIKELY (!xfmpc_mpdclient_pp (interface->mpdclient)))
    return;
  xfmpc_interface_set_pp (interface, xfmpc_mpdclient_is_playing (interface->mpdclient));
}

void
xfmpc_interface_set_pp (XfmpcInterface *interface,
                        gboolean play)
{
  XfmpcInterfacePrivate *priv = XFMPC_INTERFACE_GET_PRIVATE (interface);

  GtkWidget *image = gtk_bin_get_child (GTK_BIN (priv->button_pp));

  if (play == TRUE)
    gtk_image_set_from_stock (GTK_IMAGE (image), GTK_STOCK_MEDIA_PAUSE, GTK_ICON_SIZE_BUTTON);
  else
    gtk_image_set_from_stock (GTK_IMAGE (image), GTK_STOCK_MEDIA_PLAY, GTK_ICON_SIZE_BUTTON);
}

gboolean
xfmpc_interface_progress_box_press_event (XfmpcInterface *interface,
                                          GdkEventButton *event)
{
  XfmpcInterfacePrivate *priv = XFMPC_INTERFACE_GET_PRIVATE (interface);

  if (G_UNLIKELY (event->type != GDK_BUTTON_PRESS || event->button != 1))
    return FALSE;

  gint time_total = xfmpc_mpdclient_get_total_time (interface->mpdclient);
  if (G_UNLIKELY (time_total < 0))
    return FALSE;

  gdouble time = event->x / priv->progress_bar->allocation.width;
  time *= time_total;

  xfmpc_mpdclient_set_song_time (interface->mpdclient, (guint)time);

  return TRUE;
}

void
xfmpc_interface_volume_changed (XfmpcInterface *interface,
                                gdouble value)
{
  xfmpc_mpdclient_set_volume (interface->mpdclient, (guint8)(value * 100));
}

void
xfmpc_interface_set_volume (XfmpcInterface *interface,
                            guint8 volume)
{
  XfmpcInterfacePrivate *priv = XFMPC_INTERFACE_GET_PRIVATE (interface);

  gtk_scale_button_set_value (GTK_SCALE_BUTTON (priv->button_volume), (gdouble)volume / 100);
}

void
xfmpc_interface_set_time (XfmpcInterface *interface,
                          gint time,
                          gint time_total)
{
  XfmpcInterfacePrivate *priv = XFMPC_INTERFACE_GET_PRIVATE (interface);

  gint                  min, sec, min_total, sec_total;
  gchar                *text;
  gdouble               fraction = 1.0;

  min = time / 60;
  sec = time % 60;

  min_total = time_total / 60;
  sec_total = time_total % 60;

  text = g_strdup_printf ("%d:%02d / %d:%02d", min, sec, min_total, sec_total);
  gtk_progress_bar_set_text (GTK_PROGRESS_BAR (priv->progress_bar), text);
  g_free (text);

  if (G_LIKELY (time_total > 0))
    fraction = (gfloat)time / (gfloat)time_total;
  gtk_progress_bar_set_fraction (GTK_PROGRESS_BAR (priv->progress_bar), fraction);
}

static gboolean
xfmpc_interface_refresh (XfmpcInterface *interface)
{
  if (G_UNLIKELY (xfmpc_mpdclient_connect (interface->mpdclient) == FALSE))
    {
      g_warning ("Failed to connect to MPD");
      xfmpc_mpdclient_disconnect (interface->mpdclient);
      xfmpc_interface_set_pp (interface, FALSE);
      xfmpc_interface_set_time (interface, 0, 0);
      xfmpc_interface_set_volume (interface, 0);
      xfmpc_interface_set_title (interface, _("Not connected"));
      xfmpc_interface_set_subtitle (interface, PACKAGE_STRING);

      /* Start a reconnection timeout and return FALSE to kill the refresh timeout */
      g_timeout_add (15000, (GSourceFunc)xfmpc_interface_reconnect, interface);
      return FALSE;
    }

  xfmpc_mpdclient_update_status (interface->mpdclient);

  return TRUE;
}

static gboolean
xfmpc_interface_reconnect (XfmpcInterface *interface)
{
  if (G_UNLIKELY (xfmpc_mpdclient_connect (interface->mpdclient) == FALSE))
    return TRUE;

  /* Return FALSE to kill the reconnection timeout and start a refresh timeout */
  g_timeout_add (1000, (GSourceFunc)xfmpc_interface_refresh, interface);
  return FALSE;
}

static gboolean
xfmpc_interface_state_event (XfmpcInterface *interface,
                             GdkEventWindowState *event)
{
  if (G_UNLIKELY (event->type != GDK_WINDOW_STATE))
    return FALSE;

  /**
   * Hiding the top level window will unstick it too, and send a
   * window-state-event signal, so here we take the value only if
   * the window is visible
   **/
  if (event->changed_mask & GDK_WINDOW_STATE_STICKY && GTK_WIDGET_VISIBLE (GTK_WIDGET (interface)))
    {
      gboolean sticky = ((gboolean) event->new_window_state & GDK_WINDOW_STATE_STICKY) == FALSE ? FALSE : TRUE;
      g_object_set (G_OBJECT (interface->preferences),
                    "last-window-state-sticky", sticky,
                    NULL);
    }

  return FALSE;
}

static gboolean
xfmpc_interface_closed (XfmpcInterface *interface,
                        GdkEvent *event)
{
  gint posx, posy;
  gint width, height;
  gtk_window_get_position (GTK_WINDOW (interface), &posx, &posy);
  gtk_window_get_size (GTK_WINDOW (interface), &width, &height);

  g_object_set (G_OBJECT (interface->preferences),
                "last-window-posx", posx,
                "last-window-posy", posy,
                "last-window-width", width,
                "last-window-height", height,
                NULL);

  gtk_main_quit ();
  return FALSE;
}

static void
cb_song_changed (XfmpcInterface *interface)
{
  /* title */
  xfmpc_interface_set_title (interface, xfmpc_mpdclient_get_title (interface->mpdclient));

  /* subtitle "by \"artist\" from \"album\" (year)" */
  gchar *text = g_strdup_printf (_("by \"%s\" from \"%s\" (%s)"),
                                 xfmpc_mpdclient_get_artist (interface->mpdclient),
                                 xfmpc_mpdclient_get_album (interface->mpdclient),
                                 xfmpc_mpdclient_get_date (interface->mpdclient));
  /* text = xfmpc_interface_get_subtitle (interface); to avoid "n/a" values, so far I don't care */
  xfmpc_interface_set_subtitle (interface, text);
  g_free (text);
}

static void
cb_pp_changed (XfmpcInterface *interface,
               gboolean is_playing)
{
  XfmpcInterfacePrivate *priv = XFMPC_INTERFACE_GET_PRIVATE (interface);

  xfmpc_interface_set_pp (interface, is_playing);

  if (priv->refresh_title)
    {
      cb_song_changed (interface);
      priv->refresh_title = FALSE;
    }
}

static void
cb_time_changed (XfmpcInterface *interface,
                 gint time,
                 gint total_time)
{
  xfmpc_interface_set_time (interface, time, total_time);
}

static void
cb_volume_changed (XfmpcInterface *interface,
                   gint volume)
{
  xfmpc_interface_set_volume (interface, volume);
}

static void
cb_stopped (XfmpcInterface *interface)
{
  XfmpcInterfacePrivate *priv = XFMPC_INTERFACE_GET_PRIVATE (interface);

  xfmpc_interface_set_pp (interface, FALSE);
  xfmpc_interface_set_time (interface, 0, 0);
  xfmpc_interface_set_title (interface, _("Stopped"));
  xfmpc_interface_set_subtitle (interface, PACKAGE_STRING);

  priv->refresh_title = TRUE;
}



static void
xfmpc_interface_action_previous (GtkAction *action,
                                 XfmpcInterface *interface)
{
  xfmpc_mpdclient_previous (interface->mpdclient);
}

static void
xfmpc_interface_action_pp (GtkAction *action,
                           XfmpcInterface *interface)
{
  xfmpc_interface_pp_clicked (interface);
}

static void
xfmpc_interface_action_stop (GtkAction *action,
                             XfmpcInterface *interface)
{
  xfmpc_mpdclient_stop (interface->mpdclient);
}

static void
xfmpc_interface_action_next (GtkAction *action,
                             XfmpcInterface *interface)
{
  xfmpc_mpdclient_next (interface->mpdclient);
}

static void
xfmpc_interface_action_volume (GtkAction *action,
                               XfmpcInterface *interface)
{
  XfmpcInterfacePrivate *priv = XFMPC_INTERFACE_GET_PRIVATE (interface);

  g_signal_emit_by_name (priv->button_volume, "popup", G_TYPE_NONE);
}

