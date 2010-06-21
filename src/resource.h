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

#ifndef __LIBAUTOZ_RESOURCE_H__
#define __LIBAUTOZ_RESOURCE_H__

#include <glib.h>
#include <glib-object.h>


G_BEGIN_DECLS


#define TYPE_AUTOZ_RESOURCE                 (autoz_resource_get_type ())
#define AUTOZ_RESOURCE(obj)                 (G_TYPE_CHECK_INSTANCE_CAST ((obj), TYPE_AUTOZ_RESOURCE, AutozResource))
#define AUTOZ_RESOURCE_CLASS(klass)         (G_TYPE_CHECK_CLASS_CAST ((klass), TYPE_AUTOZ_RESOURCE, AutozResourceClass))
#define IS_AUTOZ_RESOURCE(obj)              (G_TYPE_CHECK_INSTANCE_TYPE ((obj), TYPE_AUTOZ_RESOURCE))
#define IS_AUTOZ_RESOURCE_CLASS(klass)      (G_TYPE_CHECK_CLASS_TYPE ((klass), TYPE_AUTOZ_RESOURCE))
#define AUTOZ_RESOURCE_GET_CLASS(obj)       (G_TYPE_INSTANCE_GET_CLASS ((obj), TYPE_AUTOZ_RESOURCE, AutozResourceClass))


typedef struct _AutozResource AutozResource;
typedef struct _AutozResourceClass AutozResourceClass;

struct _AutozResource
	{
		GObject parent;
	};

struct _AutozResourceClass
	{
		GObjectClass parent_class;
	};

GType autoz_resource_get_type (void) G_GNUC_CONST;


AutozResource *autoz_resource_new (void);


G_END_DECLS


#endif /* __LIBAUTOZ_RESOURCE_H__ */
