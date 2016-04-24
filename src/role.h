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

#ifndef __LIB_ZAK_AUTHO_ROLE_H__
#define __LIB_ZAK_AUTHO_ROLE_H__

#include <glib.h>
#include <glib-object.h>


G_BEGIN_DECLS


#define ZAK_AUTHO_TYPE_ROLE                 (zak_autho_role_get_type ())
#define ZAK_AUTHO_ROLE(obj)                 (G_TYPE_CHECK_INSTANCE_CAST ((obj), ZAK_AUTHO_TYPE_ROLE, ZakAuthoRole))
#define ZAK_AUTHO_ROLE_CLASS(klass)         (G_TYPE_CHECK_CLASS_CAST ((klass), ZAK_AUTHO_TYPE_ROLE, ZakAuthoRoleClass))
#define ZAK_AUTHO_IS_ROLE(obj)              (G_TYPE_CHECK_INSTANCE_TYPE ((obj), ZAK_AUTHO_TYPE_ROLE))
#define ZAK_AUTHO_IS_ROLE_CLASS(klass)      (G_TYPE_CHECK_CLASS_TYPE ((klass), ZAK_AUTHO_TYPE_ROLE))
#define ZAK_AUTHO_ROLE_GET_CLASS(obj)       (G_TYPE_INSTANCE_GET_CLASS ((obj), ZAK_AUTHO_TYPE_ROLE, ZakAuthoRoleClass))


typedef struct _ZakAuthoRole ZakAuthoRole;
typedef struct _ZakAuthoRoleClass ZakAuthoRoleClass;

struct _ZakAuthoRole
	{
		GObject parent;
	};

struct _ZakAuthoRoleClass
	{
		GObjectClass parent_class;
	};

GType zak_autho_role_get_type (void) G_GNUC_CONST;


ZakAuthoRole *zak_autho_role_new (const gchar *role_id);


G_END_DECLS


#endif /* __LIB_ZAK_AUTHO_ROLE_H__ */
