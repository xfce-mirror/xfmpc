/*
 *  Copyright (c) 2009 Vincent Legout <vincent@legout.info>
 *  Copyright (c) 2009 Mike Massonnet <mmassonnet@xfce.org>
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

#include "statusbar.h"
#include "mpdclient.h"

#define GET_PRIVATE(o) \
    (G_TYPE_INSTANCE_GET_PRIVATE ((o), XFMPC_TYPE_STATUSBAR, XfmpcStatusbarPrivate))



static void             xfmpc_statusbar_class_init             (XfmpcStatusbarClass *klass);
static void             xfmpc_statusbar_init                   (XfmpcStatusbar *statusbar);
static void             xfmpc_statusbar_finalize               (GObject *object);
static void             xfmpc_statusbar_set_property           (GObject *object,
                                                                guint prop_id,
                                                                const GValue *value,
                                                                GParamSpec *pspec);

static void             cb_playlist_changed                    (XfmpcStatusbar *statusbar);



enum
{
  TEXT = 1,
};



struct _XfmpcStatusbarClass
{
  GtkStatusbarClass         parent;
};

struct _XfmpcStatusbar
{
  GtkStatusbar              parent;
  XfmpcMpdclient           *mpdclient;
  /*<private>*/
  XfmpcStatusbarPrivate    *priv;
};

struct _XfmpcStatusbarPrivate
{
  guint                     context_id;
};



static GObjectClass *parent_class = NULL;



GType
xfmpc_statusbar_get_type (void)
{
  static GType xfmpc_statusbar_type = G_TYPE_INVALID;

  if (G_UNLIKELY (xfmpc_statusbar_type == G_TYPE_INVALID))
    {
      static const GTypeInfo xfmpc_statusbar_info =
      {
        sizeof (XfmpcStatusbarClass),
        NULL,
        NULL,
        (GClassInitFunc) xfmpc_statusbar_class_init,
        NULL,
        NULL,
        sizeof (XfmpcStatusbar),
        0,
        (GInstanceInitFunc) xfmpc_statusbar_init,
        NULL,
      };

      xfmpc_statusbar_type = g_type_register_static (GTK_TYPE_STATUSBAR, "XfmpcStatusbar", &xfmpc_statusbar_info, 0);
    }

  return xfmpc_statusbar_type;
}

static void
xfmpc_statusbar_class_init (XfmpcStatusbarClass *klass)
{
  GObjectClass *gobject_class;

  g_type_class_add_private (klass, sizeof (XfmpcStatusbarPrivate));

  parent_class = g_type_class_peek_parent (klass);

  gobject_class = G_OBJECT_CLASS (klass);
  gobject_class->finalize = xfmpc_statusbar_finalize;
  gobject_class->set_property = xfmpc_statusbar_set_property;

  g_object_class_install_property (gobject_class,
                                   TEXT,
                                   g_param_spec_string ("text",
                                                        "Text",
                                                        "Text in the statusbar",
                                                        NULL,
                                                        G_PARAM_WRITABLE));
}

static void
xfmpc_statusbar_init (XfmpcStatusbar *statusbar)
{
  XfmpcStatusbarPrivate *priv = statusbar->priv = GET_PRIVATE (statusbar);

  statusbar->mpdclient = xfmpc_mpdclient_get ();

  priv->context_id = gtk_statusbar_get_context_id (GTK_STATUSBAR (statusbar), "Main text");
  gtk_statusbar_set_has_resize_grip (GTK_STATUSBAR (statusbar), FALSE);

  /* === Signals === */
  g_signal_connect_swapped (statusbar->mpdclient, "playlist-changed",
                            G_CALLBACK (cb_playlist_changed), statusbar);
}

static void
xfmpc_statusbar_finalize (GObject *object)
{
  XfmpcStatusbar *statusbar = XFMPC_STATUSBAR (object);
  g_object_unref (statusbar->mpdclient);
  G_OBJECT_CLASS (parent_class)->finalize (object);
}

static void
xfmpc_statusbar_set_property (GObject *object,
                              guint prop_id,
                              const GValue *value,
                              GParamSpec   *pspec)
{
  XfmpcStatusbar *statusbar = XFMPC_STATUSBAR (object);

  switch (prop_id)
    {
    case TEXT:
      xfmpc_statusbar_set_text (statusbar, g_value_get_string (value));
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
    }
}

GtkWidget*
xfmpc_statusbar_new (void)
{
  return g_object_new (XFMPC_TYPE_STATUSBAR, NULL);
}

void
xfmpc_statusbar_set_text (XfmpcStatusbar *statusbar,
                          const gchar *text)
{
  XfmpcStatusbarPrivate *priv = statusbar->priv = GET_PRIVATE (statusbar);

  gtk_statusbar_pop (GTK_STATUSBAR (statusbar), priv->context_id);
  gtk_statusbar_push (GTK_STATUSBAR (statusbar), priv->context_id, text);
}

static void
cb_playlist_changed (XfmpcStatusbar *statusbar)
{
  gchar    *text;
  gint      seconds, length;

  length = xfmpc_mpdclient_playlist_get_length (statusbar->mpdclient);
  seconds = xfmpc_mpdclient_playlist_get_total_time (statusbar->mpdclient);

  if (seconds / 3600 > 0)
    text = g_strdup_printf ("%d songs, %d hours and %d minutes", length, seconds / 3600, (seconds / 60) % 60);
  else
    text = g_strdup_printf ("%d songs, %d minutes", length, (seconds / 60) % 60);

  xfmpc_statusbar_set_text (statusbar, text);
  g_free (text);
}

