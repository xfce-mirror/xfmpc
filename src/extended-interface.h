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
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#ifndef __XFMPC_EXTENDED_INTERFACE_H__
#define __XFMPC_EXTENDED_INTERFACE_H__

G_BEGIN_DECLS

typedef enum
{
  XFMPC_EXTENDED_INTERFACE_PLAYLIST,
  XFMPC_EXTENDED_INTERFACE_DBBROWSER,
} XfmpcExtendedInterfaceWidget;

#define XFMPC_TYPE_EXTENDED_INTERFACE               (xfmpc_extended_interface_get_type())

#define XFMPC_EXTENDED_INTERFACE(obj)               (G_TYPE_CHECK_INSTANCE_CAST ((obj), XFMPC_TYPE_EXTENDED_INTERFACE, XfmpcExtendedInterface))
#define XFMPC_EXTENDED_INTERFACE_CLASS(klass)       (G_TYPE_CHECK_CLASS_CAST ((klass), XFMPC_TYPE_EXTENDED_INTERFACE, XfmpcExtendedInterfaceClass))

#define XFMPC_IS_EXTENDED_INTERFACE(obj)            (G_TYPE_CHECK_INSTANCE_TYPE ((obj), XFMPC_TYPE_EXTENDED_INTERFACE))
#define XFMPC_IS_EXTENDED_INTERFACE_CLASS(klass)    (G_TYPE_CHECK_CLASS_TYPE ((klass), XFMPC_TYPE_EXTENDED_INTERFACE))

#define XFMPC_EXTENDED_INTERFACE_GET_CLASS(obj)     (G_TYPE_INSTANCE_GET_CLASS ((obj), XFMPC_TYPE_EXTENDED_INTERFACE, XfmpcExtendedInterfaceClass))

typedef struct _XfmpcExtendedInterfaceClass         XfmpcExtendedInterfaceClass;
typedef struct _XfmpcExtendedInterface              XfmpcExtendedInterface;
typedef struct _XfmpcExtendedInterfacePrivate       XfmpcExtendedInterfacePrivate;

GType                   xfmpc_extended_interface_get_type        () G_GNUC_CONST;

GtkWidget *             xfmpc_extended_interface_new             ();

void                    xfmpc_extended_interface_append_child    (XfmpcExtendedInterface *extended_interface,
                                                                  GtkWidget *child,
                                                                  const gchar *title);

void                    xfmpc_extended_interface_set_active      (XfmpcExtendedInterface *extended_interface,
                                                                  XfmpcExtendedInterfaceWidget active_widget);

G_END_DECLS

#endif

