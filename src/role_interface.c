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

#include "role_interface.h"

typedef AutozIRoleIface AutozIRoleInterface;
G_DEFINE_INTERFACE (AutozIRole, autoz_irole, G_TYPE_OBJECT)

static void
autoz_irole_default_init (AutozIRoleInterface *iface)
{
}

const gchar
*autoz_irole_get_role_id (AutozIRole *irole)
{
	AutozIRoleIface *iface;

	gchar *ret;

	ret = NULL;

	g_return_val_if_fail (AUTOZ_IS_IROLE (irole), ret);

	iface = AUTOZ_IROLE_GET_IFACE (irole);

	if (iface->get_role_id)
		{
			ret = (gchar *)(* iface->get_role_id) (irole);
		}

	return ret;
}
