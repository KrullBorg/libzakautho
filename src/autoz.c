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

#include <stdarg.h>

#include "autoz.h"

typedef struct _Role Role;
struct _Role
	{
		AutozIRole *irole;
		GList *parents;
	};

typedef struct _Resource Resource;
struct _Resource
	{
		AutozIResource *iresource;
		GList *parents;
	};

typedef struct _Rule Rule;
struct _Rule
	{
		Role *role;
		Resource *resource;
	};

static void autoz_class_init (AutozClass *class);
static void autoz_init (Autoz *autoz);

static void autoz_set_property (GObject *object,
                               guint property_id,
                               const GValue *value,
                               GParamSpec *pspec);
static void autoz_get_property (GObject *object,
                               guint property_id,
                               GValue *value,
                               GParamSpec *pspec);

#define AUTOZ_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), TYPE_AUTOZ, AutozPrivate))

typedef struct _AutozPrivate AutozPrivate;
struct _AutozPrivate
	{
		GHashTable *roles;
		GHashTable *resources;

		GHashTable *rules;
	};

G_DEFINE_TYPE (Autoz, autoz, G_TYPE_OBJECT)

static void
autoz_class_init (AutozClass *class)
{
	GObjectClass *object_class = G_OBJECT_CLASS (class);

	object_class->set_property = autoz_set_property;
	object_class->get_property = autoz_get_property;

	g_type_class_add_private (object_class, sizeof (AutozPrivate));
}

static void
autoz_init (Autoz *autoz)
{
	AutozPrivate *priv = AUTOZ_GET_PRIVATE (autoz);

	priv->roles = g_hash_table_new (g_str_hash, g_str_equal);
	priv->resources = g_hash_table_new (g_str_hash, g_str_equal);

	priv->rules = g_hash_table_new (g_str_hash, g_str_equal);
}

/**
 * autoz_new:
 *
 * Returns: the newly created #Autoz object.
 */
Autoz
*autoz_new ()
{

	return AUTOZ (g_object_new (autoz_get_type (), NULL));
}

void
autoz_add_role (Autoz *autoz, AutozIRole *irole)
{
	autoz_add_role_with_parents (autoz, irole, NULL);
}

void
autoz_add_role_with_parents (Autoz *autoz, AutozIRole *irole, ...)
{
	AutozPrivate *priv = AUTOZ_GET_PRIVATE (autoz);

	const gchar *role_id;

	role_id = autoz_irole_get_role_id (irole);

	if (g_hash_table_lookup (priv->roles, role_id) == NULL)
		{
			va_list args;
			Role *role;

			AutozIRole *irole_parent;
			Role *role_parent;

			role = (Role *)g_malloc0 (sizeof (Role));
			role->irole = irole;
			role->parents = NULL;

			va_start (args, irole);
			while ((irole_parent = va_arg (args, AutozIRole *)) != NULL)
				{
					role_parent = g_hash_table_lookup (priv->roles, autoz_irole_get_role_id (irole_parent));
					if (role_parent != NULL)
						{
							role->parents = g_list_append (role->parents, role_parent);
						}					
				}
			va_end (args);

			g_hash_table_insert (priv->roles, (gpointer)role_id, (gpointer)role);
		}
}

void
autoz_add_resource (Autoz *autoz, AutozIResource *iresource)
{
	AutozPrivate *priv = AUTOZ_GET_PRIVATE (autoz);

	const gchar *resource_id;

	resource_id = autoz_iresource_get_resource_id (iresource);

	if (g_hash_table_lookup (priv->resources, resource_id) == NULL)
		{
			Resource *resource;

			resource = (Resource *)g_malloc0 (sizeof (Resource));
			resource->iresource = iresource;
			resource->parents = NULL;

			g_hash_table_insert (priv->resources, (gpointer)resource_id, (gpointer)resource);
		}
}

void
autoz_allow (Autoz *autoz, AutozIRole *irole, AutozIResource *iresource)
{
	AutozPrivate *priv = AUTOZ_GET_PRIVATE (autoz);

	Role *role;
	Resource *resource;

	Rule *r;

	gchar *str_id;

	/* check if exists */
	role = g_hash_table_lookup (priv->roles, autoz_irole_get_role_id (irole));
	if (role == NULL)
		{
			return;
		}

	/* TODO accept also NULL resource (equal to allow to every resource) */
	resource = g_hash_table_lookup (priv->resources, autoz_iresource_get_resource_id (iresource));
	if (resource == NULL)
		{
			return;
		}

	r = (Rule *)g_malloc0 (sizeof (Rule));
	r->role = role;
	r->resource = resource;

	str_id = g_strconcat (autoz_irole_get_role_id (r->role->irole),
	                      "|",
	                      autoz_iresource_get_resource_id (r->resource->iresource),
	                      NULL);

	if (g_hash_table_lookup (priv->rules, str_id) == NULL)
		{
			g_hash_table_insert (priv->rules, str_id, r);
		}
}
	
gboolean
autoz_is_allowed (Autoz *autoz, AutozIRole *irole, AutozIResource *iresource)
{
	gboolean ret;

	Role *role;
	Resource *resource;

	gchar *str_id;

	AutozPrivate *priv = AUTOZ_GET_PRIVATE (autoz);

	ret = FALSE;

	role = g_hash_table_lookup (priv->roles, autoz_irole_get_role_id (irole));
	if (role == NULL)
		{
			return ret;
		}
	resource = g_hash_table_lookup (priv->resources, autoz_iresource_get_resource_id (iresource));
	if (resource == NULL)
		{
			return ret;
		}

	str_id = g_strconcat (autoz_irole_get_role_id (role->irole),
	                      "|",
	                      autoz_iresource_get_resource_id (resource->iresource),
	                      NULL);

	if (g_hash_table_lookup (priv->rules, str_id) != NULL)
		{
			ret = TRUE;
		}

	return ret;
}

/* PRIVATE */
static void
autoz_set_property (GObject *object,
                   guint property_id,
                   const GValue *value,
                   GParamSpec *pspec)
{
	Autoz *autoz = (Autoz *)object;

	AutozPrivate *priv = AUTOZ_GET_PRIVATE (autoz);

	switch (property_id)
		{
			default:
				G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
				break;
	  }
}

static void
autoz_get_property (GObject *object,
                   guint property_id,
                   GValue *value,
                   GParamSpec *pspec)
{
	Autoz *autoz = (Autoz *)object;

	AutozPrivate *priv = AUTOZ_GET_PRIVATE (autoz);

	switch (property_id)
		{
			default:
				G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
				break;
	  }
}
