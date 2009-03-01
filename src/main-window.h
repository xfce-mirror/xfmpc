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

#ifndef __XFMPC_MAIN_WINDOW_H__
#define __XFMPC_MAIN_WINDOW_H__

G_BEGIN_DECLS;

#define XFMPC_TYPE_MAIN_WINDOW               (xfmpc_main_window_get_type ())

#define XFMPC_MAIN_WINDOW(obj)               (G_TYPE_CHECK_INSTANCE_CAST ((obj), XFMPC_TYPE_MAIN_WINDOW, XfmpcMainWindow))
#define XFMPC_MAIN_WINDOW_CLASS(klass)       (G_TYPE_CHECK_CLASS_CAST ((klass), XFMPC_TYPE_MAIN_WINDOW, XfmpcMainWindowClass))
#define XFMPC_IS_MAIN_WINDOW(obj)            (G_TYPE_CHECK_INSTANCE_TYPE ((obj), XFMPC_TYPE_MAIN_WINDOW))
#define XFMPC_IS_MAIN_WINDOW_CLASS(klass)    (G_TYPE_CHECK_CLASS_TYPE ((klass), XFMPC_TYPE_MAIN_WINDOW))
#define XFMPC_MAIN_WINDOW_GET_CLASS(obj)     (G_TYPE_INSTANCE_GET_CLASS ((obj), XFMPC_TYPE_MAIN_WINDOW, XfmpcMainWindowClass))

typedef struct _XfmpcMainWindowClass         XfmpcMainWindowClass;
typedef struct _XfmpcMainWindow              XfmpcMainWindow;
typedef struct _XfmpcMainWindowPrivate       XfmpcMainWindowPrivate;

GType          xfmpc_main_window_get_type    (void) G_GNUC_CONST;

GtkWidget*     xfmpc_main_window_new         (void);

G_END_DECLS;

#endif
