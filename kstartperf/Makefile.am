INCLUDES = $(all_includes)

lib_LTLIBRARIES = libkstartperf.la
libkstartperf_la_LDFLAGS = $(all_libraries) -version-info 1:0 -no-undefined
# libkstartperf_la_LIBADD = ../libltdl/libltdlc.la
libkstartperf_la_SOURCES = libkstartperf.c

bin_PROGRAMS = kstartperf
kstartperf_LDFLAGS = $(all_libraries)
kstartperf_LDADD = $(LIB_KDECORE)
kstartperf_SOURCES = kstartperf.cpp

messages:
	$(XGETTEXT) $(kstartperf_SOURCES) -o $(podir)/kstartperf.pot
