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

#ifndef __LIB_ZAK_AUTHO_IRESOURCE_H__
#define __LIB_ZAK_AUTHO_IRESOURCE_H__

#include <glib-object.h>


G_BEGIN_DECLS


#define ZAK_AUTHO_TYPE_IRESOURCE             (zak_autho_iresource_get_type ())
#define ZAK_AUTHO_IRESOURCE(obj)             (G_TYPE_CHECK_INSTANCE_CAST ((obj), ZAK_AUTHO_TYPE_IRESOURCE, ZakAuthoIResource))
#define ZAK_AUTHO_IS_IRESOURCE(obj)          (G_TYPE_CHECK_INSTANCE_TYPE ((obj), ZAK_AUTHO_TYPE_IRESOURCE))
#define ZAK_AUTHO_IRESOURCE_GET_IFACE(obj)   (G_TYPE_INSTANCE_GET_INTERFACE ((obj), ZAK_AUTHO_TYPE_IRESOURCE, ZakAuthoIResourceIface))


typedef struct _ZakAuthoIResource ZakAuthoIResource;
typedef struct _ZakAuthoIResourceIface ZakAuthoIResourceIface;

struct _ZakAuthoIResourceIface
	{
		GTypeInterface g_iface;

		const gchar *(*get_resource_id) (ZakAuthoIResource *iresource);
	};

GType zak_autho_iresource_get_type (void) G_GNUC_CONST;

const gchar *zak_autho_iresource_get_resource_id (ZakAuthoIResource *iresource);


G_END_DECLS


#endif /* __LIB_ZAK_AUTHO_IRESOURCE_H__ */
