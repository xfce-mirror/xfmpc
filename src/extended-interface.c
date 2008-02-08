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

#include "extended-interface.h"

#define BORDER 4

#define XFMPC_EXTENDED_INTERFACE_GET_PRIVATE(o) \
    (G_TYPE_INSTANCE_GET_PRIVATE ((o), XFMPC_TYPE_EXTENDED_INTERFACE, XfmpcExtendedInterfacePrivate))



/* List store identifiers */
enum
{
  COLUMN_STRING,
  COLUMN_POINTER,
  N_COLUMNS,
};



static void             xfmpc_extended_interface_class_init (XfmpcExtendedInterfaceClass *klass);
static void             xfmpc_extended_interface_init       (XfmpcExtendedInterface *extended_interface);
static void             xfmpc_extended_interface_dispose    (GObject *object);
static void             xfmpc_extended_interface_finalize   (GObject *object);

static void             cb_xfmpc_extended_interface_combobox_changed (GtkComboBox *widget,
                                                             XfmpcExtendedInterface *extended_interface);



struct _XfmpcExtendedInterfaceClass
{
  GtkVBoxClass                      parent_class;
};

struct _XfmpcExtendedInterface
{
  GtkVBox                           parent;
  XfmpcExtendedInterfacePrivate    *priv;
};

struct _XfmpcExtendedInterfacePrivate
{
  GtkListStore                     *list_store;
  GtkWidget                        *combobox;
  GtkWidget                        *notebook;
};



static GObjectClass *parent_class = NULL;



GType
xfmpc_extended_interface_get_type ()
{
  static GType xfmpc_extended_interface_type = G_TYPE_INVALID;

  if (G_UNLIKELY (xfmpc_extended_interface_type == G_TYPE_INVALID))
    {
      static const GTypeInfo xfmpc_extended_interface_info =
        {
          sizeof (XfmpcExtendedInterfaceClass),
          (GBaseInitFunc) NULL,
          (GBaseFinalizeFunc) NULL,
          (GClassInitFunc) xfmpc_extended_interface_class_init,
          (GClassFinalizeFunc) NULL,
          NULL,
          sizeof (XfmpcExtendedInterface),
          0,
          (GInstanceInitFunc) xfmpc_extended_interface_init,
          NULL
        };
      xfmpc_extended_interface_type = g_type_register_static (GTK_TYPE_VBOX, "XfmpcExtendedInterface", &xfmpc_extended_interface_info, 0);
    }

  return xfmpc_extended_interface_type;
}



static void
xfmpc_extended_interface_class_init (XfmpcExtendedInterfaceClass *klass)
{
  GObjectClass *gobject_class;

  g_type_class_add_private (klass, sizeof (XfmpcExtendedInterfacePrivate));

  parent_class = g_type_class_peek_parent (klass);

  gobject_class = G_OBJECT_CLASS (klass);
  gobject_class->dispose = xfmpc_extended_interface_dispose;
  gobject_class->finalize = xfmpc_extended_interface_finalize;
}

static void
xfmpc_extended_interface_init (XfmpcExtendedInterface *extended_interface)
{
  XfmpcExtendedInterfacePrivate *priv = XFMPC_EXTENDED_INTERFACE_GET_PRIVATE (extended_interface);

  /* Combo box */
  priv->list_store = gtk_list_store_new (N_COLUMNS,
                                         G_TYPE_STRING,
                                         G_TYPE_POINTER);

  priv->combobox = gtk_combo_box_new_with_model (GTK_TREE_MODEL (priv->list_store));

  GtkCellRenderer *cell = gtk_cell_renderer_text_new ();
  gtk_cell_layout_pack_start (GTK_CELL_LAYOUT (priv->combobox), cell, TRUE);
  gtk_cell_layout_set_attributes (GTK_CELL_LAYOUT (priv->combobox),
                                  cell, "text", COLUMN_STRING,
                                  NULL);

  /* Notebook */
  priv->notebook = gtk_notebook_new ();
  gtk_notebook_set_show_tabs (GTK_NOTEBOOK (priv->notebook), FALSE);

  /* Extended interface widgets */
  GtkWidget *child = gtk_label_new ("Hello world!");
  xfmpc_extended_interface_append_child (extended_interface, child, "Hello world!");

  child = gtk_label_new ("Good bye world!");
  xfmpc_extended_interface_append_child (extended_interface, child, "Good bye world!");

  /* Containers */
  gtk_box_pack_start (GTK_BOX (extended_interface), priv->combobox, FALSE, FALSE, BORDER);
  gtk_box_pack_start (GTK_BOX (extended_interface), priv->notebook, TRUE, TRUE, 0);

  /* Signals */
  g_signal_connect (priv->combobox, "changed",
                    G_CALLBACK (cb_xfmpc_extended_interface_combobox_changed), extended_interface);
}

static void
xfmpc_extended_interface_dispose (GObject *object)
{
  (*G_OBJECT_CLASS (parent_class)->dispose) (object);
}

static void
xfmpc_extended_interface_finalize (GObject *object)
{
  (*G_OBJECT_CLASS (parent_class)->finalize) (object);
}



GtkWidget*
xfmpc_extended_interface_new ()
{
  return g_object_new (XFMPC_TYPE_EXTENDED_INTERFACE, NULL);
}

void
xfmpc_extended_interface_append_child (XfmpcExtendedInterface *extended_interface,
                                       GtkWidget *child,
                                       const gchar *title)
{
  XfmpcExtendedInterfacePrivate *priv = XFMPC_EXTENDED_INTERFACE_GET_PRIVATE (extended_interface);

  GtkTreeIter iter;

  gtk_list_store_append (priv->list_store, &iter);
  gtk_list_store_set (priv->list_store, &iter,
                      COLUMN_STRING, title,
                      COLUMN_POINTER, child,
                      -1);

  if (gtk_combo_box_get_active(GTK_COMBO_BOX (priv->combobox)) == -1)
    gtk_combo_box_set_active (GTK_COMBO_BOX (priv->combobox), 0);

  gtk_notebook_append_page (GTK_NOTEBOOK (priv->notebook), child, NULL);
  gtk_notebook_set_tab_label_packing (GTK_NOTEBOOK (priv->notebook), child, TRUE, TRUE, GTK_PACK_START);
}

static void
cb_xfmpc_extended_interface_combobox_changed (GtkComboBox *widget,
                                              XfmpcExtendedInterface *extended_interface)
{
  XfmpcExtendedInterfacePrivate *priv = XFMPC_EXTENDED_INTERFACE_GET_PRIVATE (extended_interface);

  GtkWidget            *child;
  GtkTreeIter           iter;
  gint                  i;

  if (gtk_combo_box_get_active_iter (widget, &iter) == FALSE)
    return;

  gtk_tree_model_get (GTK_TREE_MODEL (priv->list_store), &iter,
                      COLUMN_POINTER, &child,
                      -1);
  g_return_if_fail (G_LIKELY (GTK_IS_WIDGET (child)));

  i = gtk_notebook_page_num (GTK_NOTEBOOK (priv->notebook), child);
  gtk_notebook_set_current_page (GTK_NOTEBOOK (priv->notebook), i);
}

