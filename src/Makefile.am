lib_LTLIBRARIES = libsynocracy.la

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = libsynocracy.pc

include_HEADERS = \
    ../include/synocracy.h \
    ../include/myp_mystatefulmod.h \
    ../include/myp_mystatelessmod.h \
    ../include/mongoose.h

libsynocracy_la_SOURCES = \
    myp_mystatefulmod.c \
    myp_mystatelessmod.c \
    mongoose.c

INCLUDES = -I$(top_srcdir)/include
bin_PROGRAMS = synocracy synocracy_selftest
synocracy_selftest_LDADD = libsynocracy.la
synocracy_LDADD = libsynocracy.la

if ON_MINGW
libsynocracy_la_LDFLAGS = -no-undefined -avoid-version -version-info @LTVER@
else
libsynocracy_la_LDFLAGS = -version-info @LTVER@
endif

TESTS = synocracy_selftest

