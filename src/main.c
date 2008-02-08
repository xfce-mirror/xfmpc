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

#ifdef HAVE_STDLIB_H
#include <stdlib.h>
#endif

#ifdef HAVE_STRING_H
#include <string.h>
#endif

#include <gtk/gtk.h>
#include <libxfcegui4/libxfcegui4.h>
#include <libxfce4util/libxfce4util.h>

#include "interface.h"



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

  GtkWidget *interface = xfmpc_interface_new ();

  gtk_widget_show_all (interface);

  gtk_main ();

  return 0;
}

