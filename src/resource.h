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

#ifndef __LIB_ZAK_AUTHO_RESOURCE_H__
#define __LIB_ZAK_AUTHO_RESOURCE_H__

#include <glib.h>
#include <glib-object.h>


G_BEGIN_DECLS


#define ZAK_AUTHO_TYPE_RESOURCE                 (zak_autho_resource_get_type ())
#define ZAK_AUTHO_RESOURCE(obj)                 (G_TYPE_CHECK_INSTANCE_CAST ((obj), ZAK_AUTHO_TYPE_RESOURCE, ZakAuthoResource))
#define ZAK_AUTHO_RESOURCE_CLASS(klass)         (G_TYPE_CHECK_CLASS_CAST ((klass), ZAK_AUTHO_TYPE_RESOURCE, ZakAuthoResourceClass))
#define ZAK_AUTHO_IS_RESOURCE(obj)              (G_TYPE_CHECK_INSTANCE_TYPE ((obj), ZAK_AUTHO_TYPE_RESOURCE))
#define ZAK_AUTHO_IS_RESOURCE_CLASS(klass)      (G_TYPE_CHECK_CLASS_TYPE ((klass), ZAK_AUTHO_TYPE_RESOURCE))
#define ZAK_AUTHO_RESOURCE_GET_CLASS(obj)       (G_TYPE_INSTANCE_GET_CLASS ((obj), ZAK_AUTHO_TYPE_RESOURCE, ZakAuthoResourceClass))


typedef struct _ZakAuthoResource ZakAuthoResource;
typedef struct _ZakAuthoResourceClass ZakAuthoResourceClass;

struct _ZakAuthoResource
	{
		GObject parent;
	};

struct _ZakAuthoResourceClass
	{
		GObjectClass parent_class;
	};

GType zak_autho_resource_get_type (void) G_GNUC_CONST;


ZakAuthoResource *zak_autho_resource_new (const gchar *resource_id);


G_END_DECLS


#endif /* __LIBZAK_AUTHO_RESOURCE_H__ */
