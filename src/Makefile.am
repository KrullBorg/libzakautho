LIBS = $(AUTOZ_LIBS)

AM_CPPFLAGS = $(AUTOZ_CFLAGS) \
              -DG_LOG_DOMAIN=\"ZakAutho\"

lib_LTLIBRARIES = libzakautho.la

libzakautho_la_SOURCES = autoz.c \
                      resource_interface.c \
                      role_interface.c \
                      resource.c \
                      role.c

libzakautho_la_LDFLAGS = -no-undefined

libzakautho_include_HEADERS = autoz.h \
                           resource_interface.h \
                           role_interface.h \
                           resource.h \
                           role.h

libzakautho_includedir = $(includedir)/libzakautho
