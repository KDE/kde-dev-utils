project(kstartperf)



########### next target ###############

set(kstartperf_LIB_SRCS libkstartperf.c )

kde4_automoc(${kstartperf_LIB_SRCS})

kde4_add_library(kstartperf SHARED ${kstartperf_LIB_SRCS})

target_link_libraries(kstartperf  ${KDE4_KDECORE_LIBS} )

set_target_properties(kstartperf PROPERTIES VERSION 1.0.0 SOVERSION 1 )
install(TARGETS kstartperf  DESTINATION ${LIB_INSTALL_DIR} )


########### next target ###############

set(kstartperf_bin_SRCS kstartperf.cpp )

kde4_automoc(${kstartperf_bin_SRCS})

kde4_add_executable(kstartperf_bin ${kstartperf_bin_SRCS})

set_target_properties(kstartperf_bin PROPERTIES OUTPUT_NAME kstartperf)


target_link_libraries(kstartperf_bin  ${KDE4_KDECORE_LIBS} kstartperf)

install(TARGETS kstartperf_bin  DESTINATION ${BIN_INSTALL_DIR} )


########### install files ###############





#original Makefile.am contents follow:

#INCLUDES = $(all_includes)
#
#lib_LTLIBRARIES = libkstartperf.la
#libkstartperf_la_LDFLAGS = $(all_libraries) -version-info 1:0 -no-undefined
## libkstartperf_la_LIBADD = ../libltdl/libltdlc.la
#libkstartperf_la_SOURCES = libkstartperf.c
#
#bin_PROGRAMS = kstartperf
#kstartperf_LDFLAGS = $(all_libraries)
#kstartperf_LDADD = $(LIB_KDECORE)
#kstartperf_SOURCES = kstartperf.cpp
#
#messages:
#	$(XGETTEXT) $(kstartperf_SOURCES) -o $(podir)/kstartperf.pot
