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

#ifndef __XFMPC_DBBROWSER_H__
#define __XFMPC_DBBROWSER_H__

G_BEGIN_DECLS

#define XFMPC_TYPE_DBBROWSER                (xfmpc_dbbrowser_get_type())

#define XFMPC_DBBROWSER(obj)                (G_TYPE_CHECK_INSTANCE_CAST ((obj), XFMPC_TYPE_DBBROWSER, XfmpcDbbrowser))
#define XFMPC_DBBROWSER_CLASS(klass)        (G_TYPE_CHECK_CLASS_CAST ((klass), XFMPC_TYPE_DBBROWSER, XfmpcDbbrowserClass))

#define XFMPC_IS_DBBROWSER(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), XFMPC_TYPE_DBBROWSER))
#define XFMPC_IS_DBBROWSER_CLASS(klass)     (G_TYPE_CHECK_CLASS_TYPE ((klass), XFMPC_TYPE_DBBROWSER))

#define XFMPC_DBBROWSER_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS ((obj), XFMPC_TYPE_DBBROWSER, XfmpcDbbrowserClass))

typedef struct _XfmpcDbbrowserClass         XfmpcDbbrowserClass;
typedef struct _XfmpcDbbrowser              XfmpcDbbrowser;
typedef struct _XfmpcDbbrowserPrivate       XfmpcDbbrowserPrivate;

GType           xfmpc_dbbrowser_get_type                       () G_GNUC_CONST;

GtkWidget *     xfmpc_dbbrowser_new                            ();

void            xfmpc_dbbrowser_clear                          (XfmpcDbbrowser *dbbrowser);
void            xfmpc_dbbrowser_append                         (XfmpcDbbrowser *dbbrowser,
                                                                gchar *filename,
                                                                gchar *basename,
                                                                gboolean is_dir,
                                                                gboolean is_bold);

void            xfmpc_dbbrowser_add_selected_rows              (XfmpcDbbrowser *dbbrowser);
void            xfmpc_dbbrowser_replace_with_selected_rows     (XfmpcDbbrowser *dbbrowser);
void            xfmpc_dbbrowser_reload                         (XfmpcDbbrowser *dbbrowser);
void            xfmpc_dbbrowser_search                         (XfmpcDbbrowser *dbbrowser,
                                                                const gchar *query);

void            xfmpc_dbbrowser_set_wdir                       (XfmpcDbbrowser *dbbrowser,
                                                                const gchar *dir);
gboolean        xfmpc_dbbrowser_wdir_is_root                   (XfmpcDbbrowser *dbbrowser);
gchar *         xfmpc_dbbrowser_get_parent_wdir                (XfmpcDbbrowser *dbbrowser);

G_END_DECLS

#endif

