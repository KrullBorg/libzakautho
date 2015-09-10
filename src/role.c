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

#include "role.h"
#include "role_interface.h"

static void zak_autho_role_class_init (ZakAuthoRoleClass *class);
static void zak_autho_role_init (ZakAuthoRole *role);

static void zak_autho_irole_interface_init (ZakAuthoIRoleIface *iface);

static const gchar *zak_autho_role_get_role_id (ZakAuthoIRole *irole);

static void zak_autho_role_set_property (GObject *object,
                               guint property_id,
                               const GValue *value,
                               GParamSpec *pspec);
static void zak_autho_role_get_property (GObject *object,
                               guint property_id,
                               GValue *value,
                               GParamSpec *pspec);

#define ZAK_AUTHO_ROLE_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), ZAK_AUTHO_TYPE_ROLE, ZakAuthoRolePrivate))

typedef struct _ZakAuthoRolePrivate ZakAuthoRolePrivate;
struct _ZakAuthoRolePrivate
	{
		gchar *role_id;
	};

G_DEFINE_TYPE_WITH_CODE (ZakAuthoRole, zak_autho_role, G_TYPE_OBJECT,
                         G_IMPLEMENT_INTERFACE (ZAK_AUTHO_TYPE_IROLE,
                                                zak_autho_irole_interface_init));

static void
zak_autho_role_class_init (ZakAuthoRoleClass *class)
{
	GObjectClass *object_class = G_OBJECT_CLASS (class);

	object_class->set_property = zak_autho_role_set_property;
	object_class->get_property = zak_autho_role_get_property;

	g_type_class_add_private (object_class, sizeof (ZakAuthoRolePrivate));
}

static void
zak_autho_role_init (ZakAuthoRole *form)
{
	ZakAuthoRolePrivate *priv = ZAK_AUTHO_ROLE_GET_PRIVATE (form);
}

static void
zak_autho_irole_interface_init (ZakAuthoIRoleIface *iface)
{
	iface->get_role_id = zak_autho_role_get_role_id;
}

/**
 * zak_autho_role_new:
 * @role_id:
 *
 * Returns: the newly created #ZakAuthoRole object.
 */
ZakAuthoRole
*zak_autho_role_new (const gchar *role_id)
{
	ZakAuthoRole *role;
	ZakAuthoRolePrivate *priv;

	role = ZAK_AUTHO_ROLE (g_object_new (zak_autho_role_get_type (), NULL));

	priv = ZAK_AUTHO_ROLE_GET_PRIVATE (role);

	priv->role_id = g_strdup (role_id);

	return role;
}

/* PRIVATE */
static const gchar
*zak_autho_role_get_role_id (ZakAuthoIRole *irole)
{
	ZakAuthoRolePrivate *priv;

	const gchar *ret;

	ret = NULL;

	g_return_val_if_fail (ZAK_AUTHO_IS_ROLE (irole), ret);

	priv = ZAK_AUTHO_ROLE_GET_PRIVATE (irole);

	ret = (const gchar *)g_strdup (priv->role_id);

	return ret;
}

static void
zak_autho_role_set_property (GObject *object,
                   guint property_id,
                   const GValue *value,
                   GParamSpec *pspec)
{
	ZakAuthoRole *form = (ZakAuthoRole *)object;

	ZakAuthoRolePrivate *priv = ZAK_AUTHO_ROLE_GET_PRIVATE (form);

	switch (property_id)
		{
			default:
				G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
				break;
	  }
}

static void
zak_autho_role_get_property (GObject *object,
                   guint property_id,
                   GValue *value,
                   GParamSpec *pspec)
{
	ZakAuthoRole *form = (ZakAuthoRole *)object;

	ZakAuthoRolePrivate *priv = ZAK_AUTHO_ROLE_GET_PRIVATE (form);

	switch (property_id)
		{
			default:
				G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
				break;
	  }
}
