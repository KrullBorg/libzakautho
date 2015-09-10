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

#include <stdarg.h>
#include <string.h>

#include "autoz.h"

#include "role.h"
#include "resource.h"

typedef struct _Role Role;
struct _Role
	{
		ZakAuthoIRole *irole;
		GList *parents; /* struct Role */
	};

typedef struct _Resource Resource;
struct _Resource
	{
		ZakAuthoIResource *iresource;
		GList *parents; /* struct Resource */
	};

typedef struct _Rule Rule;
struct _Rule
	{
		Role *role;
		Resource *resource;
	};

typedef enum ZakAuthoIsAllowed
	{
		ZAK_AUTHO_ALLOWED,
		ZAK_AUTHO_DENIED,
		ZAK_AUTHO_NOT_FOUND
	} ZakAuthoIsAllowed;

static void zak_autho_class_init (ZakAuthoClass *class);
static void zak_autho_init (ZakAutho *zak_autho);

static ZakAuthoIsAllowed _zak_autho_is_allowed_role (ZakAutho *zak_autho, Role *role, Resource *resource, gboolean exclude_null);
static ZakAuthoIsAllowed _zak_autho_is_allowed_resource (ZakAutho *zak_autho, Role *role, Resource *resource);

static gboolean _zak_autho_delete_table_content (GdaConnection *gdacon, const gchar *table_prefix);
static guint _zak_autho_find_new_table_id (GdaConnection *gdacon, const gchar *table_name);
static guint _zak_autho_get_role_id_db (GdaConnection *gdacon, const gchar *table_name, const gchar *role_id);
static guint _zak_autho_get_resource_id_db (GdaConnection *gdacon, const gchar *table_name, const gchar *resource_id);

static void _zak_autho_check_updated (ZakAutho *zak_autho);

static Role *_zak_autho_get_role_from_id (ZakAutho *zak_autho, const gchar *role_id);
static Resource *_zak_autho_get_resource_from_id (ZakAutho *zak_autho, const gchar *resource_id);

static gchar *_zak_autho_remove_role_name_prefix_from_id (ZakAutho *zak_autho, const gchar *role_id);
static gchar *_zak_autho_remove_resource_name_prefix_from_id (ZakAutho *zak_autho, const gchar *resource_id);

static void zak_autho_set_property (GObject *object,
                               guint property_id,
                               const GValue *value,
                               GParamSpec *pspec);
static void zak_autho_get_property (GObject *object,
                               guint property_id,
                               GValue *value,
                               GParamSpec *pspec);

#define ZAK_AUTHO_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), ZAK_TYPE_AUTHO, ZakAuthoPrivate))

typedef struct _ZakAuthoPrivate ZakAuthoPrivate;
struct _ZakAuthoPrivate
	{
		gchar *role_name_prefix;
		gchar *resource_name_prefix;

		GHashTable *roles; /* struct Role */
		GHashTable *resources; /* struct Resource */

		GHashTable *rules_allow; /* struct Rule */
		GHashTable *rules_deny; /* struct Rule */

		GdaConnection *gdacon;
		gchar *table_prefix;
		GDateTime *gdt_last_load;
		gboolean on_loading;
	};

G_DEFINE_TYPE (ZakAutho, zak_autho, G_TYPE_OBJECT)

static void
zak_autho_class_init (ZakAuthoClass *class)
{
	GObjectClass *object_class = G_OBJECT_CLASS (class);

	object_class->set_property = zak_autho_set_property;
	object_class->get_property = zak_autho_get_property;

	g_type_class_add_private (object_class, sizeof (ZakAuthoPrivate));
}

static void
zak_autho_init (ZakAutho *zak_autho)
{
	ZakAuthoPrivate *priv = ZAK_AUTHO_GET_PRIVATE (zak_autho);

	priv->role_name_prefix = NULL;
	priv->resource_name_prefix = NULL;

	priv->roles = g_hash_table_new (g_str_hash, g_str_equal);
	priv->resources = g_hash_table_new (g_str_hash, g_str_equal);

	priv->rules_allow = g_hash_table_new (g_str_hash, g_str_equal);
	priv->rules_deny = g_hash_table_new (g_str_hash, g_str_equal);

	priv->gdacon = NULL;
	priv->table_prefix = NULL;
	priv->gdt_last_load = NULL;
	priv->on_loading = FALSE;
}

/**
 * zak_autho_new:
 *
 * Returns: the newly created #ZakAutho object.
 */
ZakAutho
*zak_autho_new ()
{
	return ZAK_AUTHO (g_object_new (zak_autho_get_type (), NULL));
}

/**
 * zak_autho_set_role_name_prefix:
 * @zak_autho: an #ZakAutho object.
 * @prefix:
 *
 */
void
zak_autho_set_role_name_prefix (ZakAutho *zak_autho, const gchar *prefix)
{
	ZakAuthoPrivate *priv;

	g_return_if_fail (IS_ZAK_AUTHO (zak_autho));

	priv = ZAK_AUTHO_GET_PRIVATE (zak_autho);

	if (priv->role_name_prefix != NULL)
		{
			g_free (priv->role_name_prefix);
		}

	if (prefix == NULL)
		{
			priv->role_name_prefix = NULL;
		}
	else
		{
			priv->role_name_prefix = g_strdup (prefix);
		}
}

/**
 * zak_autho_get_role_name_prefix:
 * @zak_autho: an #ZakAutho object.
 *
 */
const gchar
*zak_autho_get_role_name_prefix (ZakAutho *zak_autho)
{
	ZakAuthoPrivate *priv;

	g_return_val_if_fail (IS_ZAK_AUTHO (zak_autho), NULL);

	priv = ZAK_AUTHO_GET_PRIVATE (zak_autho);

	return priv->role_name_prefix == NULL ? NULL : g_strdup (priv->role_name_prefix);
}

/**
 * zak_autho_set_resource_name_prefix:
 * @zak_autho: an #ZakAutho object.
 * @prefix:
 *
 */
void
zak_autho_set_resource_name_prefix (ZakAutho *zak_autho, const gchar *prefix)
{
	ZakAuthoPrivate *priv;

	g_return_if_fail (IS_ZAK_AUTHO (zak_autho));

	priv = ZAK_AUTHO_GET_PRIVATE (zak_autho);

	if (priv->resource_name_prefix != NULL)
		{
			g_free (priv->resource_name_prefix);
		}

	if (prefix == NULL)
		{
			priv->resource_name_prefix = NULL;
		}
	else
		{
			priv->resource_name_prefix = g_strdup (prefix);
		}
}

/**
 * zak_autho_get_resource_name_prefix:
 * @zak_autho: an #ZakAutho object.
 *
 */
const gchar
*zak_autho_get_resource_name_prefix (ZakAutho *zak_autho)
{
	ZakAuthoPrivate *priv;

	g_return_val_if_fail (IS_ZAK_AUTHO (zak_autho), NULL);

	priv = ZAK_AUTHO_GET_PRIVATE (zak_autho);

	return priv->resource_name_prefix == NULL ? NULL : g_strdup (priv->resource_name_prefix);
}

/**
 * zak_autho_add_role:
 * @zak_autho: an #ZakAutho object.
 * @irole:
 *
 */
void
zak_autho_add_role (ZakAutho *zak_autho, ZakAuthoIRole *irole)
{
	zak_autho_add_role_with_parents (zak_autho, irole, NULL);
}

/**
 * zak_autho_add_role_with_parents:
 * @zak_autho: an #ZakAutho object.
 * @irole:
 * @...:
 *
 */
void
zak_autho_add_role_with_parents (ZakAutho *zak_autho, ZakAuthoIRole *irole, ...)
{
	ZakAuthoPrivate *priv;

	const gchar *role_id;
	const gchar *role_id_parent;

	g_return_if_fail (IS_ZAK_AUTHO (zak_autho));
	g_return_if_fail (ZAK_AUTHO_IS_IROLE (irole));

	priv = ZAK_AUTHO_GET_PRIVATE (zak_autho);

	role_id = zak_autho_irole_get_role_id (irole);

	if (g_hash_table_lookup (priv->roles, role_id) == NULL)
		{
			va_list args;
			Role *role;

			ZakAuthoIRole *irole_parent;
			Role *role_parent;

			role = (Role *)g_malloc0 (sizeof (Role));
			role->irole = irole;
			role->parents = NULL;

			va_start (args, irole);
			while ((irole_parent = va_arg (args, ZakAuthoIRole *)) != NULL)
				{
					role_id_parent = zak_autho_irole_get_role_id (irole_parent);
					if (g_strcmp0 (role_id, role_id_parent) == 0)
						{
							g_warning ("The parent cannot be himself (%s).", role_id);
						}
					else
						{
							role_parent = g_hash_table_lookup (priv->roles, role_id_parent);
							if (role_parent != NULL)
								{
									role->parents = g_list_append (role->parents, role_parent);
								}
							else
								{
									g_warning ("Role «%s» not found.", zak_autho_irole_get_role_id (irole_parent));
								}
						}
				}
			va_end (args);

			g_hash_table_insert (priv->roles, (gpointer)role_id, (gpointer)role);
		}
	else
		{
			g_warning ("Role «%s» not found.", role_id);
		}
}

/**
 * zak_autho_add_parent_to_role:
 * @zak_autho: an #ZakAutho object.
 * @irole:
 * @irole_parent:
 *
 */
void
zak_autho_add_parent_to_role (ZakAutho *zak_autho, ZakAuthoIRole *irole, ZakAuthoIRole *irole_parent)
{
	zak_autho_add_parents_to_role (zak_autho, irole, irole_parent, NULL);
}

/**
 * zak_autho_add_parents_to_role:
 * @zak_autho: an #ZakAutho object.
 * @irole:
 * @...:
 *
 */
void
zak_autho_add_parents_to_role (ZakAutho *zak_autho, ZakAuthoIRole *irole, ...)
{
	ZakAuthoPrivate *priv;

	Role *role;

	const gchar *role_id;
	const gchar *role_id_parent;

	g_return_if_fail (IS_ZAK_AUTHO (zak_autho));
	g_return_if_fail (ZAK_AUTHO_IS_IROLE (irole));

	priv = ZAK_AUTHO_GET_PRIVATE (zak_autho);

	role_id = zak_autho_irole_get_role_id (irole);

	role = g_hash_table_lookup (priv->roles, role_id);
	if (role != NULL)
		{
			va_list args;

			ZakAuthoIRole *irole_parent;
			Role *role_parent;

			va_start (args, irole);
			while ((irole_parent = va_arg (args, ZakAuthoIRole *)) != NULL)
				{
					role_id_parent = zak_autho_irole_get_role_id (irole_parent);
					if (g_strcmp0 (role_id, role_id_parent) == 0)
						{
							g_warning ("The parent cannot be himself (%s).", role_id);
						}
					else
						{
							role_parent = g_hash_table_lookup (priv->roles, role_id_parent);
							if (role_parent != NULL)
								{
									role->parents = g_list_append (role->parents, role_parent);
								}
							else
								{
									g_warning ("Role «%s» not found.", role_id);
								}
						}
				}
			va_end (args);
		}
	else
		{
			g_warning ("Role «%s» not found.", role_id);
		}
}

/**
 * zak_autho_role_is_child:
 * @zak_autho: an #ZakAutho object.
 * @irole: an #ZakAuthoIRole object.
 * @irole_parent: an #ZakAuthoIRole object.
 *
 * Returns: #TRUE if @irole is a @irole_parent's child; #FALSE otherwise.
 */
gboolean
zak_autho_role_is_child (ZakAutho *zak_autho, ZakAuthoIRole *irole, ZakAuthoIRole *irole_parent)
{
	ZakAuthoPrivate *priv;
	gboolean ret;

	Role *role;
	Role *role_parent;
	const gchar *role_id_parent;
	GList *parent;

	g_return_val_if_fail (IS_ZAK_AUTHO (zak_autho), FALSE);
	g_return_val_if_fail (ZAK_AUTHO_IS_IROLE (irole), FALSE);
	g_return_val_if_fail (ZAK_AUTHO_IS_IROLE (irole_parent), FALSE);

	_zak_autho_check_updated (zak_autho);

	ret = FALSE;
	priv = ZAK_AUTHO_GET_PRIVATE (zak_autho);

	role = g_hash_table_lookup (priv->roles, zak_autho_irole_get_role_id (irole));
	if (role == NULL)
		{
			g_warning ("Role «%s» not found.", zak_autho_irole_get_role_id (irole));
			return ret;
		}
	role_id_parent = zak_autho_irole_get_role_id (irole_parent);
	role_parent = g_hash_table_lookup (priv->roles, role_id_parent);
	if (role_parent == NULL)
		{
			g_warning ("Role parent «%s» not found.", role_id_parent);
			return ret;
		}

	parent = g_list_first (role->parents);
	while (parent != NULL)
		{
			/* TODO recursion */
			role_parent = (Role *)parent->data;
			if (g_strcmp0 (role_id_parent, zak_autho_irole_get_role_id (role_parent->irole)) == 0)
				{
					ret = TRUE;
					break;
				}

			parent = g_list_next (parent);
		}

	return ret;
}

static Role
*_zak_autho_get_role_from_id (ZakAutho *zak_autho, const gchar *role_id)
{
	ZakAuthoPrivate *priv;
	Role *role;

	gchar *_role_id;

	g_return_val_if_fail (IS_ZAK_AUTHO (zak_autho), NULL);

	_zak_autho_check_updated (zak_autho);

	priv = ZAK_AUTHO_GET_PRIVATE (zak_autho);

	if (priv->role_name_prefix != NULL)
		{
			_role_id = g_strdup_printf ("%s%s", priv->role_name_prefix, role_id);
		}
	else
		{
			_role_id = g_strdup (role_id);
		}

	role = g_hash_table_lookup (priv->roles, _role_id);
	g_free (_role_id);

	return role;
}

/**
 * zak_autho_get_role_from_id:
 * @zak_autho: an #ZakAutho object.
 * @role_id:
 *
 */
ZakAuthoIRole
*zak_autho_get_role_from_id (ZakAutho *zak_autho, const gchar *role_id)
{
	Role *role;

	role = _zak_autho_get_role_from_id (zak_autho, role_id);
	if (role == NULL)
		{
			return NULL;
		}
	else
		{
			return role->irole;
		}
}

/**
 * zak_autho_add_resource:
 * @zak_autho: an #ZakAutho object.
 * @iresource:
 *
 */
void
zak_autho_add_resource (ZakAutho *zak_autho, ZakAuthoIResource *iresource)
{
	zak_autho_add_resource_with_parents (zak_autho, iresource, NULL);
}

/**
 * zak_autho_add_resource_with_parents:
 * @zak_autho: an #ZakAutho object.
 * @iresource:
 * @...:
 *
 */
void
zak_autho_add_resource_with_parents (ZakAutho *zak_autho, ZakAuthoIResource *iresource, ...)
{
	ZakAuthoPrivate *priv = ZAK_AUTHO_GET_PRIVATE (zak_autho);

	const gchar *resource_id;
	const gchar *resource_id_parent;

	g_return_if_fail (IS_ZAK_AUTHO (zak_autho));
	g_return_if_fail (ZAK_AUTHO_IS_IRESOURCE (iresource));

	resource_id = zak_autho_iresource_get_resource_id (iresource);

	if (g_hash_table_lookup (priv->resources, resource_id) == NULL)
		{
			va_list args;
			Resource *resource;

			ZakAuthoIResource *iresource_parent;
			Resource *resource_parent;

			resource = (Resource *)g_malloc0 (sizeof (Resource));
			resource->iresource = iresource;
			resource->parents = NULL;

			va_start (args, iresource);
			while ((iresource_parent = va_arg (args, ZakAuthoIResource *)) != NULL)
				{
					resource_id_parent = zak_autho_iresource_get_resource_id (iresource_parent);
					if (g_strcmp0 (resource_id, resource_id_parent) == 0)
						{
							g_warning ("The parent cannot be himself (%s).", resource_id);
						}
					else
						{
							resource_parent = g_hash_table_lookup (priv->resources, resource_id_parent);
							if (resource_parent != NULL)
								{
									resource->parents = g_list_append (resource->parents, resource_parent);
								}
							else
								{
									g_warning ("Resource «%s» not found.", zak_autho_iresource_get_resource_id (iresource_parent));
							}
						}
				}
			va_end (args);

			g_hash_table_insert (priv->resources, (gpointer)resource_id, (gpointer)resource);
		}
	else
		{
			g_warning ("Resource «%s» not found.", resource_id);
		}
}

/**
 * zak_autho_add_parent_to_resource:
 * @zak_autho: an #ZakAutho object.
 * @iresource: an #ZakAuthoIResource object.
 * @iresource_parent: an #ZakAuthoIResource object.
 *
 */
void
zak_autho_add_parent_to_resource (ZakAutho *zak_autho, ZakAuthoIResource *iresource, ZakAuthoIResource *iresource_parent)
{
	zak_autho_add_parents_to_resource (zak_autho, iresource, iresource_parent, NULL);
}

/**
 * zak_autho_add_parents_to_resource:
 * @zak_autho: an #ZakAutho object.
 * @iresource:
 * @...:
 *
 */
void
zak_autho_add_parents_to_resource (ZakAutho *zak_autho, ZakAuthoIResource *iresource, ...)
{
	ZakAuthoPrivate *priv;

	Resource *resource;

	const gchar *resource_id;
	const gchar *resource_id_parent;

	g_return_if_fail (IS_ZAK_AUTHO (zak_autho));
	g_return_if_fail (ZAK_AUTHO_IS_IRESOURCE (iresource));

	priv = ZAK_AUTHO_GET_PRIVATE (zak_autho);

	resource_id = zak_autho_iresource_get_resource_id (iresource);

	resource = g_hash_table_lookup (priv->resources, resource_id);
	if (resource != NULL)
		{
			va_list args;

			ZakAuthoIResource *iresource_parent;
			Resource *resource_parent;

			va_start (args, iresource);
			while ((iresource_parent = va_arg (args, ZakAuthoIResource *)) != NULL)
				{
					resource_id_parent = zak_autho_iresource_get_resource_id (iresource_parent);
					if (g_strcmp0 (resource_id, resource_id_parent) == 0)
						{
							g_warning ("The parent cannot be himself (%s).", resource_id);
						}
					else
						{
							resource_parent = g_hash_table_lookup (priv->resources, resource_id_parent);
							if (resource_parent != NULL)
								{
									resource->parents = g_list_append (resource->parents, resource_parent);
								}
							else
								{
									g_warning ("Resource «%s» not found.", resource_id);
								}
						}
				}
			va_end (args);
		}
	else
		{
			g_warning ("Resource «%s» not found.", resource_id);
		}
}

/**
 * zak_autho_resource_is_child:
 * @zak_autho: an #ZakAutho object.
 * @iresource: an #ZakAuthoIResource object.
 * @iresource_parent: an #ZakAuthoIResource object.
 *
 * Returns: #TRUE if @iresource is a @iresource_parent's child; #FALSE otherwise.
 */
gboolean
zak_autho_resource_is_child (ZakAutho *zak_autho, ZakAuthoIResource *iresource, ZakAuthoIResource *iresource_parent)
{
	ZakAuthoPrivate *priv;
	gboolean ret;

	Resource *resource;
	Resource *resource_parent;
	const gchar *resource_id_parent;
	GList *parent;

	g_return_val_if_fail (IS_ZAK_AUTHO (zak_autho), FALSE);
	g_return_val_if_fail (ZAK_AUTHO_IS_IRESOURCE (iresource), FALSE);
	g_return_val_if_fail (ZAK_AUTHO_IS_IRESOURCE (iresource_parent), FALSE);

	_zak_autho_check_updated (zak_autho);

	ret = FALSE;
	priv = ZAK_AUTHO_GET_PRIVATE (zak_autho);

	resource = g_hash_table_lookup (priv->resources, zak_autho_iresource_get_resource_id (iresource));
	if (resource == NULL)
		{
			g_warning ("Resource «%s» not found.", zak_autho_iresource_get_resource_id (iresource));
			return ret;
		}
	resource_id_parent = zak_autho_iresource_get_resource_id (iresource_parent);
	resource_parent = g_hash_table_lookup (priv->resources, resource_id_parent);
	if (resource_parent == NULL)
		{
			g_warning ("Resource parent «%s» not found.", resource_id_parent);
			return ret;
		}

	parent = g_list_first (resource->parents);
	while (parent != NULL)
		{
			/* TODO recursion */
			resource_parent = (Resource *)parent->data;
			if (g_strcmp0 (resource_id_parent, zak_autho_iresource_get_resource_id (resource_parent->iresource)) == 0)
				{
					ret = TRUE;
					break;
				}

			parent = g_list_next (parent);
		}

	return ret;
}

static Resource
*_zak_autho_get_resource_from_id (ZakAutho *zak_autho, const gchar *resource_id)
{
	ZakAuthoPrivate *priv;
	Resource *resource;

	gchar *_resource_id;

	g_return_val_if_fail (IS_ZAK_AUTHO (zak_autho), NULL);

	_zak_autho_check_updated (zak_autho);

	priv = ZAK_AUTHO_GET_PRIVATE (zak_autho);

	if (priv->resource_name_prefix != NULL)
		{
			_resource_id = g_strdup_printf ("%s%s", priv->resource_name_prefix, resource_id);
		}
	else
		{
			_resource_id = g_strdup (resource_id);
		}

	resource = g_hash_table_lookup (priv->resources, _resource_id);
	g_free (_resource_id);

	return resource;
}

/**
 * zak_autho_get_resource_from_id:
 * @zak_autho: an #ZakAutho object.
 * @resource_id:
 *
 */
ZakAuthoIResource
*zak_autho_get_resource_from_id (ZakAutho *zak_autho, const gchar *resource_id)
{
	Resource *resource;

	resource = _zak_autho_get_resource_from_id (zak_autho, resource_id);
	if (resource == NULL)
		{
			return NULL;
		}
	else
		{
			return resource->iresource;
		}
}

/**
 * zak_autho_allow:
 * @zak_autho: an #ZakAutho object.
 * @irole:
 * @iresource:
 *
 */
void
zak_autho_allow (ZakAutho *zak_autho, ZakAuthoIRole *irole, ZakAuthoIResource *iresource)
{
	ZakAuthoPrivate *priv;

	Role *role;
	Resource *resource;

	Rule *r;

	gchar *str_id;

	g_return_if_fail (IS_ZAK_AUTHO (zak_autho));

	priv = ZAK_AUTHO_GET_PRIVATE (zak_autho);

	/* check if exists */
	role = g_hash_table_lookup (priv->roles, zak_autho_irole_get_role_id (irole));
	if (role == NULL)
		{
			g_warning ("Role «%s» not found.", zak_autho_irole_get_role_id (irole));
			return;
		}

	/* accept also NULL resource (equal to allow every resource) */
	if (iresource == NULL)
		{
			resource = NULL;
		}
	else
		{
			g_return_if_fail (ZAK_AUTHO_IS_IRESOURCE (iresource));

			resource = g_hash_table_lookup (priv->resources, zak_autho_iresource_get_resource_id (iresource));
			if (resource == NULL)
				{
					g_warning ("Resource «%s» not found.", zak_autho_iresource_get_resource_id (iresource));
					return;
				}
		}

	r = (Rule *)g_malloc0 (sizeof (Rule));
	r->role = role;
	r->resource = resource;

	str_id = g_strconcat (zak_autho_irole_get_role_id (r->role->irole),
	                      "|",
	                      (resource == NULL ? "NULL" : zak_autho_iresource_get_resource_id (r->resource->iresource)),
	                      NULL);

	if (g_hash_table_lookup (priv->rules_allow, str_id) == NULL)
		{
			g_hash_table_insert (priv->rules_allow, str_id, r);
		}
}

/**
 * zak_autho_deny:
 * @zak_autho: an #ZakAutho object.
 * @irole:
 * @iresource:
 *
 */
void
zak_autho_deny (ZakAutho *zak_autho, ZakAuthoIRole *irole, ZakAuthoIResource *iresource)
{
	ZakAuthoPrivate *priv;

	Role *role;
	Resource *resource;

	Rule *r;

	gchar *str_id;

	g_return_if_fail (IS_ZAK_AUTHO (zak_autho));

	priv = ZAK_AUTHO_GET_PRIVATE (zak_autho);

	/* check if exists */
	role = g_hash_table_lookup (priv->roles, zak_autho_irole_get_role_id (irole));
	if (role == NULL)
		{
			g_warning ("Role «%s» not found.", zak_autho_irole_get_role_id (irole));
			return;
		}

	/* accept also NULL resource (equal to deny every resource) */
	if (iresource == NULL)
		{
			resource = NULL;
		}
	else
		{
			g_return_if_fail (ZAK_AUTHO_IS_IRESOURCE (iresource));

			resource = g_hash_table_lookup (priv->resources, zak_autho_iresource_get_resource_id (iresource));
			if (resource == NULL)
				{
					return;
				}
		}

	r = (Rule *)g_malloc0 (sizeof (Rule));
	r->role = role;
	r->resource = resource;

	str_id = g_strconcat (zak_autho_irole_get_role_id (r->role->irole),
	                      "|",
	                      (resource == NULL ? "NULL" : zak_autho_iresource_get_resource_id (r->resource->iresource)),
	                      NULL);

	if (g_hash_table_lookup (priv->rules_deny, str_id) == NULL)
		{
			g_hash_table_insert (priv->rules_deny, str_id, r);
		}
}

static ZakAuthoIsAllowed
_zak_autho_is_allowed_role (ZakAutho *zak_autho, Role *role, Resource *resource, gboolean exclude_null)
{
	ZakAuthoIsAllowed ret;

	gchar *str_id;

	ZakAuthoPrivate *priv = ZAK_AUTHO_GET_PRIVATE (zak_autho);

	ret = ZAK_AUTHO_NOT_FOUND;

	if (!exclude_null)
		{
			/* first trying for a rule for every resource */
			str_id = g_strconcat (zak_autho_irole_get_role_id (role->irole),
			                     "|NULL",
			                     NULL);

			if (g_hash_table_lookup (priv->rules_deny, str_id) != NULL)
				{
					ret = ZAK_AUTHO_DENIED;
					return ret;
				}
			if (g_hash_table_lookup (priv->rules_allow, str_id) != NULL)
				{
					ret = ZAK_AUTHO_ALLOWED;
					return ret;
				}
		}

	/* and after for specific resource */
	str_id = g_strconcat (zak_autho_irole_get_role_id (role->irole),
	                      "|",
	                      zak_autho_iresource_get_resource_id (resource->iresource),
	                      NULL);

	if (g_hash_table_lookup (priv->rules_deny, str_id) != NULL)
		{
			ret = ZAK_AUTHO_DENIED;
			return ret;
		}
	if (g_hash_table_lookup (priv->rules_allow, str_id) != NULL)
		{
			ret = ZAK_AUTHO_ALLOWED;
			return ret;
		}

	if (ret == ZAK_AUTHO_NOT_FOUND && resource->parents != NULL)
		{
			/* trying parents */
			GList *parents;

			parents = g_list_first (resource->parents);
			while (parents != NULL)
				{
					ret = _zak_autho_is_allowed_resource (zak_autho, role, (Resource *)parents->data);
					if (ret != ZAK_AUTHO_NOT_FOUND)
						 {
						 	break;
						 }

					parents = g_list_next (parents);
				}
		}

	if (ret == ZAK_AUTHO_NOT_FOUND && role->parents != NULL)
		{
			/* trying parents */
			GList *parents;

			parents = g_list_first (role->parents);
			while (parents != NULL)
				{
					ret = _zak_autho_is_allowed_role (zak_autho, (Role *)parents->data, resource, exclude_null);
					if (ret != ZAK_AUTHO_NOT_FOUND)
						 {
						 	break;
						 }

					parents = g_list_next (parents);
				}
		}

	return ret;
}

static ZakAuthoIsAllowed
_zak_autho_is_allowed_resource (ZakAutho *zak_autho, Role *role, Resource *resource)
{
	ZakAuthoIsAllowed ret;

	gchar *str_id;

	ZakAuthoPrivate *priv = ZAK_AUTHO_GET_PRIVATE (zak_autho);

	ret = ZAK_AUTHO_NOT_FOUND;

	str_id = g_strconcat (zak_autho_irole_get_role_id (role->irole),
	                      "|",
	                      zak_autho_iresource_get_resource_id (resource->iresource),
	                      NULL);

	if (g_hash_table_lookup (priv->rules_deny, str_id) != NULL)
		{
			ret = ZAK_AUTHO_DENIED;
			return ret;
		}
	else if (g_hash_table_lookup (priv->rules_allow, str_id) != NULL)
		{
			ret = ZAK_AUTHO_ALLOWED;
			return ret;
		}
	else if (resource->parents != NULL)
		{
			/* trying parents */
			GList *parents;

			parents = g_list_first (resource->parents);
			while (parents != NULL)
				{
					ret = _zak_autho_is_allowed_resource (zak_autho, role, (Resource *)parents->data);
					if (ret != ZAK_AUTHO_NOT_FOUND)
						 {
							break;
						}

					parents = g_list_next (parents);
				}
		}

	return ret;
}

static gchar
*_zak_autho_remove_role_name_prefix_from_id (ZakAutho *zak_autho, const gchar *role_id)
{
	gchar *ret;

	ZakAuthoPrivate *priv;

	g_return_val_if_fail (IS_ZAK_AUTHO (zak_autho), FALSE);

	priv = ZAK_AUTHO_GET_PRIVATE (zak_autho);

	ret = NULL;

	if (priv->role_name_prefix == NULL
	    || strlen (priv->role_name_prefix) > strlen (role_id))
		{
			ret = g_strdup (role_id);
		}
	else
		{
			ret = g_strdup (role_id + strlen (priv->role_name_prefix));
		}

	return ret;
}

static gchar
*_zak_autho_remove_resource_name_prefix_from_id (ZakAutho *zak_autho, const gchar *resource_id)
{
	gchar *ret;

	ZakAuthoPrivate *priv;

	g_return_val_if_fail (IS_ZAK_AUTHO (zak_autho), FALSE);

	priv = ZAK_AUTHO_GET_PRIVATE (zak_autho);

	ret = NULL;

	if (priv->resource_name_prefix == NULL
	    || strlen (priv->resource_name_prefix) > strlen (resource_id))
		{
			ret = g_strdup (resource_id);
		}
	else
		{
			ret = g_strdup (resource_id + strlen (priv->resource_name_prefix));
		}

	return ret;
}

/**
 * zak_autho_is_allowed:
 * @zak_autho: an #ZakAutho object.
 * @irole: an #ZakAuthoIRole object.
 * @iresource: an #ZakAuthoIResource object.
 * @exclude_null: whether or not to exclude roles allowed to every resource.
 *
 */
gboolean
zak_autho_is_allowed (ZakAutho *zak_autho, ZakAuthoIRole *irole, ZakAuthoIResource *iresource, gboolean exclude_null)
{
	gboolean ret;
	ZakAuthoIsAllowed isAllowed;

	Role *role;
	Resource *resource;

	gchar *str_id;

	ZakAuthoPrivate *priv;

	g_return_val_if_fail (IS_ZAK_AUTHO (zak_autho), FALSE);
	g_return_val_if_fail (ZAK_AUTHO_IS_IROLE (irole), FALSE);

	_zak_autho_check_updated (zak_autho);

	priv = ZAK_AUTHO_GET_PRIVATE (zak_autho);
	ret = FALSE;
	isAllowed = ZAK_AUTHO_NOT_FOUND;

	role = _zak_autho_get_role_from_id (zak_autho, _zak_autho_remove_role_name_prefix_from_id (zak_autho, zak_autho_irole_get_role_id (irole)));
	if (role == NULL)
		{
			g_warning ("Role «%s» not found.", zak_autho_irole_get_role_id (irole));
			return ret;
		}

	if (!exclude_null)
		{
			/* first trying for a rule for every resource */
			str_id = g_strconcat (zak_autho_irole_get_role_id (irole),
			                      "|NULL",
			                      NULL);

			if (g_hash_table_lookup (priv->rules_deny, str_id) != NULL)
				{
					ret = FALSE;
					return ret;
				}
			if (g_hash_table_lookup (priv->rules_allow, str_id) != NULL)
				{
					ret = TRUE;
					return ret;
				}
		}

	g_return_val_if_fail (ZAK_AUTHO_IS_IRESOURCE (iresource), FALSE);

	resource = _zak_autho_get_resource_from_id (zak_autho, _zak_autho_remove_resource_name_prefix_from_id (zak_autho, zak_autho_iresource_get_resource_id (iresource)));
	if (resource == NULL)
		{
			g_warning ("Resource «%s» not found.", zak_autho_iresource_get_resource_id (iresource));
			return ret;
		}

	/* and after for specific resource */
	str_id = g_strconcat (zak_autho_irole_get_role_id (irole),
	                      "|",
	                      zak_autho_iresource_get_resource_id (iresource),
	                      NULL);

	if (g_hash_table_lookup (priv->rules_deny, str_id) != NULL)
		{
			ret = FALSE;
			return ret;
		}
	if (g_hash_table_lookup (priv->rules_allow, str_id) != NULL)
		{
			ret = TRUE;
			return ret;
		}

	if (!ret && resource->parents != NULL)
		{
			/* trying parents */
			GList *parents;

			parents = g_list_first (resource->parents);
			while (parents != NULL)
				{
					isAllowed = _zak_autho_is_allowed_resource (zak_autho, role, (Resource *)parents->data);
					if (isAllowed == ZAK_AUTHO_DENIED)
						 {
						 	ret = FALSE;
						 	break;
						 }
					else if (isAllowed == ZAK_AUTHO_ALLOWED)
						{
							ret = TRUE;
							break;
						}

					parents = g_list_next (parents);
				}
		}

	if (!ret && isAllowed == ZAK_AUTHO_NOT_FOUND && role->parents != NULL)
		{
			/* trying parents */
			GList *parents;

			parents = g_list_first (role->parents);
			while (parents != NULL)
				{
					isAllowed = _zak_autho_is_allowed_role (zak_autho, (Role *)parents->data, resource, exclude_null);
					if (isAllowed == ZAK_AUTHO_DENIED)
						 {
						 	ret = FALSE;
						 	break;
						 }
					else if (isAllowed == ZAK_AUTHO_ALLOWED)
						{
							ret = TRUE;
							break;
						}

					parents = g_list_next (parents);
				}
		}

	return ret;
}

/**
 * zak_autho_clear:
 * @zak_autho:
 *
 */
gboolean
zak_autho_clear (ZakAutho *zak_autho)
{
	gboolean ret;

	ZakAuthoPrivate *priv;

	g_return_val_if_fail (IS_ZAK_AUTHO (zak_autho), FALSE);

	priv = ZAK_AUTHO_GET_PRIVATE (zak_autho);

	ret = TRUE;

	g_hash_table_destroy (priv->roles);
	g_hash_table_destroy (priv->resources);
	g_hash_table_destroy (priv->rules_allow);
	g_hash_table_destroy (priv->rules_deny);

	priv->roles = g_hash_table_new (g_str_hash, g_str_equal);
	priv->resources = g_hash_table_new (g_str_hash, g_str_equal);
	priv->rules_allow = g_hash_table_new (g_str_hash, g_str_equal);
	priv->rules_deny = g_hash_table_new (g_str_hash, g_str_equal);

	return ret;
}

/**
 * zak_autho_get_xml:
 * @zak_autho: an #ZakAutho object.
 *
 */
xmlNodePtr
zak_autho_get_xml (ZakAutho *zak_autho)
{
	ZakAuthoPrivate *priv;
	xmlNodePtr ret;
	xmlNodePtr xnode_parent;
	xmlNodePtr xnode;

	GHashTableIter iter;
	gpointer key, value;

	Role *role;
	Resource *resource;
	Rule *rule;

	GList *parent;

	g_return_val_if_fail (IS_ZAK_AUTHO (zak_autho), FALSE);

	priv = ZAK_AUTHO_GET_PRIVATE (zak_autho);

	ret = xmlNewNode (NULL, "zak_autho");

	/* roles */
	g_hash_table_iter_init (&iter, priv->roles);
	while (g_hash_table_iter_next (&iter, &key, &value))
		{
			xnode_parent = xmlNewNode (NULL, "role");

			role = (Role *)value;
			xmlSetProp (xnode_parent, "id", zak_autho_irole_get_role_id (ZAK_AUTHO_IROLE (role->irole)));

			xmlAddChild (ret, xnode_parent);

			parent = role->parents;
			while (parent != NULL)
				{
					xnode = xmlNewNode (NULL, "parent");

					role = (Role *)parent->data;
					xmlSetProp (xnode, "id", zak_autho_irole_get_role_id (ZAK_AUTHO_IROLE (role->irole)));

					xmlAddChild (xnode_parent, xnode);

					parent = g_list_next (parent);
				}
		}

	/* resources */
	g_hash_table_iter_init (&iter, priv->resources);
	while (g_hash_table_iter_next (&iter, &key, &value))
		{
			xnode_parent = xmlNewNode (NULL, "resource");

			resource = (Resource *)value;
			xmlSetProp (xnode_parent, "id", zak_autho_iresource_get_resource_id (ZAK_AUTHO_IRESOURCE (resource->iresource)));

			xmlAddChild (ret, xnode_parent);

			parent = resource->parents;
			while (parent != NULL)
				{
					xnode = xmlNewNode (NULL, "parent");

					resource = (Resource *)parent->data;
					xmlSetProp (xnode, "id", zak_autho_iresource_get_resource_id (ZAK_AUTHO_IRESOURCE (resource->iresource)));

					xmlAddChild (xnode_parent, xnode);

					parent = g_list_next (parent);
				}
		}

	/* rules allow */
	g_hash_table_iter_init (&iter, priv->rules_allow);
	while (g_hash_table_iter_next (&iter, &key, &value))
		{
			xnode = xmlNewNode (NULL, "rule");

			xmlSetProp (xnode, "allow", "yes");

			rule = (Rule *)value;
			xmlSetProp (xnode, "role", zak_autho_irole_get_role_id (ZAK_AUTHO_IROLE (rule->role->irole)));
			if (rule->resource != NULL)
				{
					xmlSetProp (xnode, "resource", zak_autho_iresource_get_resource_id (ZAK_AUTHO_IRESOURCE (rule->resource->iresource)));
				}
			else
				{
					xmlSetProp (xnode, "resource", "");
				}

			xmlAddChild (ret, xnode);
		}

	/* rules deny */
	g_hash_table_iter_init (&iter, priv->rules_deny);
	while (g_hash_table_iter_next (&iter, &key, &value))
		{
			xnode = xmlNewNode (NULL, "rule");

			xmlSetProp (xnode, "allow", "no");

			rule = (Rule *)value;
			xmlSetProp (xnode, "role", zak_autho_irole_get_role_id (ZAK_AUTHO_IROLE (rule->role->irole)));
			if (rule->resource != NULL)
				{
					xmlSetProp (xnode, "resource", zak_autho_iresource_get_resource_id (ZAK_AUTHO_IRESOURCE (rule->resource->iresource)));
				}
			else
				{
					xmlSetProp (xnode, "resource", "all");
				}

			xmlAddChild (ret, xnode);
		}

	return ret;
}

/**
 * zak_autho_load_from_xml:
 * @zak_autho: an #ZakAutho object.
 * @xnode:
 * @replace:
 *
 */
gboolean
zak_autho_load_from_xml (ZakAutho *zak_autho, xmlNodePtr xnode, gboolean replace)
{
	gboolean ret;

	ZakAuthoPrivate *priv;

	xmlNodePtr current;
	xmlNodePtr current_parent;

	ZakAuthoIRole *irole;
	ZakAuthoIResource *iresource;
	gchar *prop;

	g_return_val_if_fail (IS_ZAK_AUTHO (zak_autho), FALSE);
	g_return_val_if_fail (xnode != NULL, FALSE);

	priv = ZAK_AUTHO_GET_PRIVATE (zak_autho);

	ret = TRUE;

	if (replace)
		{
			/* clearing current authorizations */
			zak_autho_clear (zak_autho);
		}

	if (xmlStrcmp (xnode->name, "zak_autho") != 0)
		{
			g_warning ("Invalid xml structure.");
			ret = FALSE;
		}
	else
		{
			current = xnode->children;
			while (current != NULL)
				{
					if (!xmlNodeIsText (current))
						{
							if (xmlStrcmp (current->name, "role") == 0)
								{
									prop = g_strstrip (g_strdup ((gchar *)xmlGetProp (current, "id")));
									if (g_strcmp0 (prop, "") != 0)
										{
											irole = ZAK_AUTHO_IROLE (zak_autho_role_new (prop));
											g_free (prop);
											zak_autho_add_role (zak_autho, irole);

											current_parent = current->children;
											while (current_parent != NULL)
												{
													if (!xmlNodeIsText (current_parent) &&
													    xmlStrcmp (current_parent->name, "parent") == 0)
														{
															prop = g_strstrip (g_strdup ((gchar *)xmlGetProp (current_parent, "id")));
															if (g_strcmp0 (prop, "") != 0)
																{
																	zak_autho_add_parent_to_role (zak_autho, irole,  zak_autho_get_role_from_id (zak_autho, prop));
																}
															g_free (prop);
														}
													current_parent = current_parent->next;
												}
										}
								}
							else if (xmlStrcmp (current->name, "resource") == 0)
								{
									prop = g_strstrip (g_strdup ((gchar *)xmlGetProp (current, "id")));
									if (g_strcmp0 (prop, "") != 0)
										{
											iresource = ZAK_AUTHO_IRESOURCE (zak_autho_resource_new (prop));
											zak_autho_add_resource (zak_autho, iresource);
											g_free (prop);

											current_parent = current->children;
											while (current_parent != NULL)
												{
													if (!xmlNodeIsText (current_parent) &&
													    xmlStrcmp (current_parent->name, "parent") == 0)
														{
															prop = g_strstrip (g_strdup ((gchar *)xmlGetProp (current_parent, "id")));
															if (g_strcmp0 (prop, "") != 0)
																{
																	zak_autho_add_parent_to_resource (zak_autho, iresource, zak_autho_get_resource_from_id (zak_autho, prop));
																}
															g_free (prop);
														}
													current_parent = current_parent->next;
												}
										}
								}
							else if (xmlStrcmp (current->name, "rule") == 0)
								{
									prop = g_strstrip (g_strdup ((gchar *)xmlGetProp (current, "role")));
									irole = zak_autho_get_role_from_id (zak_autho, prop);
									if (irole != NULL)
										{
											prop = g_strstrip (g_strdup ((gchar *)xmlGetProp (current, "resource")));
											if (g_strcmp0 (prop, "") == 0)
												{
													iresource = NULL;
												}
											else
												{
													iresource = zak_autho_get_resource_from_id (zak_autho, prop);
												}
											g_free (prop);

											prop = g_strstrip (g_strdup ((gchar *)xmlGetProp (current, "allow")));
											if (g_strcmp0 (prop, "yes") == 0)
												{
													zak_autho_allow (zak_autho, irole, iresource);
												}
											else
												{
													zak_autho_deny (zak_autho, irole, iresource);
												}
											g_free (prop);
										}
								}
						}

					current = current->next;
				}
		}

	return ret;
}

static gboolean
_zak_autho_delete_table_content (GdaConnection *gdacon, const gchar *table_prefix)
{
	gboolean ret;

	gchar *sql;
	GError *error;

	ret = TRUE;

	error = NULL;
	sql = g_strdup_printf ("DELETE FROM %sroles", table_prefix);
	gda_connection_execute_non_select_command (gdacon, sql, &error);
	g_free (sql);
	if (error != NULL)
		{
			g_warning ("Error on deleting roles table content: %s",
			           error->message != NULL ? error->message : "No details");
		}
	error = NULL;
	sql = g_strdup_printf ("DELETE FROM %sroles_parents", table_prefix);
	gda_connection_execute_non_select_command (gdacon, sql, &error);
	g_free (sql);
	if (error != NULL)
		{
			g_warning ("Error on deleting roles_parents table content: %s",
			           error->message != NULL ? error->message : "No details");
		}
	error = NULL;
	sql = g_strdup_printf ("DELETE FROM %sresources", table_prefix);
	gda_connection_execute_non_select_command (gdacon, sql, &error);
	g_free (sql);
	if (error != NULL)
		{
			g_warning ("Error on deleting resources table content: %s",
			           error->message != NULL ? error->message : "No details");
		}
	error = NULL;
	sql = g_strdup_printf ("DELETE FROM %sresources_parents", table_prefix);
	gda_connection_execute_non_select_command (gdacon, sql, &error);
	g_free (sql);
	if (error != NULL)
		{
			g_warning ("Error on deleting resources_parents table content: %s",
			           error->message != NULL ? error->message : "No details");
		}
	error = NULL;
	sql = g_strdup_printf ("DELETE FROM %srules", table_prefix);
	gda_connection_execute_non_select_command (gdacon, sql, &error);
	g_free (sql);
	if (error != NULL)
		{
			g_warning ("Error on deleting rules table content: %s",
			           error->message != NULL ? error->message : "No details");
		}

	return ret;
}

static guint
_zak_autho_find_new_table_id (GdaConnection *gdacon, const gchar *table_name)
{
	gchar *sql;
	GError *error;
	GdaDataModel *dm;

	guint new_id;

	error = NULL;
	sql = g_strdup_printf ("SELECT COALESCE (MAX (id), 0) FROM %s",
	                       table_name);
	dm = gda_connection_execute_select_command (gdacon, sql, &error);
	g_free (sql);
	if (dm != NULL && gda_data_model_get_n_rows (dm) == 1)
		{
			new_id = g_value_get_int (gda_data_model_get_value_at (dm, 0, 0, &error));
			new_id++;
		}
	else if (dm == NULL || error != NULL)
		{
			new_id = 0;
			g_warning ("Problem on getting new id from table «%s»: %s",
			           error->message != NULL ? error->message : "no details");
		}
	else
		{
			new_id = 1;
		}
	g_object_unref (dm);

	return new_id;
}

static guint
_zak_autho_get_role_id_db (GdaConnection *gdacon, const gchar *table_name, const gchar *role_id)
{
	gchar *sql;
	GError *error;
	GdaDataModel *dm;

	guint id;

	id = 0;

	error = NULL;
	sql = g_strdup_printf ("SELECT id FROM %s"
	                       " WHERE role_id = '%s'",
	                       table_name,
	                       role_id);
	dm = gda_connection_execute_select_command (gdacon, sql, &error);
	g_free (sql);
	if (dm != NULL && gda_data_model_get_n_rows (dm) == 1)
		{
			id = g_value_get_int (gda_data_model_get_value_at (dm, 0, 0, &error));
		}
	else if (error != NULL)
		{
			g_warning ("Problem on getting role id from table «%s»: %s",
			           error->message != NULL ? error->message : "no details");
		}
	g_object_unref (dm);

	return id;
}

static guint
_zak_autho_get_resource_id_db (GdaConnection *gdacon, const gchar *table_name, const gchar *resource_id)
{
	gchar *sql;
	GError *error;
	GdaDataModel *dm;

	guint id;

	id = 0;

	error = NULL;
	sql = g_strdup_printf ("SELECT id FROM %s"
	                       " WHERE resource_id = '%s'",
	                       table_name,
	                       resource_id);
	dm = gda_connection_execute_select_command (gdacon, sql, &error);
	g_free (sql);
	if (dm != NULL && gda_data_model_get_n_rows (dm) == 1)
		{
			id = g_value_get_int (gda_data_model_get_value_at (dm, 0, 0, &error));
		}
	else if (error != NULL)
		{
			g_warning ("Problem on getting resource id from table «%s»: %s",
			           error->message != NULL ? error->message : "no details");
		}
	g_object_unref (dm);

	return id;
}

/**
 * zak_autho_save_to_db:
 * @zak_autho: an #ZakAutho object.
 * @gdacon:
 * @table_prefix:
 * @replace:
 *
 */
gboolean
zak_autho_save_to_db (ZakAutho *zak_autho, GdaConnection *gdacon,
                  const gchar *table_prefix, gboolean replace)
{
	ZakAuthoPrivate *priv;

	gboolean ret;

	gboolean in_trans;

	gchar *prefix;

	gchar *sql;
	GError *error;

	GHashTableIter iter;
	gpointer key, value;

	Role *role;
	Resource *resource;
	Rule *rule;

	GList *parent;

	gchar *table_name;
	gchar *table_name_parent;

	guint new_id;
	guint id_parent;
	guint id_roles;
	guint id_resources;

	g_return_val_if_fail (IS_ZAK_AUTHO (zak_autho), FALSE);
	g_return_val_if_fail (GDA_IS_CONNECTION (gdacon), FALSE);

	priv = ZAK_AUTHO_GET_PRIVATE (zak_autho);

	ret = TRUE;

	error = NULL;
	in_trans = gda_connection_begin_transaction (gdacon, "zak_autho-save-to-db", 0, &error);
	if (!in_trans)
		{
			g_warning ("Error on starting transaction: %s",
			           error != NULL && error->message != NULL ? error->message : "No details");
		}

	if (table_prefix == NULL)
		{
			prefix = g_strdup ("");
		}
	else
		{
			prefix = g_strstrip (g_strdup (table_prefix));
		}

	if (replace)
		{
			/* deleting table's content */
			_zak_autho_delete_table_content (gdacon, prefix);
		}

	/* roles */
	table_name = g_strdup_printf ("%sroles", prefix);
	table_name_parent = g_strdup_printf ("%s_parents", table_name);
	g_hash_table_iter_init (&iter, priv->roles);
	while (g_hash_table_iter_next (&iter, &key, &value))
		{
			new_id = _zak_autho_find_new_table_id (gdacon, table_name);
			if (new_id <= 0)
				{
					ret = FALSE;
					break;
				}

			role = (Role *)value;

			error = NULL;
			sql = g_strdup_printf ("INSERT INTO %s"
			                       " (id, role_id)"
			                       " VALUES (%d, '%s')",
			                       table_name,
			                       new_id,
			                       zak_autho_irole_get_role_id (ZAK_AUTHO_IROLE (role->irole)));
			gda_connection_execute_non_select_command (gdacon, sql, &error);
			g_free (sql);
			if (error != NULL)
				{
					g_warning ("Error on saving role «%s»: %s",
					           zak_autho_irole_get_role_id (ZAK_AUTHO_IROLE (role->irole)),
					           error->message != NULL ? error->message : "no details");
					continue;
				}

			parent = role->parents;
			while (parent != NULL)
				{
					role = (Role *)parent->data;

					id_parent = _zak_autho_get_role_id_db (gdacon, table_name, zak_autho_irole_get_role_id (ZAK_AUTHO_IROLE (role->irole)));
					if (id_parent > 0)
						{
							error = NULL;
							sql = g_strdup_printf ("INSERT INTO %s"
							                       " (id_roles, id_roles_parent)"
							                       " VALUES (%d, %d)",
							                       table_name_parent,
							                       new_id,
							                       id_parent);
							gda_connection_execute_non_select_command (gdacon, sql, &error);
							g_free (sql);
							if (error != NULL)
								{
									g_warning ("Error on saving role parent «%s»: %s",
									           zak_autho_irole_get_role_id (ZAK_AUTHO_IROLE (role->irole)),
									           error->message != NULL ? error->message : "no details");
									continue;
								}
						}
					else
						{
							g_warning ("Unable to find parent role «%s»",
							           zak_autho_irole_get_role_id (ZAK_AUTHO_IROLE (role->irole)));
						}

					parent = g_list_next (parent);
				}
		}

	if (!ret)
		{
			g_warning ("Unable to save roles");
		}

	ret = TRUE;

	/* resources */
	table_name = g_strdup_printf ("%sresources", prefix);
	table_name_parent = g_strdup_printf ("%s_parents", table_name);
	g_hash_table_iter_init (&iter, priv->resources);
	while (g_hash_table_iter_next (&iter, &key, &value))
		{
			new_id = _zak_autho_find_new_table_id (gdacon, table_name);
			if (new_id <= 0)
				{
					ret = FALSE;
					break;
				}

			resource = (Resource *)value;

			error = NULL;
			sql = g_strdup_printf ("INSERT INTO %s"
			                       " (id, resource_id)"
			                       " VALUES (%d, '%s')",
			                       table_name,
			                       new_id,
			                       zak_autho_iresource_get_resource_id (ZAK_AUTHO_IRESOURCE (resource->iresource)));
			gda_connection_execute_non_select_command (gdacon, sql, &error);
			g_free (sql);
			if (error != NULL)
				{
					g_warning ("Error on saving resource «%s»: %s",
					           zak_autho_iresource_get_resource_id (ZAK_AUTHO_IRESOURCE (resource->iresource)),
					           error->message != NULL ? error->message : "no details");
					continue;
				}

			parent = resource->parents;
			while (parent != NULL)
				{
					resource = (Resource *)parent->data;

					id_parent = _zak_autho_get_resource_id_db (gdacon, table_name, zak_autho_iresource_get_resource_id (ZAK_AUTHO_IRESOURCE (resource->iresource)));
					if (id_parent > 0)
						{
							error = NULL;
							sql = g_strdup_printf ("INSERT INTO %s"
							                       " (id_resources, id_resources_parent)"
							                       " VALUES (%d, %d)",
							                       table_name_parent,
							                       new_id,
							                       id_parent);
							gda_connection_execute_non_select_command (gdacon, sql, &error);
							g_free (sql);
							if (error != NULL)
								{
									g_warning ("Error on saving resource parent «%s»: %s",
									           zak_autho_iresource_get_resource_id (ZAK_AUTHO_IRESOURCE (resource->iresource)),
									           error->message != NULL ? error->message : "no details");
									continue;
								}
						}
					else
						{
							g_warning ("Unable to find parent resource «%s»",
							           zak_autho_iresource_get_resource_id (ZAK_AUTHO_IRESOURCE (resource->iresource)));
						}

					parent = g_list_next (parent);
				}
		}

	if (!ret)
		{
			g_warning ("Unable to save resources");
		}

	/* rules allow */
	table_name = g_strdup_printf ("%srules", prefix);
	table_name_parent = g_strdup_printf ("%s_parents", table_name);
	g_hash_table_iter_init (&iter, priv->rules_allow);
	while (g_hash_table_iter_next (&iter, &key, &value))
		{
			new_id = _zak_autho_find_new_table_id (gdacon, table_name);
			if (new_id <= 0)
				{
					ret = FALSE;
					break;
				}

			rule = (Rule *)value;

			id_roles = _zak_autho_get_role_id_db (gdacon, g_strdup_printf ("%sroles", prefix), zak_autho_irole_get_role_id (ZAK_AUTHO_IROLE (rule->role->irole)));
			if (id_roles > 0)
				{
					if (rule->resource != NULL)
						{
							id_resources = _zak_autho_get_resource_id_db (gdacon, g_strdup_printf ("%sresources", prefix), zak_autho_iresource_get_resource_id (ZAK_AUTHO_IRESOURCE (rule->resource->iresource)));
						}
					else
						{
							id_resources = 0;
						}

					error = NULL;
					sql = g_strdup_printf ("INSERT INTO %s"
					                       " (id, type, id_roles, id_resources)"
					                       " VALUES (%d, 1, %d, %d)",
					                       table_name,
					                       new_id,
					                       id_roles,
					                       id_resources);
					gda_connection_execute_non_select_command (gdacon, sql, &error);
					g_free (sql);
					if (error != NULL)
						{
							g_warning ("Error on saving rule: %s",
							           error->message != NULL ? error->message : "no details");
						}
				}
		}

	/* rules deny */
	g_hash_table_iter_init (&iter, priv->rules_deny);
	while (g_hash_table_iter_next (&iter, &key, &value))
		{
			new_id = _zak_autho_find_new_table_id (gdacon, table_name);
			if (new_id <= 0)
				{
					ret = FALSE;
					break;
				}

			rule = (Rule *)value;

			id_roles = _zak_autho_get_role_id_db (gdacon, g_strdup_printf ("%sroles", prefix), zak_autho_irole_get_role_id (ZAK_AUTHO_IROLE (rule->role->irole)));
			if (id_roles > 0)
				{
					if (rule->resource != NULL)
						{
							id_resources = _zak_autho_get_resource_id_db (gdacon, g_strdup_printf ("%sresources", prefix), zak_autho_iresource_get_resource_id (ZAK_AUTHO_IRESOURCE (rule->resource->iresource)));
						}
					else
						{
							id_resources = 0;
						}

					error = NULL;
					sql = g_strdup_printf ("INSERT INTO %s"
					                       " (id, type, id_roles, id_resources)"
					                       " VALUES (%d, 2, %d, %d)",
					                       table_name,
					                       new_id,
					                       id_roles,
					                       id_resources);
					gda_connection_execute_non_select_command (gdacon, sql, &error);
					g_free (sql);
					if (error != NULL)
						{
							g_warning ("Error on saving rule: %s",
							           error->message != NULL ? error->message : "no details");
						}
				}
		}

	error = NULL;
	if (in_trans && !gda_connection_commit_transaction (gdacon, "zak_autho-save-to-db", &error))
		{
			g_warning ("Error on committing transaction: %s",
			           error != NULL && error->message != NULL ? error->message : "No details");
		}
	else
		{
			ret = TRUE;
		}

	return ret;
}

/**
 * zak_autho_load_from_db:
 * @zak_autho: an #ZakAutho object.
 * @gdacon:
 * @table_prefix:
 * @replace:
 *
 */
gboolean
zak_autho_load_from_db (ZakAutho *zak_autho, GdaConnection *gdacon, const gchar *table_prefix, gboolean replace)
{
	ZakAuthoPrivate *priv;

	gboolean ret;

	gchar *prefix;

	gchar *sql;
	GError *error;
	GdaDataModel *dm;

	const GValue *gval;
	gchar *role_id;
	gchar *resource_id;
	guint rule_type;

	ZakAuthoIRole *irole;
	ZakAuthoIRole *irole_parent;
	ZakAuthoIResource *iresource;
	ZakAuthoIResource *iresource_parent;
	Rule *rule;

	guint row;
	guint rows;

	g_return_val_if_fail (IS_ZAK_AUTHO (zak_autho), FALSE);
	g_return_val_if_fail (GDA_IS_CONNECTION (gdacon), FALSE);

	priv = ZAK_AUTHO_GET_PRIVATE (zak_autho);

	priv->on_loading = TRUE;

	ret = TRUE;

	if (replace)
		{
			/* clearing current authorizations */
			zak_autho_clear (zak_autho);
		}

	if (table_prefix == NULL)
		{
			prefix = g_strdup ("");
		}
	else
		{
			prefix = g_strstrip (g_strdup (table_prefix));
		}

	/* roles */
	error = NULL;
	sql = g_strdup_printf ("SELECT role_id FROM %sroles ORDER BY id",
	                       prefix);
	dm = gda_connection_execute_select_command (gdacon, sql, &error);
	g_free (sql);
	if (dm != NULL && error == NULL)
		{
			rows = gda_data_model_get_n_rows (dm);
			for (row = 0; row < rows; row++)
				{
					error = NULL;
					irole = ZAK_AUTHO_IROLE (zak_autho_role_new (gda_value_stringify (gda_data_model_get_value_at (dm, 0, row, &error))));
					zak_autho_add_role (zak_autho, irole);
				}
		}
	else if (error != NULL)
		{
			g_warning ("Error on reading table «%sroles»: %s",
			           prefix,
			           error->message != NULL ? error->message : "no details");
		}
	g_object_unref (dm);

	/* roles parents */
	error = NULL;
	sql = g_strdup_printf ("SELECT r1.role_id, r2.role_id"
	                       " FROM %sroles_parents AS rp"
	                       " INNER JOIN %sroles AS r1 ON rp.id_roles = r1.id"
	                       " INNER JOIN %sroles AS r2 ON rp.id_roles_parent = r2.id"
	                       " ORDER BY rp.id_roles, rp.id_roles_parent",
	                       prefix,
	                       prefix,
	                       prefix);
	dm = gda_connection_execute_select_command (gdacon, sql, &error);
	g_free (sql);
	if (dm != NULL && error == NULL)
		{
			rows = gda_data_model_get_n_rows (dm);
			for (row = 0; row < rows; row++)
				{
					error = NULL;
					irole = ZAK_AUTHO_IROLE (zak_autho_role_new (gda_value_stringify (gda_data_model_get_value_at (dm, 0, row, &error))));
					error = NULL;
					irole_parent = ZAK_AUTHO_IROLE (zak_autho_role_new (gda_value_stringify (gda_data_model_get_value_at (dm, 1, row, &error))));
					zak_autho_add_parent_to_role (zak_autho, irole, irole_parent);
				}
		}
	else if (error != NULL)
		{
			g_warning ("Error on reading table «%sroles_parents»: %s",
			           prefix,
			           error->message != NULL ? error->message : "no details");
		}
	g_object_unref (dm);

	/* resources */
	error = NULL;
	sql = g_strdup_printf ("SELECT resource_id FROM %sresources ORDER BY id",
	                       prefix);
	dm = gda_connection_execute_select_command (gdacon, sql, &error);
	g_free (sql);
	if (dm != NULL && error == NULL)
		{
			rows = gda_data_model_get_n_rows (dm);
			for (row = 0; row < rows; row++)
				{
					error = NULL;
					iresource = ZAK_AUTHO_IRESOURCE (zak_autho_resource_new (gda_value_stringify (gda_data_model_get_value_at (dm, 0, row, &error))));
					zak_autho_add_resource (zak_autho, iresource);
				}
		}
	else if (error != NULL)
		{
			g_warning ("Error on reading table «%sresources»: %s",
			           prefix,
			           error->message != NULL ? error->message : "no details");
		}
	g_object_unref (dm);

	/* resources parents */
	error = NULL;
	sql = g_strdup_printf ("SELECT r1.resource_id, r2.resource_id"
	                       " FROM %sresources_parents AS rp"
	                       " INNER JOIN %sresources AS r1 ON rp.id_resources = r1.id"
	                       " INNER JOIN %sresources AS r2 ON rp.id_resources_parent = r2.id"
	                       " ORDER BY rp.id_resources, rp.id_resources_parent",
	                       prefix,
	                       prefix,
	                       prefix);
	dm = gda_connection_execute_select_command (gdacon, sql, &error);
	g_free (sql);
	if (dm != NULL && error == NULL)
		{
			rows = gda_data_model_get_n_rows (dm);
			for (row = 0; row < rows; row++)
				{
					error = NULL;
					iresource = ZAK_AUTHO_IRESOURCE (zak_autho_resource_new (gda_value_stringify (gda_data_model_get_value_at (dm, 0, row, &error))));
					error = NULL;
					iresource_parent = ZAK_AUTHO_IRESOURCE (zak_autho_resource_new (gda_value_stringify (gda_data_model_get_value_at (dm, 1, row, &error))));
					zak_autho_add_parent_to_resource (zak_autho, iresource, iresource_parent);
				}
		}
	else if (error != NULL)
		{
			g_warning ("Error on reading table «%sresources_parents»: %s",
			           prefix,
			           error->message != NULL ? error->message : "no details");
		}
	g_object_unref (dm);

	/* rules */
	error = NULL;
	sql = g_strdup_printf ("SELECT ru.type, ro.role_id, re.resource_id"
	                       " FROM %srules AS ru"
	                       " LEFT JOIN %sroles AS ro ON ru.id_roles = ro.id"
	                       " LEFT JOIN %sresources AS re ON ru.id_resources = re.id",
	                       prefix,
	                       prefix,
	                       prefix);
	dm = gda_connection_execute_select_command (gdacon, sql, &error);
	g_free (sql);
	if (dm != NULL && error == NULL)
		{
			rows = gda_data_model_get_n_rows (dm);
			for (row = 0; row < rows; row++)
				{
					error = NULL;
					gval = gda_data_model_get_value_at (dm, 1, row, &error);
					if (gval == NULL || error != NULL)
						{
							g_warning ("Error on reading role_id value: %s",
							           error != NULL && error->message != NULL ? error->message : "no details");
						}
					else if (gval != NULL && error == NULL && !gda_value_is_null (gval))
						{
							role_id = gda_value_stringify (gval);
							irole = zak_autho_get_role_from_id (zak_autho, role_id);
							if (irole != NULL)
								{
									error = NULL;
									gval = gda_data_model_get_value_at (dm, 2, row, &error);
									if (gval == NULL || gda_value_is_null (gval))
										{
											if (error != NULL)
												{
													g_warning ("Error on reading resource value: %s",
													           error != NULL && error->message != NULL ? error->message : "no details");
												}
											iresource = NULL;
										}
									else
										{
											resource_id = gda_value_stringify (gval);
											iresource = zak_autho_get_resource_from_id (zak_autho, resource_id);
										}

									error = NULL;
									gval = gda_data_model_get_value_at (dm, 0, row, &error);
									if (gval == NULL || error != NULL)
										{
											g_warning ("Error on reading type value: %s",
											           error != NULL && error->message != NULL ? error->message : "no details");
										}
									else if (gval != NULL && error == NULL && !gda_value_is_null (gval))
										{
											rule_type = g_value_get_int (gval);
											if (rule_type == 1)
												{
													zak_autho_allow (zak_autho, irole, iresource);
												}
											else if (rule_type == 2)
												{
													zak_autho_deny (zak_autho, irole, iresource);
												}
											else
												{
													g_warning ("Rule type %d not admitted", rule_type);
												}
										}
								}
						}
				}
		}
	else if (error != NULL)
		{
			g_warning ("Error on reading table «%srules»: %s",
			           prefix,
			           error->message != NULL ? error->message : "no details");
		}
	g_object_unref (dm);

	g_free (prefix);

	if (priv->gdt_last_load != NULL)
		{
			g_date_time_unref (priv->gdt_last_load);
		}
	priv->gdt_last_load = g_date_time_new_now_local ();

	priv->on_loading = FALSE;

	return ret;
}

gboolean
zak_autho_load_from_db_with_monitor (ZakAutho *zak_autho, GdaConnection *gdacon, const gchar *table_prefix, gboolean replace)
{
	ZakAuthoPrivate *priv = ZAK_AUTHO_GET_PRIVATE (zak_autho);

	g_return_val_if_fail (IS_ZAK_AUTHO (zak_autho), FALSE);
	g_return_val_if_fail (GDA_IS_CONNECTION (gdacon), FALSE);

	priv->gdacon = gdacon;
	if (table_prefix == NULL)
		{
			if (priv->table_prefix != NULL)
				{
					g_free (priv->table_prefix);
				}
			priv->table_prefix = g_strdup ("");
		}
	else
		{
			priv->table_prefix = g_strdup (table_prefix);
		}

	zak_autho_load_from_db (zak_autho, gdacon, table_prefix, replace);
}

static void
_zak_autho_check_updated (ZakAutho *zak_autho)
{
	GError *error;
	gchar *sql;
	GdaDataModel *dm;

	const GValue *gval;
	const GdaTimestamp *gda_timestamp;
	GDateTime *gda_datetime;

	ZakAuthoPrivate *priv = ZAK_AUTHO_GET_PRIVATE (zak_autho);

	g_return_if_fail (IS_ZAK_AUTHO (zak_autho));

	if (priv->on_loading)
		{
			return;
		}

	if (!GDA_IS_CONNECTION (priv->gdacon))
		{
			return;
		}

	error = NULL;
	sql = g_strdup_printf ("SELECT update FROM %stimestamp_update",
	                       priv->table_prefix);
	dm = gda_connection_execute_select_command (priv->gdacon, sql, &error);
	g_free (sql);
	if (dm != NULL && error == NULL && gda_data_model_get_n_rows (dm) > 0)
		{
			gval = gda_data_model_get_value_at (dm, 0, 0, NULL);
			if (!gda_value_is_null (gval))
				{
					gda_timestamp = gda_value_get_timestamp (gval);
					gda_datetime = g_date_time_new_local (gda_timestamp->year,
					                                      gda_timestamp->month,
					                                      gda_timestamp->day,
					                                      gda_timestamp->hour,
					                                      gda_timestamp->minute,
					                                      gda_timestamp->second);

					if (g_date_time_compare (priv->gdt_last_load, gda_datetime) < 0)
						{
							/* to reload */
							zak_autho_load_from_db_with_monitor (zak_autho, priv->gdacon, priv->table_prefix, TRUE);
						}
					g_date_time_unref (gda_datetime);
				}
		}
	else if (error != NULL)
		{
			g_warning ("Error on reading table «%stimestamp_update»: %s",
			           priv->table_prefix,
			           error->message != NULL ? error->message : "no details");
		}
	g_object_unref (dm);
}

/* PRIVATE */
static void
zak_autho_set_property (GObject *object,
                    guint property_id,
                    const GValue *value,
                    GParamSpec *pspec)
{
	ZakAutho *zak_autho = (ZakAutho *)object;
	ZakAuthoPrivate *priv = ZAK_AUTHO_GET_PRIVATE (zak_autho);

	switch (property_id)
		{
			default:
				G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
				break;
	  }
}

static void
zak_autho_get_property (GObject *object,
                    guint property_id,
                    GValue *value,
                    GParamSpec *pspec)
{
	ZakAutho *zak_autho = (ZakAutho *)object;
	ZakAuthoPrivate *priv = ZAK_AUTHO_GET_PRIVATE (zak_autho);

	switch (property_id)
		{
			default:
				G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
				break;
	  }
}
