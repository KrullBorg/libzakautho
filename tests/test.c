/*
 * Copyright (C) 2010-2015 Andrea Zagli <azagli@libero.it>
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

#include <glib/gprintf.h>

#include <libxml/tree.h>

#include "autoz.h"
#include "role.h"
#include "resource.h"

int
main (int argc, char **argv)
{
	ZakAutho *zak_autho;
	ZakAuthoRole *role_writer;
	ZakAuthoRole *role_writer_child;
	ZakAuthoRole *role_read_only;
	ZakAuthoResource *resource_page;

	xmlDocPtr xdoc;
	xmlNodePtr xnode;

	zak_autho = zak_autho_new ();

	zak_autho_add_role (zak_autho, ZAK_AUTHO_IROLE (zak_autho_role_new ("super-admin")));

	zak_autho_allow (zak_autho,
		zak_autho_get_role_from_id (zak_autho, "super-admin"),
		NULL);

	role_writer = zak_autho_role_new ("writer");
	zak_autho_add_role (zak_autho, ZAK_AUTHO_IROLE (role_writer));

	role_writer_child = zak_autho_role_new ("writer-child");
	zak_autho_add_role_with_parents (zak_autho, ZAK_AUTHO_IROLE (role_writer_child),
	                             ZAK_AUTHO_IROLE (role_writer),
	                             NULL);

	role_read_only = zak_autho_role_new ("read-only");
	zak_autho_add_role (zak_autho, ZAK_AUTHO_IROLE (role_read_only));

	resource_page = zak_autho_resource_new ("page");
	zak_autho_add_resource (zak_autho, ZAK_AUTHO_IRESOURCE (resource_page));

	zak_autho_add_resource_with_parents (zak_autho,
		ZAK_AUTHO_IRESOURCE (zak_autho_resource_new ("paragraph")),
		zak_autho_get_resource_from_id (zak_autho, "page"),
		NULL);

	zak_autho_allow (zak_autho, ZAK_AUTHO_IROLE (role_writer), ZAK_AUTHO_IRESOURCE (resource_page));

	zak_autho_deny (zak_autho, ZAK_AUTHO_IROLE (role_writer_child), ZAK_AUTHO_IRESOURCE (zak_autho_get_resource_from_id (zak_autho, "paragraph")));

	/* get xml */
	xnode = zak_autho_get_xml (zak_autho);
	if (xnode != NULL)
		{
			xdoc = xmlNewDoc ("1.0");
			xmlDocSetRootElement (xdoc, xnode);
			g_fprintf (stdout, "\n");
			xmlSaveFormatFile ("-", xdoc, 2);
			g_fprintf (stdout, "\n");
		}

	g_message ("super-admin %s allowed to page.",
	           (zak_autho_is_allowed (zak_autho, zak_autho_get_role_from_id (zak_autho, "super-admin"), ZAK_AUTHO_IRESOURCE (resource_page), FALSE) ? "is" : "isn't"));
	g_message ("super-admin %s allowed to paragraph.",
	           (zak_autho_is_allowed (zak_autho, zak_autho_get_role_from_id (zak_autho, "super-admin"), ZAK_AUTHO_IRESOURCE (zak_autho_get_resource_from_id (zak_autho, "paragraph")), FALSE) ? "is" : "isn't"));
	g_message ("writer %s allowed to page.",
	           (zak_autho_is_allowed (zak_autho, ZAK_AUTHO_IROLE (role_writer), ZAK_AUTHO_IRESOURCE (resource_page), FALSE) ? "is" : "isn't"));
	g_message ("writer-child %s allowed to page.",
	           (zak_autho_is_allowed (zak_autho, ZAK_AUTHO_IROLE (role_writer_child), ZAK_AUTHO_IRESOURCE (resource_page), FALSE) ? "is" : "isn't"));
	g_message ("writer %s allowed to paragraph.",
	           (zak_autho_is_allowed (zak_autho, ZAK_AUTHO_IROLE (role_writer), ZAK_AUTHO_IRESOURCE (zak_autho_get_resource_from_id (zak_autho, "paragraph")), FALSE) ? "is" : "isn't"));
	g_message ("writer-child %s allowed to paragraph.",
	           (zak_autho_is_allowed (zak_autho, ZAK_AUTHO_IROLE (role_writer_child), ZAK_AUTHO_IRESOURCE (zak_autho_get_resource_from_id (zak_autho, "paragraph")), FALSE) ? "is" : "isn't"));
	g_message ("read-only %s allowed to page.",
	           (zak_autho_is_allowed (zak_autho, ZAK_AUTHO_IROLE (role_read_only), ZAK_AUTHO_IRESOURCE (resource_page), FALSE) ? "is" : "isn't"));
	g_message ("read-only %s allowed to paragraph.",
	           (zak_autho_is_allowed (zak_autho, ZAK_AUTHO_IROLE (role_read_only), ZAK_AUTHO_IRESOURCE (zak_autho_get_resource_from_id (zak_autho, "paragraph")), FALSE) ? "is" : "isn't"));

	return 0;
}
