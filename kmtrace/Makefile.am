#    This file is part of the KDE libraries
#    Copyright (C) 1996-1997 Matthias Kalle Dalheimer (kalle@kde.org)
#              (C) 1997-1998 Stephan Kulow (coolo@kde.org)

#    This library is free software; you can redistribute it and/or
#    modify it under the terms of the GNU Library General Public
#    License as published by the Free Software Foundation; either
#    version 2 of the License, or (at your option) any later version.

#    This library is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
#    Library General Public License for more details.

#    You should have received a copy of the GNU Library General Public License
#    along with this library; see the file COPYING.LIB.  If not, write to
#    the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
#    Boston, MA 02111-1307, USA.

AM_CPPFLAGS = -DQT_NO_ASCII_CAST -UQT_NO_COMPAT -O3
LDADD = $(LIB_KDECORE) -liberty
INCLUDES = $(all_includes)

bin_PROGRAMS = kmtrace demangle kmmatch
kmtrace_SOURCES = kmtrace.cpp
kmtrace_LDFLAGS = $(all_libraries)

demangle_SOURCES = demangle.cpp
demangle_LDFLAGS = $(all_libraries)

kmmatch_SOURCES = match.cpp
kmmatch_LDFLAGS = $(all_libraries)

bin_SCRIPTS = kminspector

lib_LIBRARIES = libktrace_s.a
libktrace_s_a_SOURCES = ktrace_s.c

ktrace_s.c: $(srcdir)/ktrace.c
	$(LN_S) $(srcdir)/ktrace.c $@

lib_LTLIBRARIES = libktrace.la
libktrace_la_LDFLAGS = -avoid-version
libktrace_la_SOURCES = ksotrace.cpp ktrace.c
libktrace_la_LIBADD = $(LIBDL)

exclude_DATA = kde.excludes
excludedir = $(kde_datadir)/kmtrace

include_HEADERS = ktrace.h
