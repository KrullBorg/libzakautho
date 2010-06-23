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

#include "autoz.h"

typedef struct _Rule Rule;
struct _Rule
	{
		AutozIRole *irole;
		AutozIResource *iresource;
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

		GList *rules;
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

	priv->rules = NULL;
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
	AutozPrivate *priv = AUTOZ_GET_PRIVATE (autoz);

	const gchar *role_id;

	role_id = autoz_irole_get_role_id (irole);

	if (g_hash_table_lookup (priv->roles, role_id) == NULL)
		{
			g_hash_table_insert (priv->roles, (gpointer)role_id, (gpointer)irole);
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
			g_hash_table_insert (priv->resources, (gpointer)resource_id, (gpointer)iresource);
		}
}

void
autoz_allow (Autoz *autoz, AutozIRole *irole, AutozIResource *iresource)
{
	AutozPrivate *priv = AUTOZ_GET_PRIVATE (autoz);

	AutozIRole *real_irole;
	AutozIResource *real_iresource;

	Rule r;

	/* check if exists */
	real_irole = g_hash_table_lookup (priv->roles, autoz_irole_get_role_id (irole));
	if (real_irole == NULL)
		{
			return;
		}
	real_iresource = g_hash_table_lookup (priv->resources, autoz_iresource_get_resource_id (iresource));
	if (real_iresource == NULL)
		{
			return;
		}

	r.irole = real_irole;
	r.iresource = real_iresource;

	priv->rules = g_list_append (priv->rules, g_memdup (&r, sizeof (Rule)));
}

gboolean
autoz_is_allowed (Autoz *autoz, AutozIRole *irole, AutozIResource *iresource)
{
	gboolean ret;

	AutozIRole *real_irole;
	AutozIResource *real_iresource;

	GList *rules;
	Rule *r;

	AutozPrivate *priv = AUTOZ_GET_PRIVATE (autoz);

	ret = FALSE;

	real_irole = g_hash_table_lookup (priv->roles, autoz_irole_get_role_id (irole));
	if (real_irole == NULL)
		{
			return ret;
		}
	real_iresource = g_hash_table_lookup (priv->resources, autoz_iresource_get_resource_id (iresource));
	if (real_iresource == NULL)
		{
			return ret;
		}

	rules = g_list_first (priv->rules);
	while (rules != NULL)
		{
			r = (Rule *)rules->data;

			if (g_strcmp0 (autoz_irole_get_role_id (real_irole), autoz_irole_get_role_id (r->irole)) == 0)
				{
					if (g_strcmp0 (autoz_iresource_get_resource_id (real_iresource), autoz_iresource_get_resource_id (r->iresource)) == 0)
						{
							ret = TRUE;
							break;
						}
				}

			rules = g_list_next (rules);
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
