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

#ifndef __LIBAUTOZ_ROLE_H__
#define __LIBAUTOZ_ROLE_H__

#include <glib.h>
#include <glib-object.h>


G_BEGIN_DECLS


#define AUTOZ_TYPE_ROLE                 (autoz_role_get_type ())
#define AUTOZ_ROLE(obj)                 (G_TYPE_CHECK_INSTANCE_CAST ((obj), AUTOZ_TYPE_ROLE, AutozRole))
#define AUTOZ_ROLE_CLASS(klass)         (G_TYPE_CHECK_CLASS_CAST ((klass), AUTOZ_TYPE_ROLE, AutozRoleClass))
#define AUTOZ_IS_ROLE(obj)              (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AUTOZ_TYPE_ROLE))
#define AUTOZ_IS_ROLE_CLASS(klass)      (G_TYPE_CHECK_CLASS_TYPE ((klass), AUTOZ_TYPE_ROLE))
#define AUTOZ_ROLE_GET_CLASS(obj)       (G_TYPE_INSTANCE_GET_CLASS ((obj), AUTOZ_TYPE_ROLE, AutozRoleClass))


typedef struct _AutozRole AutozRole;
typedef struct _AutozRoleClass AutozRoleClass;

struct _AutozRole
	{
		GObject parent;
	};

struct _AutozRoleClass
	{
		GObjectClass parent_class;
	};

GType autoz_role_get_type (void) G_GNUC_CONST;


AutozRole *autoz_role_new (const gchar *role_id);


G_END_DECLS


#endif /* __LIBAUTOZ_ROLE_H__ */
