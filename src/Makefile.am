LIBS = $(AUTOZ_LIBS)

AM_CPPFLAGS = $(AUTOZ_CFLAGS) \
              -DG_LOG_DOMAIN=\"Autoz\"

lib_LTLIBRARIES = libautoz.la

libautoz_la_SOURCES = autoz.c \
                      resource_interface.c \
                      role_interface.c \
                      resource.c \
                      role.c

libautoz_la_LDFLAGS = -no-undefined

libautoz_include_HEADERS = autoz.h \
                           resource_interface.h \
                           role_interface.h \
                           resource.h \
                           role.h

libautoz_includedir = $(includedir)/libautoz
