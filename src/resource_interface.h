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

#ifndef __LIBAUTOZ_IRESOURCE_H__
#define __LIBAUTOZ_IRESOURCE_H__

#include <glib-object.h>


G_BEGIN_DECLS


#define AUTOZ_TYPE_IRESOURCE             (autoz_iresource_get_type ())
#define AUTOZ_IRESOURCE(obj)             (G_TYPE_CHECK_INSTANCE_CAST ((obj), AUTOZ_TYPE_IRESOURCE, AutozIResource))
#define AUTOZ_IS_IRESOURCE(obj)          (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AUTOZ_TYPE_IRESOURCE))
#define AUTOZ_IRESOURCE_GET_IFACE(obj)   (G_TYPE_INSTANCE_GET_INTERFACE ((obj), AUTOZ_TYPE_IRESOURCE, AutozIResourceIface))


typedef struct _AutozIResource AutozIResource;
typedef struct _AutozIResourceIface AutozIResourceIface;

struct _AutozIResourceIface
	{
		GTypeInterface g_iface;

		const gchar *(*get_resource_id) (AutozIResource *iresource);
	};

GType autoz_iresource_get_type (void) G_GNUC_CONST;

const gchar *autoz_iresource_get_resource_id (AutozIResource *iresource);


G_END_DECLS


#endif /* __LIBAUTOZ_IRESOURCE_H__ */
