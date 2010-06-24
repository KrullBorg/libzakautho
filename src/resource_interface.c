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

#include "resource_interface.h"

typedef AutozIResourceIface AutozIResourceInterface;
G_DEFINE_INTERFACE (AutozIResource, autoz_iresource, G_TYPE_OBJECT)

static void
autoz_iresource_default_init (AutozIResourceInterface *iface)
{
}

const gchar
*autoz_iresource_get_resource_id (AutozIResource *iresource)
{
	AutozIResourceIface *iface;

	gchar *ret;

	ret = NULL;

	g_return_val_if_fail (AUTOZ_IS_IRESOURCE (iresource), ret);

	iface = AUTOZ_IRESOURCE_GET_IFACE (iresource);

	if (iface->get_resource_id)
		{
			ret = (gchar *)(* iface->get_resource_id) (iresource);
		}

	return ret;
}
