/*
 *  Copyright (c) 2009 Vincent Legout <vincent@legout.info>
 *
 *  Based on ThunarStatus:
 *  Copyright (c) 2005-2006 Benedikt Meurer <benny@xfce.org>
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

#ifndef __XFMPC_STATUSBAR_H__
#define __XFMPC_STATUSBAR_H__

G_BEGIN_DECLS;

#define XFMPC_TYPE_STATUSBAR               (xfmpc_statusbar_get_type ())

#define XFMPC_STATUSBAR(obj)               (G_TYPE_CHECK_INSTANCE_CAST ((obj), XFMPC_TYPE_STATUSBAR, XfmpcStatusbar))
#define XFMPC_STATUSBAR_CLASS(klass)       (G_TYPE_CHECK_CLASS_CAST ((klass), XFMPC_TYPE_STATUSBAR, XfmpcStatusbarClass))
#define XFMPC_IS_STATUSBAR(obj)            (G_TYPE_CHECK_INSTANCE_TYPE ((obj), XFMPC_TYPE_STATUSBAR))
#define XFMPC_IS_STATUSBAR_CLASS(klass)    (G_TYPE_CHECK_CLASS_TYPE ((klass), XFMPC_TYPE_STATUSBAR))
#define XFMPC_STATUSBAR_GET_CLASS(obj)     (G_TYPE_INSTANCE_GET_CLASS ((obj), XFMPC_TYPE_STATUSBAR, XfmpcStatusbarClass))

typedef struct _XfmpcStatusbarClass        XfmpcStatusbarClass;
typedef struct _XfmpcStatusbar             XfmpcStatusbar;
typedef struct _XfmpcStatusbarPrivate      XfmpcStatusbarPrivate;

GType          xfmpc_statusbar_get_type    (void) G_GNUC_CONST;

GtkWidget*     xfmpc_statusbar_new         (void);

void           xfmpc_statusbar_set_text    (XfmpcStatusbar *statusbar,
                                            const gchar *text);

G_END_DECLS;

#endif

