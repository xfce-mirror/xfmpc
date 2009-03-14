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

#include "interface.h"
#include "preferences.h"
#include "mpdclient.h"

#define BORDER 4

#define GET_PRIVATE(o) \
    (G_TYPE_INSTANCE_GET_PRIVATE ((o), XFMPC_TYPE_INTERFACE, XfmpcInterfacePrivate))



static void             xfmpc_interface_class_init              (XfmpcInterfaceClass *klass);
static void             xfmpc_interface_init                    (XfmpcInterface *interface);
static void             xfmpc_interface_dispose                 (GObject *object);
static void             xfmpc_interface_finalize                (GObject *object);

static gboolean         xfmpc_interface_refresh                 (XfmpcInterface *interface);

static gboolean         xfmpc_interface_reconnect               (XfmpcInterface *interface);

static void             cb_song_changed                         (XfmpcInterface *interface);

static void             cb_pp_changed                           (XfmpcInterface *interface,
                                                                 gboolean is_playing);
static void             cb_time_changed                         (XfmpcInterface *interface,
                                                                 gint time,
                                                                 gint total_time);
static void             cb_volume_changed                       (XfmpcInterface *interface,
                                                                 gint volume);
static void             cb_stopped                              (XfmpcInterface *interface);



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
      xfmpc_interface_type = g_type_register_static (GTK_TYPE_VBOX, "XfmpcInterface", &xfmpc_interface_info, 0);
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
  XfmpcInterfacePrivate *priv = interface->priv = GET_PRIVATE (interface);

  gtk_container_set_border_width (GTK_CONTAINER (interface), BORDER);
  interface->preferences = xfmpc_preferences_get ();
  interface->mpdclient = xfmpc_mpdclient_get ();
  priv->refresh_title = TRUE; /* bug #4975 */

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
  GtkAdjustment* adjustment = gtk_scale_button_get_adjustment (GTK_SCALE_BUTTON (control));
  adjustment->upper *= 100;
  adjustment->step_increment *= 100;
  adjustment->page_increment *= 100;

  GtkWidget *progress_box = gtk_event_box_new ();
  control = priv->progress_bar = gtk_progress_bar_new ();
  gtk_progress_bar_set_text (GTK_PROGRESS_BAR (control), "0:00 / 0:00");
  gtk_progress_bar_set_fraction (GTK_PROGRESS_BAR (control), 1.0);
  gtk_container_add (GTK_CONTAINER (progress_box), control);

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
  GtkWidget *box = gtk_hbox_new (FALSE, 0);
  gtk_box_pack_start (GTK_BOX (interface), box, FALSE, FALSE, 0);
  gtk_box_pack_start (GTK_BOX (box), priv->button_prev, FALSE, FALSE, 0);
  gtk_box_pack_start (GTK_BOX (box), priv->button_pp, FALSE, FALSE, 0);
  gtk_box_pack_start (GTK_BOX (box), priv->button_next, FALSE, FALSE, 0);
  gtk_box_pack_start (GTK_BOX (box), progress_box, TRUE, TRUE, BORDER);
  gtk_box_pack_start (GTK_BOX (box), priv->button_volume, FALSE, FALSE, 0);

  box = gtk_vbox_new (FALSE, 0);
  gtk_box_pack_start (GTK_BOX (interface), box, FALSE, TRUE, 0);
  gtk_container_add (GTK_CONTAINER (box), priv->title);
  gtk_container_add (GTK_CONTAINER (box), priv->subtitle);

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

  g_signal_connect_swapped (interface->mpdclient, "connected",
                            G_CALLBACK (xfmpc_interface_reconnect), interface);
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
  return g_object_new (XFMPC_TYPE_INTERFACE, 
                       "spacing", BORDER, NULL);
}

void
xfmpc_interface_set_title (XfmpcInterface *interface,
                           const gchar *title)
{
  XfmpcInterfacePrivate *priv = XFMPC_INTERFACE (interface)->priv;

  gtk_label_set_text (GTK_LABEL (priv->title), title);
}

void
xfmpc_interface_set_subtitle (XfmpcInterface *interface,
                              const gchar *subtitle)
{
  XfmpcInterfacePrivate *priv = XFMPC_INTERFACE (interface)->priv;

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
  XfmpcInterfacePrivate *priv = XFMPC_INTERFACE (interface)->priv;

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
  XfmpcInterfacePrivate *priv = XFMPC_INTERFACE (interface)->priv;

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
  xfmpc_mpdclient_set_volume (interface->mpdclient, value);
}

void
xfmpc_interface_set_volume (XfmpcInterface *interface,
                            guint8 volume)
{
  XfmpcInterfacePrivate *priv = XFMPC_INTERFACE (interface)->priv;

  gtk_scale_button_set_value (GTK_SCALE_BUTTON (priv->button_volume), volume);
}

void
xfmpc_interface_popup_volume (XfmpcInterface *interface)
{
  XfmpcInterfacePrivate *priv = XFMPC_INTERFACE (interface)->priv;

  g_signal_emit_by_name (priv->button_volume, "popup", G_TYPE_NONE);
}

void
xfmpc_interface_set_time (XfmpcInterface *interface,
                          gint time,
                          gint time_total)
{
  XfmpcInterfacePrivate *priv = XFMPC_INTERFACE (interface)->priv;

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
  gtk_progress_bar_set_fraction (GTK_PROGRESS_BAR (priv->progress_bar),
                                 (fraction <= 1.0) ? fraction : 1.0);
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
  XfmpcInterfacePrivate *priv = XFMPC_INTERFACE (interface)->priv;

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
  XfmpcInterfacePrivate *priv = XFMPC_INTERFACE (interface)->priv;

  xfmpc_interface_set_pp (interface, FALSE);
  xfmpc_interface_set_time (interface, 0, 0);
  xfmpc_interface_set_title (interface, _("Stopped"));
  xfmpc_interface_set_subtitle (interface, PACKAGE_STRING);

  priv->refresh_title = TRUE;
}

