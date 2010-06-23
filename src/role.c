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

#include "role.h"
#include "role_interface.h"

static void autoz_role_class_init (AutozRoleClass *class);
static void autoz_role_init (AutozRole *role);

static void autoz_irole_interface_init (AutozIRoleIface *iface);

static const gchar *autoz_role_get_role_id (AutozIRole *irole);

static void autoz_role_set_property (GObject *object,
                               guint property_id,
                               const GValue *value,
                               GParamSpec *pspec);
static void autoz_role_get_property (GObject *object,
                               guint property_id,
                               GValue *value,
                               GParamSpec *pspec);

#define AUTOZ_ROLE_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), AUTOZ_TYPE_ROLE, AutozRolePrivate))

typedef struct _AutozRolePrivate AutozRolePrivate;
struct _AutozRolePrivate
	{
		gchar *role_id;
	};

G_DEFINE_TYPE_WITH_CODE (AutozRole, autoz_role, G_TYPE_OBJECT,
                         G_IMPLEMENT_INTERFACE (AUTOZ_TYPE_IROLE,
                                                autoz_irole_interface_init));

static void
autoz_role_class_init (AutozRoleClass *class)
{
	GObjectClass *object_class = G_OBJECT_CLASS (class);

	object_class->set_property = autoz_role_set_property;
	object_class->get_property = autoz_role_get_property;

	g_type_class_add_private (object_class, sizeof (AutozRolePrivate));
}

static void
autoz_role_init (AutozRole *form)
{
	AutozRolePrivate *priv = AUTOZ_ROLE_GET_PRIVATE (form);
}

static void
autoz_irole_interface_init (AutozIRoleIface *iface)
{
	iface->get_role_id = autoz_role_get_role_id;
}

/**
 * autoz_role_new:
 * @role_id:
 *
 * Returns: the newly created #AutozRole object.
 */
AutozRole
*autoz_role_new (const gchar *role_id)
{
	AutozRole *role;
	AutozRolePrivate *priv;

	role = AUTOZ_ROLE (g_object_new (autoz_role_get_type (), NULL));

	priv = AUTOZ_ROLE_GET_PRIVATE (role);

	priv->role_id = g_strdup (role_id);

	return role;
}

/* PRIVATE */
static const gchar
*autoz_role_get_role_id (AutozIRole *irole)
{
	AutozRolePrivate *priv;

	const gchar *ret;

	ret = NULL;

	g_return_val_if_fail (AUTOZ_IS_ROLE (irole), ret);

	priv = AUTOZ_ROLE_GET_PRIVATE (irole);

	ret = (const gchar *)g_strdup (priv->role_id);

	return ret;
}

static void
autoz_role_set_property (GObject *object,
                   guint property_id,
                   const GValue *value,
                   GParamSpec *pspec)
{
	AutozRole *form = (AutozRole *)object;

	AutozRolePrivate *priv = AUTOZ_ROLE_GET_PRIVATE (form);

	switch (property_id)
		{
			default:
				G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
				break;
	  }
}

static void
autoz_role_get_property (GObject *object,
                   guint property_id,
                   GValue *value,
                   GParamSpec *pspec)
{
	AutozRole *form = (AutozRole *)object;

	AutozRolePrivate *priv = AUTOZ_ROLE_GET_PRIVATE (form);

	switch (property_id)
		{
			default:
				G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
				break;
	  }
}
