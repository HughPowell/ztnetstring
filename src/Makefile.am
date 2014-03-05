lib_LTLIBRARIES = libsynocracy.la

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = libsynocracy.pc

include_HEADERS = \
    ../include/ztnetstr.h \
    ../include/ztns_impl.h

libsynocracy_la_SOURCES = \
    ztns_impl.c

INCLUDES = -I$(top_srcdir)/include
bin_PROGRAMS = ztnetstr ztnetstr_selftest
ztnetstr_selftest_LDADD = libsynocracy.la
ztnetstr_LDADD = libsynocracy.la

if ON_MINGW
libsynocracy_la_LDFLAGS = -no-undefined -avoid-version -version-info @LTVER@
else
libsynocracy_la_LDFLAGS = -version-info @LTVER@
endif

TESTS = ztnetstr_selftest

