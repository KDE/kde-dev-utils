# this has all of the subdirectories that make will recurse into.  if
# there are none, comment this out

# set the include path for X, qt and KDE
INCLUDES = $(all_includes)

# these are the headers for your project
noinst_HEADERS = kuiviewer.h kuiviewer_part.h

# let automoc handle all of the meta source files (moc)
METASOURCES = AUTO

messages: rc.cpp
	$(XGETTEXT) *.cpp -o $(podir)/kuiviewer.pot

KDE_ICON = kuiviewer

# this Makefile creates both a KPart application and a KPart
#########################################################################
# APPLICATION SECTION
#########################################################################
# this is the program that gets installed.  it's name is used for all
# of the other Makefile.am variables
bin_PROGRAMS = kuiviewer

# the application source, library search path, and link libraries
kuiviewer_SOURCES = main.cpp kuiviewer.cpp
kuiviewer_LDFLAGS = $(KDE_RPATH) $(all_libraries)
kuiviewer_LDADD   = $(LIB_KPARTS)

xdg_apps_DATA =kuiviewer.desktop

# this is where the shell's XML-GUI resource file goes
shellrcdir   = $(kde_datadir)/kuiviewer
shellrc_DATA = kuiviewerui.rc

#########################################################################
# KPART SECTION
#########################################################################
kde_module_LTLIBRARIES = libkuiviewerpart.la quithumbnail.la

# the Part's source, library search path, and link libraries
libkuiviewerpart_la_SOURCES = kuiviewer_part.cpp
libkuiviewerpart_la_LDFLAGS = -module $(KDE_PLUGIN) $(all_libraries)
libkuiviewerpart_la_LIBADD  = $(LIB_KPARTS) $(LIB_KFILE) -lqui

# this is where the desktop file will go 
partdesktopdir   = $(kde_servicesdir)
partdesktop_DATA = kuiviewer_part.desktop

# this is where the part's XML-GUI resource file goes
partrcdir   = $(kde_datadir)/kuiviewerpart
partrc_DATA = kuiviewer_part.rc


#########################################################################
# THUMBNAIL SECTION
#########################################################################
quithumbnail_la_SOURCES = quicreator.cpp
quithumbnail_la_LIBADD = $(LIB_KDECORE) -lqui
quithumbnail_la_LDFLAGS = $(all_libraries) -module $(KDE_PLUGIN)
services_DATA = designerthumbnail.desktop
servicesdir = $(kde_servicesdir)

#########################################################################
# UTILS SECTION
#########################################################################
.PHONY:	changes
changes:
	cvs2cl.pl --window 3600 -w --hide-filenames -I '.desktop'
