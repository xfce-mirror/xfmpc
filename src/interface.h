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

#ifndef __XFMPC_INTERFACE_H__
#define __XFMPC_INTERFACE_H__

G_BEGIN_DECLS

#define XFMPC_TYPE_INTERFACE            (xfmpc_interface_get_type())

#define XFMPC_INTERFACE(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), XFMPC_TYPE_INTERFACE, XfmpcInterface))
#define XFMPC_INTERFACE_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), XFMPC_TYPE_INTERFACE, XfmpcInterfaceClass))

#define XFMPC_IS_INTERFACE(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), XFMPC_TYPE_INTERFACE))
#define XFMPC_IS_INTERFACE_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), XFMPC_TYPE_INTERFACE))

#define XFMPC_INTERFACE_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj), XFMPC_TYPE_INTERFACE, XfmpcInterfaceClass))

typedef struct _XfmpcInterfaceClass     XfmpcInterfaceClass;
typedef struct _XfmpcInterface          XfmpcInterface;
typedef struct _XfmpcInterfacePrivate   XfmpcInterfacePrivate;

GType                   xfmpc_interface_get_type                () G_GNUC_CONST;

GtkWidget *             xfmpc_interface_new                     ();

void                    xfmpc_interface_set_title               (XfmpcInterface *interface,
                                                                 const gchar *title);
void                    xfmpc_interface_set_subtitle            (XfmpcInterface *interface,
                                                                 const gchar *subtitle);
void                    xfmpc_interface_pp_clicked              (XfmpcInterface *interface);

void                    xfmpc_interface_set_pp                  (XfmpcInterface *interface,
                                                                 gboolean play);
gboolean                xfmpc_interface_progress_box_press_event (XfmpcInterface *interface,
                                                                 GdkEventButton *event);
void                    xfmpc_interface_volume_changed          (XfmpcInterface *interface,
                                                                 gdouble value);
void                    xfmpc_interface_set_volume              (XfmpcInterface *interface,
                                                                 guint8 volume);
void                    xfmpc_interface_refresh_volume          (XfmpcInterface *interface);

void                    xfmpc_interface_set_time                (XfmpcInterface *interface,
                                                                 gint time,
                                                                 gint time_total);
G_END_DECLS

#endif

