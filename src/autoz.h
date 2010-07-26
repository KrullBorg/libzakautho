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

#ifndef __LIBAUTOZ_H__
#define __LIBAUTOZ_H__

#include <glib.h>
#include <glib-object.h>

#include <libxml/tree.h>
#include <libgda/libgda.h>


G_BEGIN_DECLS


#include "role_interface.h"
#include "resource_interface.h"

#define TYPE_AUTOZ                 (autoz_get_type ())
#define AUTOZ(obj)                 (G_TYPE_CHECK_INSTANCE_CAST ((obj), TYPE_AUTOZ, Autoz))
#define AUTOZ_CLASS(klass)         (G_TYPE_CHECK_CLASS_CAST ((klass), TYPE_AUTOZ, AutozClass))
#define IS_AUTOZ(obj)              (G_TYPE_CHECK_INSTANCE_TYPE ((obj), TYPE_AUTOZ))
#define IS_AUTOZ_CLASS(klass)      (G_TYPE_CHECK_CLASS_TYPE ((klass), TYPE_AUTOZ))
#define AUTOZ_GET_CLASS(obj)       (G_TYPE_INSTANCE_GET_CLASS ((obj), TYPE_AUTOZ, AutozClass))


typedef struct _Autoz Autoz;
typedef struct _AutozClass AutozClass;

struct _Autoz
	{
		GObject parent;
	};

struct _AutozClass
	{
		GObjectClass parent_class;
	};

GType autoz_get_type (void) G_GNUC_CONST;


Autoz *autoz_new (void);

void autoz_add_role (Autoz *autoz, AutozIRole *irole);
void autoz_add_role_with_parents (Autoz *autoz, AutozIRole *irole, ...);
void autoz_add_parent_to_role (Autoz *autoz, AutozIRole *irole, AutozIRole *irole_parent);
void autoz_add_parents_to_role (Autoz *autoz, AutozIRole *irole, ...);

AutozIRole *autoz_get_role_from_id (Autoz *autoz, const gchar *role_id);

void autoz_add_resource (Autoz *autoz, AutozIResource *iresource);
void autoz_add_resource_with_parents (Autoz *autoz, AutozIResource *iresource, ...);
void autoz_add_parent_to_resource (Autoz *autoz, AutozIResource *iresource, AutozIResource *iresource_parent);
void autoz_add_parents_to_resource (Autoz *autoz, AutozIResource *iresource, ...);

AutozIResource *autoz_get_resource_from_id (Autoz *autoz, const gchar *resource_id);

void autoz_allow (Autoz *autoz, AutozIRole *irole, AutozIResource *iresource);
void autoz_deny (Autoz *autoz, AutozIRole *irole, AutozIResource *iresource);

gboolean autoz_is_allowed (Autoz *autoz, AutozIRole *irole, AutozIResource *iresource);

gboolean autoz_clear (Autoz *autoz);

xmlNodePtr autoz_get_xml (Autoz *autoz);
gboolean autoz_load_from_xml (Autoz *autoz, xmlNodePtr xnode, gboolean replace);

gboolean autoz_save_to_db (Autoz *autoz, GdaConnection *gdacon, const gchar *table_prefix, gboolean replace);
gboolean autoz_load_from_db (Autoz *autoz, GdaConnection *gdacon, const gchar *table_prefix, gboolean replace);


G_END_DECLS


#endif /* __LIBAUTOZ_H__ */
