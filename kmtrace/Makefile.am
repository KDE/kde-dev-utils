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

AM_CPPFLAGS = -DQT_NO_ASCII_CAST -DQT_NO_COMPAT -O3
LDADD = $(LIB_KDECORE) /usr/lib/libiberty.a 
INCLUDES = $(all_includes)

bin_PROGRAMS = kmtrace
kmtrace_SOURCES = kmtrace.cpp
kmtrace_LDFLAGS = $(all_libraries)

lib_LIBRARIES = libktrace.a
libktrace_a_SOURCES = ktrace.c

data_DATA = kde.excludes
datadir = $(kde_datadir)/apps/kmtrace

include_HEADERS = ktrace.h
