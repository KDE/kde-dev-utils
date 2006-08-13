project(kmtrace)
# Check if libiberty is available
find_library(LIBIBERTY_LIBRARY NAMES iberty)
if (LIBIBERTY_LIBRARY)

  include_directories( ${KDE4_INCLUDE_DIR} ${QT_INCLUDES} ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR}  )
  configure_file(${CMAKE_CURRENT_SOURCE_DIR}/kminspector.in.cmake ${CMAKE_CURRENT_BINARY_DIR}/kminspector )
  INSTALL(PROGRAMS ${CMAKE_CURRENT_BINARY_DIR}/kminspector DESTINATION ${BIN_INSTALL_DIR})
  ########### kmtrace ###############

  set(kmtrace_SRCS kmtrace.cpp )

  kde4_automoc(${kmtrace_SRCS})

  kde4_add_executable(kmtrace ${kmtrace_SRCS})

  target_link_libraries(kmtrace  ${KDE4_KDECORE_LIBS} ${LIBIBERTY_LIBRARY})

install(TARGETS kmtrace  DESTINATION ${BIN_INSTALL_DIR} )

  ########### demangle ###############

  set(demangle_SRCS demangle.cpp )

  kde4_automoc(${demangle_SRCS})

  kde4_add_executable(demangle ${demangle_SRCS})

  target_link_libraries(demangle  ${KDE4_KDECORE_LIBS} ${LIBIBERTY_LIBRARY})

install(TARGETS demangle  DESTINATION ${BIN_INSTALL_DIR} )


  ########### kmmatch ###############

  set(kmmatch_SRCS match.cpp )

  kde4_automoc(${kmmatch_SRCS})

  kde4_add_executable(kmmatch ${kmmatch_SRCS})

  target_link_libraries(kmmatch  ${KDE4_KDECORE_LIBS} )

install(TARGETS kmmatch  DESTINATION ${BIN_INSTALL_DIR} )


  ########### ktrace ###############

  set(ktrace_LIB_SRCS ksotrace.cpp ktrace.c )

  kde4_automoc(${ktrace_LIB_SRCS})

  kde4_add_library(ktrace SHARED ${ktrace_LIB_SRCS})

  target_link_libraries(ktrace  ${KDE4_KDECORE_LIBS} )

  set_target_properties(ktrace PROPERTIES VERSION 1.0.0 SOVERSION 1 )
install(TARGETS ktrace  DESTINATION ${LIB_INSTALL_DIR} )

  ########### install files ###############

install( FILES kde.excludes  DESTINATION  ${DATA_INSTALL_DIR}/kmtrace )
  install( FILES ktrace.h DESTINATION ${INCLUDE_INSTALL_DIR} )

endif (LIBIBERTY_LIBRARY)




#original Makefile.am contents follow:

##    This file is part of the KDE libraries
##    Copyright (C) 1996-1997 Matthias Kalle Dalheimer (kalle@kde.org)
##              (C) 1997-1998 Stephan Kulow (coolo@kde.org)
#
##    This library is free software; you can redistribute it and/or
##    modify it under the terms of the GNU Library General Public
##    License as published by the Free Software Foundation; either
##    version 2 of the License, or (at your option) any later version.
#
##    This library is distributed in the hope that it will be useful,
##    but WITHOUT ANY WARRANTY; without even the implied warranty of
##    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
##    Library General Public License for more details.
#
##    You should have received a copy of the GNU Library General Public License
##    along with this library; see the file COPYING.LIB.  If not, write to
##    the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
##    Boston, MA 02110-1301, USA.
#
#AM_CPPFLAGS = -DQT_NO_ASCII_CAST -UQT_NO_COMPAT -O3
#LDADD = $(LIB_KDECORE) -liberty
#INCLUDES = $(all_includes)
#
#bin_PROGRAMS = kmtrace demangle kmmatch
#kmtrace_SOURCES = kmtrace.cpp
#kmtrace_LDFLAGS = $(all_libraries)
#
#demangle_SOURCES = demangle.cpp
#demangle_LDFLAGS = $(all_libraries)
#
#kmmatch_SOURCES = match.cpp
#kmmatch_LDFLAGS = $(all_libraries)
#
#bin_SCRIPTS = kminspector
#
#lib_LIBRARIES = libktrace_s.a
#libktrace_s_a_SOURCES = ktrace_s.c
#
#ktrace_s.c: $(srcdir)/ktrace.c
#	$(LN_S) $(srcdir)/ktrace.c $@
#
#lib_LTLIBRARIES = libktrace.la
#libktrace_la_LDFLAGS = -avoid-version
#libktrace_la_SOURCES = ksotrace.cpp ktrace.c
#libktrace_la_LIBADD = $(LIBDL)
#
#exclude_DATA = kde.excludes
#excludedir = $(kde_datadir)/kmtrace
#
#include_HEADERS = ktrace.h
