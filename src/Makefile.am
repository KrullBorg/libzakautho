LIBS = $(AUTOZ_LIBS)

AM_CPPFLAGS = $(AUTOZ_CFLAGS)

lib_LTLIBRARIES = libautoz.la

libautoz_la_SOURCES = autoz.c \
                      resource.c \
                      role.c

libautoz_la_LDFLAGS = -no-undefined

libautoz_include_HEADERS = autoz.h \
                           resource.h \
                           role.h

libautoz_includedir = $(includedir)/libautoz
