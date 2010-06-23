/*
 * Copyright (C) 2010 Andrea Zagli <azagli@libero.it>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#ifdef HAVE_CONFIG_H
	#include <config.h>
#endif

#include "resource.h"
#include "resource_interface.h"

static void autoz_resource_class_init (AutozResourceClass *class);
static void autoz_resource_init (AutozResource *form);

static void autoz_iresource_interface_init (AutozIResourceIface *iface);

static const gchar *autoz_resource_get_resource_id (AutozIResource *iresource);

static void autoz_resource_set_property (GObject *object,
                               guint property_id,
                               const GValue *value,
                               GParamSpec *pspec);
static void autoz_resource_get_property (GObject *object,
                               guint property_id,
                               GValue *value,
                               GParamSpec *pspec);

#define AUTOZ_RESOURCE_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), AUTOZ_TYPE_RESOURCE, AutozResourcePrivate))

typedef struct _AutozResourcePrivate AutozResourcePrivate;
struct _AutozResourcePrivate
	{
		gchar *resource_id;
	};

G_DEFINE_TYPE_WITH_CODE (AutozResource, autoz_resource, G_TYPE_OBJECT,
                         G_IMPLEMENT_INTERFACE (AUTOZ_TYPE_IRESOURCE,
                                                autoz_iresource_interface_init));

static void
autoz_resource_class_init (AutozResourceClass *class)
{
	GObjectClass *object_class = G_OBJECT_CLASS (class);

	object_class->set_property = autoz_resource_set_property;
	object_class->get_property = autoz_resource_get_property;

	g_type_class_add_private (object_class, sizeof (AutozResourcePrivate));
}

static void
autoz_resource_init (AutozResource *form)
{
	AutozResourcePrivate *priv = AUTOZ_RESOURCE_GET_PRIVATE (form);
}

static void
autoz_iresource_interface_init (AutozIResourceIface *iface)
{
	iface->get_resource_id = autoz_resource_get_resource_id;
}

/**
 * autoz_resource_new:
 * @resource_id:
 *
 * Returns: the newly created #AutozResource object.
 */
AutozResource
*autoz_resource_new (const gchar *resource_id)
{
	AutozResource *resource;
	AutozResourcePrivate *priv;

	resource = AUTOZ_RESOURCE (g_object_new (autoz_resource_get_type (), NULL));

	priv = AUTOZ_RESOURCE_GET_PRIVATE (resource);

	priv->resource_id = g_strdup (resource_id);

	return resource;
}

/* PRIVATE */
static const gchar
*autoz_resource_get_resource_id (AutozIResource *iresource)
{
	AutozResourcePrivate *priv;

	const gchar *ret;

	ret = NULL;

	g_return_val_if_fail (AUTOZ_IS_RESOURCE (iresource), ret);

	priv = AUTOZ_RESOURCE_GET_PRIVATE (iresource);

	ret = (const gchar *)g_strdup (priv->resource_id);

	return ret;
}

static void
autoz_resource_set_property (GObject *object,
                   guint property_id,
                   const GValue *value,
                   GParamSpec *pspec)
{
	AutozResource *form = (AutozResource *)object;

	AutozResourcePrivate *priv = AUTOZ_RESOURCE_GET_PRIVATE (form);

	switch (property_id)
		{
			default:
				G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
				break;
	  }
}

static void
autoz_resource_get_property (GObject *object,
                   guint property_id,
                   GValue *value,
                   GParamSpec *pspec)
{
	AutozResource *form = (AutozResource *)object;

	AutozResourcePrivate *priv = AUTOZ_RESOURCE_GET_PRIVATE (form);

	switch (property_id)
		{
			default:
				G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
				break;
	  }
}
