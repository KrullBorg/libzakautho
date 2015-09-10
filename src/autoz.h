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

#ifndef __LIB_ZAK_AUTHO_H__
#define __LIB_ZAK_AUTHO_H__

#include <glib.h>
#include <glib-object.h>

#include <libxml/tree.h>
#include <libgda/libgda.h>


G_BEGIN_DECLS


#include "role_interface.h"
#include "resource_interface.h"

#define ZAK_TYPE_AUTHO                 (zak_autho_get_type ())
#define ZAK_AUTHO(obj)                 (G_TYPE_CHECK_INSTANCE_CAST ((obj), ZAK_TYPE_AUTHO, ZakAutho))
#define ZAK_AUTHO_CLASS(klass)         (G_TYPE_CHECK_CLASS_CAST ((klass), ZAK_TYPE_AUTHO, ZakAuthoClass))
#define IS_ZAK_AUTHO(obj)              (G_TYPE_CHECK_INSTANCE_TYPE ((obj), ZAK_TYPE_AUTHO))
#define IS_ZAK_AUTHO_CLASS(klass)      (G_TYPE_CHECK_CLASS_TYPE ((klass), ZAK_TYPE_AUTHO))
#define ZAK_AUTHO_GET_CLASS(obj)       (G_TYPE_INSTANCE_GET_CLASS ((obj), ZAK_TYPE_AUTHO, ZakAuthoClass))


typedef struct _ZakAutho ZakAutho;
typedef struct _ZakAuthoClass ZakAuthoClass;

struct _ZakAutho
	{
		GObject parent;
	};

struct _ZakAuthoClass
	{
		GObjectClass parent_class;
	};

GType zak_autho_get_type (void) G_GNUC_CONST;


ZakAutho *zak_autho_new (void);

void zak_autho_set_role_name_prefix (ZakAutho *zak_autho, const gchar *prefix);
const gchar *zak_autho_get_role_name_prefix (ZakAutho *zak_autho);
void zak_autho_set_resource_name_prefix (ZakAutho *zak_autho, const gchar *prefix);
const gchar *zak_autho_get_resource_name_prefix (ZakAutho *zak_autho);

void zak_autho_add_role (ZakAutho *zak_autho, ZakAuthoIRole *irole);
void zak_autho_add_role_with_parents (ZakAutho *zak_autho, ZakAuthoIRole *irole, ...);
void zak_autho_add_parent_to_role (ZakAutho *zak_autho, ZakAuthoIRole *irole, ZakAuthoIRole *irole_parent);
void zak_autho_add_parents_to_role (ZakAutho *zak_autho, ZakAuthoIRole *irole, ...);

gboolean zak_autho_role_is_child (ZakAutho *zak_autho, ZakAuthoIRole *irole, ZakAuthoIRole *irole_parent);

ZakAuthoIRole *zak_autho_get_role_from_id (ZakAutho *zak_autho, const gchar *role_id);

void zak_autho_add_resource (ZakAutho *zak_autho, ZakAuthoIResource *iresource);
void zak_autho_add_resource_with_parents (ZakAutho *zak_autho, ZakAuthoIResource *iresource, ...);
void zak_autho_add_parent_to_resource (ZakAutho *zak_autho, ZakAuthoIResource *iresource, ZakAuthoIResource *iresource_parent);
void zak_autho_add_parents_to_resource (ZakAutho *zak_autho, ZakAuthoIResource *iresource, ...);

gboolean zak_autho_resource_is_child (ZakAutho *zak_autho, ZakAuthoIResource *iresource, ZakAuthoIResource *iresource_parent);

ZakAuthoIResource *zak_autho_get_resource_from_id (ZakAutho *zak_autho, const gchar *resource_id);

void zak_autho_allow (ZakAutho *zak_autho, ZakAuthoIRole *irole, ZakAuthoIResource *iresource);
void zak_autho_deny (ZakAutho *zak_autho, ZakAuthoIRole *irole, ZakAuthoIResource *iresource);

gboolean zak_autho_is_allowed (ZakAutho *zak_autho, ZakAuthoIRole *irole, ZakAuthoIResource *iresource, gboolean exclude_null);

gboolean zak_autho_clear (ZakAutho *zak_autho);

xmlNodePtr zak_autho_get_xml (ZakAutho *zak_autho);
gboolean zak_autho_load_from_xml (ZakAutho *zak_autho, xmlNodePtr xnode, gboolean replace);

gboolean zak_autho_save_to_db (ZakAutho *zak_autho, GdaConnection *gdacon, const gchar *table_prefix, gboolean replace);
gboolean zak_autho_load_from_db (ZakAutho *zak_autho, GdaConnection *gdacon, const gchar *table_prefix, gboolean replace);
gboolean zak_autho_load_from_db_with_monitor (ZakAutho *zak_autho, GdaConnection *gdacon, const gchar *table_prefix, gboolean replace);


G_END_DECLS


#endif /* __LIB_ZAK_AUTHO_H__ */
