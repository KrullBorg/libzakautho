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

#ifndef __LIBAUTOZ_IROLE_H__
#define __LIBAUTOZ_IROLE_H__

#include <glib-object.h>


G_BEGIN_DECLS


#define AUTOZ_TYPE_IROLE             (autoz_irole_get_type ())
#define AUTOZ_IROLE(obj)             (G_TYPE_CHECK_INSTANCE_CAST ((obj), AUTOZ_TYPE_IROLE, AutozIRole))
#define AUTOZ_IS_IROLE(obj)          (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AUTOZ_TYPE_IROLE))
#define AUTOZ_IROLE_GET_IFACE(obj)   (G_TYPE_INSTANCE_GET_INTERFACE ((obj), AUTOZ_TYPE_IROLE, AutozIRoleIface))


typedef struct _AutozIRole AutozIRole;
typedef struct _AutozIRoleIface AutozIRoleIface;

struct _AutozIRoleIface
	{
		GTypeInterface g_iface;

		const gchar *(*get_role_id) (AutozIRole *irole);
	};

GType autoz_irole_get_type (void) G_GNUC_CONST;

const gchar *autoz_irole_get_role_id (AutozIRole *irole);


G_END_DECLS


#endif /* __LIBAUTOZ_IROLE_H__ */
