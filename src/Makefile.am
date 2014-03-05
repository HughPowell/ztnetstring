lib_LTLIBRARIES = libztnetstr.la

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = libztnetstr.pc

include_HEADERS = \
    ../include/ztnetstr.h \
    ../include/ztns_impl.h

libztnetstr_la_SOURCES = \
    ztns_impl.c

INCLUDES = -I$(top_srcdir)/include
bin_PROGRAMS = ztnetstr ztnetstr_selftest
ztnetstr_selftest_LDADD = libztnetstr.la
ztnetstr_LDADD = libztnetstr.la

if ON_MINGW
libztnetstr_la_LDFLAGS = -no-undefined -avoid-version -version-info @LTVER@
else
libztnetstr_la_LDFLAGS = -version-info @LTVER@
endif

TESTS = ztnetstr_selftest

