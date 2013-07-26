#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.65)
AC_INIT([libautoz], [0.0.5], [azagli@libero.it])
AC_CONFIG_SRCDIR([src/autoz.c])
AC_CONFIG_HEADER([config.h])

AM_INIT_AUTOMAKE(-Wall)

m4_ifdef([AM_SILENT_RULES],[AM_SILENT_RULES([yes])])

AM_MAINTAINER_MODE

AC_CONFIG_MACRO_DIR([m4])

AC_CANONICAL_SYSTEM

LT_INIT

AC_LIBTOOL_WIN32_DLL

# Checks for programs.
AC_PROG_CC
AC_PROG_CXX
AC_PROG_INSTALL
AC_PROG_LN_S
AC_PROG_MAKE_SET
AC_PROG_LIBTOOL
AC_PROG_RANLIB

GTK_DOC_CHECK

# Checks for libraries.
PKG_CHECK_MODULES(AUTOZ, [libxml-2.0 >= 2.7
                          libgda-5.0 >= 5.0.0])

AC_SUBST(AUTOZ_CFLAGS)
AC_SUBST(AUTOZ_LIBS)

# Checks for header files.
AC_HEADER_STDC

# Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST

# Checks for library functions.

# Checks for library functions.
AC_CONFIG_FILES([
  libautoz.pc
  Makefile
  src/Makefile
  tests/Makefile
  docs/Makefile
  docs/reference/Makefile
  docs/reference/version.xml
])
AC_OUTPUT
