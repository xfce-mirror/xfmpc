/*
 *  Copyright (c) 2009 Mike Massonnet <mmassonnet@xfce.org>
 *  Copyright (c) 2009 Vincent Legout <vincent@xfce.org>
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

#include <glib.h>
#include <glib-object.h>
#include <gtk/gtk.h>
#include <stdlib.h>
#include <string.h>


#define XFMPC_TYPE_STATUSBAR (xfmpc_statusbar_get_type ())
#define XFMPC_STATUSBAR(obj) (G_TYPE_CHECK_INSTANCE_CAST ((obj), XFMPC_TYPE_STATUSBAR, XfmpcStatusbar))
#define XFMPC_STATUSBAR_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST ((klass), XFMPC_TYPE_STATUSBAR, XfmpcStatusbarClass))
#define XFMPC_IS_STATUSBAR(obj) (G_TYPE_CHECK_INSTANCE_TYPE ((obj), XFMPC_TYPE_STATUSBAR))
#define XFMPC_IS_STATUSBAR_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), XFMPC_TYPE_STATUSBAR))
#define XFMPC_STATUSBAR_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj), XFMPC_TYPE_STATUSBAR, XfmpcStatusbarClass))

typedef struct _XfmpcStatusbar XfmpcStatusbar;
typedef struct _XfmpcStatusbarClass XfmpcStatusbarClass;
typedef struct _XfmpcStatusbarPrivate XfmpcStatusbarPrivate;

struct _XfmpcStatusbar {
	GtkStatusbar parent_instance;
	XfmpcStatusbarPrivate * priv;
};

struct _XfmpcStatusbarClass {
	GtkStatusbarClass parent_class;
};

struct _XfmpcStatusbarPrivate {
	guint context_id;
	char* _text;
};


static gpointer xfmpc_statusbar_parent_class = NULL;

GType xfmpc_statusbar_get_type (void);
#define XFMPC_STATUSBAR_GET_PRIVATE(o) (G_TYPE_INSTANCE_GET_PRIVATE ((o), XFMPC_TYPE_STATUSBAR, XfmpcStatusbarPrivate))
enum  {
	XFMPC_STATUSBAR_DUMMY_PROPERTY,
	XFMPC_STATUSBAR_TEXT
};
XfmpcStatusbar* xfmpc_statusbar_new (void);
XfmpcStatusbar* xfmpc_statusbar_construct (GType object_type);
XfmpcStatusbar* xfmpc_statusbar_new (void);
void xfmpc_statusbar_set_text (XfmpcStatusbar* self, const char* value);
static GObject * xfmpc_statusbar_constructor (GType type, guint n_construct_properties, GObjectConstructParam * construct_properties);
static void xfmpc_statusbar_finalize (GObject* obj);
static void xfmpc_statusbar_set_property (GObject * object, guint property_id, const GValue * value, GParamSpec * pspec);



XfmpcStatusbar* xfmpc_statusbar_construct (GType object_type) {
	XfmpcStatusbar * self;
	self = g_object_newv (object_type, 0, NULL);
	return self;
}


XfmpcStatusbar* xfmpc_statusbar_new (void) {
	return xfmpc_statusbar_construct (XFMPC_TYPE_STATUSBAR);
}


void xfmpc_statusbar_set_text (XfmpcStatusbar* self, const char* value) {
	char* _tmp1_;
	const char* _tmp0_;
	g_return_if_fail (self != NULL);
	_tmp1_ = NULL;
	_tmp0_ = NULL;
	self->priv->_text = (_tmp1_ = (_tmp0_ = value, (_tmp0_ == NULL) ? NULL : g_strdup (_tmp0_)), self->priv->_text = (g_free (self->priv->_text), NULL), _tmp1_);
	gtk_statusbar_pop ((GtkStatusbar*) self, self->priv->context_id);
	gtk_statusbar_push ((GtkStatusbar*) self, self->priv->context_id, self->priv->_text);
	g_object_notify ((GObject *) self, "text");
}


static GObject * xfmpc_statusbar_constructor (GType type, guint n_construct_properties, GObjectConstructParam * construct_properties) {
	GObject * obj;
	XfmpcStatusbarClass * klass;
	GObjectClass * parent_class;
	XfmpcStatusbar * self;
	klass = XFMPC_STATUSBAR_CLASS (g_type_class_peek (XFMPC_TYPE_STATUSBAR));
	parent_class = G_OBJECT_CLASS (g_type_class_peek_parent (klass));
	obj = parent_class->constructor (type, n_construct_properties, construct_properties);
	self = XFMPC_STATUSBAR (obj);
	{
		gtk_statusbar_set_has_resize_grip ((GtkStatusbar*) self, FALSE);
		self->priv->context_id = gtk_statusbar_get_context_id ((GtkStatusbar*) self, "Main text");
	}
	return obj;
}


static void xfmpc_statusbar_class_init (XfmpcStatusbarClass * klass) {
	xfmpc_statusbar_parent_class = g_type_class_peek_parent (klass);
	g_type_class_add_private (klass, sizeof (XfmpcStatusbarPrivate));
	G_OBJECT_CLASS (klass)->set_property = xfmpc_statusbar_set_property;
	G_OBJECT_CLASS (klass)->constructor = xfmpc_statusbar_constructor;
	G_OBJECT_CLASS (klass)->finalize = xfmpc_statusbar_finalize;
	g_object_class_install_property (G_OBJECT_CLASS (klass), XFMPC_STATUSBAR_TEXT, g_param_spec_string ("text", "text", "text", NULL, G_PARAM_STATIC_NAME | G_PARAM_STATIC_NICK | G_PARAM_STATIC_BLURB | G_PARAM_WRITABLE));
}


static void xfmpc_statusbar_instance_init (XfmpcStatusbar * self) {
	self->priv = XFMPC_STATUSBAR_GET_PRIVATE (self);
}


static void xfmpc_statusbar_finalize (GObject* obj) {
	XfmpcStatusbar * self;
	self = XFMPC_STATUSBAR (obj);
	self->priv->_text = (g_free (self->priv->_text), NULL);
	G_OBJECT_CLASS (xfmpc_statusbar_parent_class)->finalize (obj);
}


GType xfmpc_statusbar_get_type (void) {
	static GType xfmpc_statusbar_type_id = 0;
	if (xfmpc_statusbar_type_id == 0) {
		static const GTypeInfo g_define_type_info = { sizeof (XfmpcStatusbarClass), (GBaseInitFunc) NULL, (GBaseFinalizeFunc) NULL, (GClassInitFunc) xfmpc_statusbar_class_init, (GClassFinalizeFunc) NULL, NULL, sizeof (XfmpcStatusbar), 0, (GInstanceInitFunc) xfmpc_statusbar_instance_init, NULL };
		xfmpc_statusbar_type_id = g_type_register_static (GTK_TYPE_STATUSBAR, "XfmpcStatusbar", &g_define_type_info, 0);
	}
	return xfmpc_statusbar_type_id;
}


static void xfmpc_statusbar_set_property (GObject * object, guint property_id, const GValue * value, GParamSpec * pspec) {
	XfmpcStatusbar * self;
	self = XFMPC_STATUSBAR (object);
	switch (property_id) {
		case XFMPC_STATUSBAR_TEXT:
		xfmpc_statusbar_set_text (self, g_value_get_string (value));
		break;
		default:
		G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
		break;
	}
}




