/*
 * Copyright (C) 2010-2015 Andrea Zagli <azagli@libero.it>
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

static void zak_autho_resource_class_init (ZakAuthoResourceClass *class);
static void zak_autho_resource_init (ZakAuthoResource *form);

static void zak_autho_iresource_interface_init (ZakAuthoIResourceIface *iface);

static const gchar *zak_autho_resource_get_resource_id (ZakAuthoIResource *iresource);

static void zak_autho_resource_set_property (GObject *object,
                               guint property_id,
                               const GValue *value,
                               GParamSpec *pspec);
static void zak_autho_resource_get_property (GObject *object,
                               guint property_id,
                               GValue *value,
                               GParamSpec *pspec);

#define ZAK_AUTHO_RESOURCE_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), ZAK_AUTHO_TYPE_RESOURCE, ZakAuthoResourcePrivate))

typedef struct _ZakAuthoResourcePrivate ZakAuthoResourcePrivate;
struct _ZakAuthoResourcePrivate
	{
		gchar *resource_id;
	};

G_DEFINE_TYPE_WITH_CODE (ZakAuthoResource, zak_autho_resource, G_TYPE_OBJECT,
                         G_IMPLEMENT_INTERFACE (ZAK_AUTHO_TYPE_IRESOURCE,
                                                zak_autho_iresource_interface_init));

static void
zak_autho_resource_class_init (ZakAuthoResourceClass *class)
{
	GObjectClass *object_class = G_OBJECT_CLASS (class);

	object_class->set_property = zak_autho_resource_set_property;
	object_class->get_property = zak_autho_resource_get_property;

	g_type_class_add_private (object_class, sizeof (ZakAuthoResourcePrivate));
}

static void
zak_autho_resource_init (ZakAuthoResource *form)
{
	ZakAuthoResourcePrivate *priv = ZAK_AUTHO_RESOURCE_GET_PRIVATE (form);
}

static void
zak_autho_iresource_interface_init (ZakAuthoIResourceIface *iface)
{
	iface->get_resource_id = zak_autho_resource_get_resource_id;
}

/**
 * zak_autho_resource_new:
 * @resource_id:
 *
 * Returns: the newly created #ZakAuthoResource object.
 */
ZakAuthoResource
*zak_autho_resource_new (const gchar *resource_id)
{
	ZakAuthoResource *resource;
	ZakAuthoResourcePrivate *priv;

	resource = ZAK_AUTHO_RESOURCE (g_object_new (zak_autho_resource_get_type (), NULL));

	priv = ZAK_AUTHO_RESOURCE_GET_PRIVATE (resource);

	priv->resource_id = g_strdup (resource_id);

	return resource;
}

/* PRIVATE */
static const gchar
*zak_autho_resource_get_resource_id (ZakAuthoIResource *iresource)
{
	ZakAuthoResourcePrivate *priv;

	const gchar *ret;

	ret = NULL;

	g_return_val_if_fail (ZAK_AUTHO_IS_RESOURCE (iresource), ret);

	priv = ZAK_AUTHO_RESOURCE_GET_PRIVATE (iresource);

	ret = (const gchar *)g_strdup (priv->resource_id);

	return ret;
}

static void
zak_autho_resource_set_property (GObject *object,
                   guint property_id,
                   const GValue *value,
                   GParamSpec *pspec)
{
	ZakAuthoResource *form = (ZakAuthoResource *)object;

	ZakAuthoResourcePrivate *priv = ZAK_AUTHO_RESOURCE_GET_PRIVATE (form);

	switch (property_id)
		{
			default:
				G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
				break;
	  }
}

static void
zak_autho_resource_get_property (GObject *object,
                   guint property_id,
                   GValue *value,
                   GParamSpec *pspec)
{
	ZakAuthoResource *form = (ZakAuthoResource *)object;

	ZakAuthoResourcePrivate *priv = ZAK_AUTHO_RESOURCE_GET_PRIVATE (form);

	switch (property_id)
		{
			default:
				G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
				break;
	  }
}
