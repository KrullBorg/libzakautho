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

#include "autoz.h"

static void autoz_class_init (AutozClass *class);
static void autoz_init (Autoz *form);

static void autoz_set_property (GObject *object,
                               guint property_id,
                               const GValue *value,
                               GParamSpec *pspec);
static void autoz_get_property (GObject *object,
                               guint property_id,
                               GValue *value,
                               GParamSpec *pspec);

#define AUTOZ_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), TYPE_AUTOZ, AutozPrivate))

typedef struct _AutozPrivate AutozPrivate;
struct _AutozPrivate
	{
		gpointer foo;
	};

G_DEFINE_TYPE (Autoz, autoz, G_TYPE_OBJECT)

static void
autoz_class_init (AutozClass *class)
{
	GObjectClass *object_class = G_OBJECT_CLASS (class);

	object_class->set_property = autoz_set_property;
	object_class->get_property = autoz_get_property;

	g_type_class_add_private (object_class, sizeof (AutozPrivate));
}

static void
autoz_init (Autoz *form)
{
	AutozPrivate *priv = AUTOZ_GET_PRIVATE (form);
}

/**
 * autoz_new:
 *
 * Returns: the newly created #Autoz object.
 */
Autoz
*autoz_new ()
{

	return AUTOZ (g_object_new (autoz_get_type (), NULL));
}

/* PRIVATE */
static void
autoz_set_property (GObject *object,
                   guint property_id,
                   const GValue *value,
                   GParamSpec *pspec)
{
	Autoz *form = (Autoz *)object;

	AutozPrivate *priv = AUTOZ_GET_PRIVATE (form);

	switch (property_id)
		{
			default:
				G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
				break;
	  }
}

static void
autoz_get_property (GObject *object,
                   guint property_id,
                   GValue *value,
                   GParamSpec *pspec)
{
	Autoz *form = (Autoz *)object;

	AutozPrivate *priv = AUTOZ_GET_PRIVATE (form);

	switch (property_id)
		{
			default:
				G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
				break;
	  }
}
