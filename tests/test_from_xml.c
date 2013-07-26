/*
 * Copyright (C) 2010-2013 Andrea Zagli <azagli@libero.it>
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

#include <libxml/tree.h>

#include "autoz.h"

int
main (int argc, char **argv)
{
	Autoz *autoz;

	xmlDocPtr xdoc;
	xmlNodePtr xnode;

	autoz = autoz_new ();

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

	autoz_load_from_xml (autoz, xmlDocGetRootElement (xdoc), TRUE);

	/* get xml */
	xnode = autoz_get_xml (autoz);
	if (xnode != NULL)
		{
			xdoc = xmlNewDoc ("1.0");
			xmlDocSetRootElement (xdoc, xnode);
			g_fprintf (stdout, "\n");
			xmlSaveFormatFile ("-", xdoc, 2);
			g_fprintf (stdout, "\n");
		}

	g_message ("super-admin %s allowed to page.",
	           (autoz_is_allowed (autoz, autoz_get_role_from_id (autoz, "super-admin"), autoz_get_resource_from_id (autoz, "page"), FALSE) ? "is" : "isn't"));
	g_message ("super-admin %s allowed to paragraph.",
	           (autoz_is_allowed (autoz, autoz_get_role_from_id (autoz, "super-admin"), autoz_get_resource_from_id (autoz, "paragraph"), FALSE) ? "is" : "isn't"));
	g_message ("super-admin %s allowed to paragraph (exclude_null).",
	           (autoz_is_allowed (autoz, autoz_get_role_from_id (autoz, "super-admin"), autoz_get_resource_from_id (autoz, "paragraph"), TRUE) ? "is" : "isn't"));
	g_message ("writer %s allowed to page.",
	           (autoz_is_allowed (autoz, autoz_get_role_from_id (autoz, "writer"), autoz_get_resource_from_id (autoz, "page"), FALSE) ? "is" : "isn't"));
	g_message ("writer-child %s allowed to page.",
	           (autoz_is_allowed (autoz, autoz_get_role_from_id (autoz, "writer-child"), autoz_get_resource_from_id (autoz, "page"), FALSE) ? "is" : "isn't"));
	g_message ("writer %s allowed to paragraph.",
	           (autoz_is_allowed (autoz, autoz_get_role_from_id (autoz, "writer"), autoz_get_resource_from_id (autoz, "paragraph"), FALSE) ? "is" : "isn't"));
	g_message ("writer-child %s allowed to paragraph.",
	           (autoz_is_allowed (autoz, autoz_get_role_from_id (autoz, "writer-child"), autoz_get_resource_from_id (autoz, "paragraph"), FALSE) ? "is" : "isn't"));
	g_message ("read-only %s allowed to page.",
	           (autoz_is_allowed (autoz, autoz_get_role_from_id (autoz, "read-only"), autoz_get_resource_from_id (autoz, "page"), FALSE) ? "is" : "isn't"));
	g_message ("read-only %s allowed to paragraph.",
	           (autoz_is_allowed (autoz, autoz_get_role_from_id (autoz, "read-only"), autoz_get_resource_from_id (autoz, "paragraph"), FALSE) ? "is" : "isn't"));

	g_message ("writer-child %s child of writer",
	           (autoz_role_is_child (autoz, autoz_get_role_from_id (autoz, "writer-child"), autoz_get_role_from_id (autoz, "writer")) ? "is" : "isn't"));
	g_message ("read-only %s child of super-admin",
	           (autoz_role_is_child (autoz, autoz_get_role_from_id (autoz, "read-only"), autoz_get_role_from_id (autoz, "super-admin")) ? "is" : "isn't"));

	g_message ("page %s child of paragraph",
	           (autoz_resource_is_child (autoz, autoz_get_resource_from_id (autoz, "page"), autoz_get_resource_from_id (autoz, "paragraph")) ? "is" : "isn't"));
	g_message ("paragraph %s child of page",
	           (autoz_resource_is_child (autoz, autoz_get_resource_from_id (autoz, "paragraph"), autoz_get_resource_from_id (autoz, "page")) ? "is" : "isn't"));

	return 0;
}
