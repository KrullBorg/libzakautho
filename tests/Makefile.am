LIBS = $(AUTOZ_LIBS) \
       -export-dynamic

AM_CPPFLAGS = $(AUTOZ_CFLAGS) \
              -I$(top_srcdir)/src \
              -DGUIDIR="\"@abs_builddir@\""

noinst_PROGRAMS = test

LDADD = $(top_builddir)/src/libautoz.la

EXTRA_DIST = 
