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

int
main (int argc, char **argv)
{
	ZakAutho *zak_autho;

	xmlDocPtr xdoc;
	xmlNodePtr xnode;

	zak_autho = zak_autho_new ();

	if (argc <= 1)
		{
			g_error ("You must specified an xml file to load.");
			return 0;
		}

	xdoc = xmlParseFile (argv[1]);
	if (xdoc == NULL)
		{
			g_error ("Unable to parse xml file.");
			return 0;
		}

	zak_autho_load_from_xml (zak_autho, xmlDocGetRootElement (xdoc), TRUE);

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
	           (zak_autho_is_allowed (zak_autho, zak_autho_get_role_from_id (zak_autho, "super-admin"), zak_autho_get_resource_from_id (zak_autho, "page"), FALSE) ? "is" : "isn't"));
	g_message ("super-admin %s allowed to paragraph.",
	           (zak_autho_is_allowed (zak_autho, zak_autho_get_role_from_id (zak_autho, "super-admin"), zak_autho_get_resource_from_id (zak_autho, "paragraph"), FALSE) ? "is" : "isn't"));
	g_message ("super-admin %s allowed to paragraph (exclude_null).",
	           (zak_autho_is_allowed (zak_autho, zak_autho_get_role_from_id (zak_autho, "super-admin"), zak_autho_get_resource_from_id (zak_autho, "paragraph"), TRUE) ? "is" : "isn't"));
	g_message ("writer %s allowed to page.",
	           (zak_autho_is_allowed (zak_autho, zak_autho_get_role_from_id (zak_autho, "writer"), zak_autho_get_resource_from_id (zak_autho, "page"), FALSE) ? "is" : "isn't"));
	g_message ("writer-child %s allowed to page.",
	           (zak_autho_is_allowed (zak_autho, zak_autho_get_role_from_id (zak_autho, "writer-child"), zak_autho_get_resource_from_id (zak_autho, "page"), FALSE) ? "is" : "isn't"));
	g_message ("writer %s allowed to paragraph.",
	           (zak_autho_is_allowed (zak_autho, zak_autho_get_role_from_id (zak_autho, "writer"), zak_autho_get_resource_from_id (zak_autho, "paragraph"), FALSE) ? "is" : "isn't"));
	g_message ("writer-child %s allowed to paragraph.",
	           (zak_autho_is_allowed (zak_autho, zak_autho_get_role_from_id (zak_autho, "writer-child"), zak_autho_get_resource_from_id (zak_autho, "paragraph"), FALSE) ? "is" : "isn't"));
	g_message ("read-only %s allowed to page.",
	           (zak_autho_is_allowed (zak_autho, zak_autho_get_role_from_id (zak_autho, "read-only"), zak_autho_get_resource_from_id (zak_autho, "page"), FALSE) ? "is" : "isn't"));
	g_message ("read-only %s allowed to paragraph.",
	           (zak_autho_is_allowed (zak_autho, zak_autho_get_role_from_id (zak_autho, "read-only"), zak_autho_get_resource_from_id (zak_autho, "paragraph"), FALSE) ? "is" : "isn't"));

	g_message ("writer-child %s child of writer",
	           (zak_autho_role_is_child (zak_autho, zak_autho_get_role_from_id (zak_autho, "writer-child"), zak_autho_get_role_from_id (zak_autho, "writer")) ? "is" : "isn't"));
	g_message ("read-only %s child of super-admin",
	           (zak_autho_role_is_child (zak_autho, zak_autho_get_role_from_id (zak_autho, "read-only"), zak_autho_get_role_from_id (zak_autho, "super-admin")) ? "is" : "isn't"));

	g_message ("page %s child of paragraph",
	           (zak_autho_resource_is_child (zak_autho, zak_autho_get_resource_from_id (zak_autho, "page"), zak_autho_get_resource_from_id (zak_autho, "paragraph")) ? "is" : "isn't"));
	g_message ("paragraph %s child of page",
	           (zak_autho_resource_is_child (zak_autho, zak_autho_get_resource_from_id (zak_autho, "paragraph"), zak_autho_get_resource_from_id (zak_autho, "page")) ? "is" : "isn't"));

	return 0;
}
