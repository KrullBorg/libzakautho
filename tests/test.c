/*
 * Copyright (C) 2010 Andrea Zagli <azagli@libero.it>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#include "autoz.h"
#include "role.h"
#include "resource.h"

int
main (int argc, char **argv)
{
	Autoz *autoz;
	AutozRole *role_writer;
	AutozRole *role_read_only;
	AutozResource *resource;

	g_type_init ();

	autoz = autoz_new ();

	role_writer = autoz_role_new ("writer");
	autoz_add_role (autoz, AUTOZ_IROLE (role_writer));

	role_read_only = autoz_role_new ("read-only");
	autoz_add_role (autoz, AUTOZ_IROLE (role_read_only));

	resource = autoz_resource_new ("page");
	autoz_add_resource (autoz, AUTOZ_IRESOURCE (resource));

	autoz_allow (autoz, AUTOZ_IROLE (role_writer), AUTOZ_IRESOURCE (resource));

	if (autoz_is_allowed (autoz, AUTOZ_IROLE (role_writer), AUTOZ_IRESOURCE (resource)))
		{
			g_message ("writer allowed to page.");
		}
	if (!autoz_is_allowed (autoz, AUTOZ_IROLE (role_read_only), AUTOZ_IRESOURCE (resource)))
		{
			g_message ("read-only not allowed to page.");
		}

	return 0;
}
