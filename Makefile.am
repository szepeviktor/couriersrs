SUBDIRS = intl m4 po 

bin_PROGRAMS = couriersrs

noinst_HEADERS = couriersrs.h

couriersrs_SOURCES = couriersrs.cc

couriersrs_LDFLAGS = @LDFLAGS@

ACLOCAL_AMFLAGS = -I m4

EXTRA_DIST = config.rpath mkinstalldirs

DEFS = -DLOCALEDIR=\"$(localedir)\" @DEFS@
