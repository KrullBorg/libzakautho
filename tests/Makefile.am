LIBS = $(AUTOZ_LIBS) \
       -export-dynamic

AM_CPPFLAGS = $(AUTOZ_CFLAGS) \
              -I$(top_srcdir)/src \
              -DGUIDIR="\"@abs_builddir@\""

noinst_PROGRAMS = test \
                  test_from_xml \
                  test_from_xml_to_db

LDADD = $(top_builddir)/src/libautoz.la

EXTRA_DIST = test_from_xml.xml \
             test_to_db.db
