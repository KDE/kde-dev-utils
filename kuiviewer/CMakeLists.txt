kde4_header()

include_directories( ${KDE4_INCLUDE_DIR} ${QT_INCLUDES} ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR}  )


########### next target ###############

set(kuiviewer_SRCS main.cpp kuiviewer.cpp )

kde4_automoc(${kuiviewer_SRCS})

kde4_add_executable(kuiviewer ${kuiviewer_SRCS})

target_link_libraries(kuiviewer  ${KDE4_KDECORE_LIBS} kparts )

install_targets(/bin kuiviewer )


########### next target ###############

set(kuiviewerpart_PART_SRCS kuiviewer_part.cpp )

kde4_automoc(${kuiviewerpart_PART_SRCS})

kde4_add_plugin(kuiviewerpart WITH_PREFIX ${kuiviewerpart_PART_SRCS})

kde4_install_libtool_file( ${PLUGIN_INSTALL_DIR} kuiviewerpart )

target_link_libraries(kuiviewerpart  ${KDE4_KDECORE_LIBS} kparts QtUiTools )

install_targets(${PLUGIN_INSTALL_DIR} kuiviewerpart )


########### next target ###############

set(quithumbnail_PART_SRCS quicreator.cpp )

kde4_automoc(${quithumbnail_PART_SRCS})

kde4_add_plugin(quithumbnail ${quithumbnail_PART_SRCS})

kde4_install_libtool_file( ${PLUGIN_INSTALL_DIR} quithumbnail )

target_link_libraries(quithumbnail  ${KDE4_KDECORE_LIBS} QtUiTools QtXml )

install_targets(${PLUGIN_INSTALL_DIR} quithumbnail )


########### install files ###############

install_files( ${XDG_APPS_DIR} FILES kuiviewer.desktop )
install_files( ${SERVICES_INSTALL_DIR} FILES kuiviewer_part.desktop )
install_files( ${DATA_INSTALL_DIR}/kuiviewerpart FILES kuiviewer_part.rc )
install_files( ${DATA_INSTALL_DIR}/kuiviewer FILES kuiviewerui.rc )
install_files( ${SERVICES_INSTALL_DIR} FILES designerthumbnail.desktop )

kde4_install_icons( ${ICON_INSTALL_DIR}  hicolor )

kde4_footer()



#original Makefile.am contents follow:

## this has all of the subdirectories that make will recurse into.  if
## there are none, comment this out
#
## set the include path for X, qt and KDE
#INCLUDES = $(all_includes)
#
## these are the headers for your project
#noinst_HEADERS = kuiviewer.h kuiviewer_part.h
#
## let automoc handle all of the meta source files (moc)
#METASOURCES = AUTO
#
#messages: rc.cpp
#	$(XGETTEXT) *.cpp -o $(podir)/kuiviewer.pot
#
#KDE_ICON = kuiviewer
#
## this Makefile creates both a KPart application and a KPart
##########################################################################
## APPLICATION SECTION
##########################################################################
## this is the program that gets installed.  it's name is used for all
## of the other Makefile.am variables
#bin_PROGRAMS = kuiviewer
#
## the application source, library search path, and link libraries
#kuiviewer_SOURCES = main.cpp kuiviewer.cpp
#kuiviewer_LDFLAGS = $(KDE_RPATH) $(all_libraries)
#kuiviewer_LDADD   = $(LIB_KPARTS)
#
#xdg_apps_DATA =kuiviewer.desktop
#
## this is where the shell's XML-GUI resource file goes
#shellrcdir   = $(kde_datadir)/kuiviewer
#shellrc_DATA = kuiviewerui.rc
#
##########################################################################
## KPART SECTION
##########################################################################
#kde_module_LTLIBRARIES = libkuiviewerpart.la quithumbnail.la
#
## the Part's source, library search path, and link libraries
#libkuiviewerpart_la_SOURCES = kuiviewer_part.cpp
#libkuiviewerpart_la_LDFLAGS = -module $(KDE_PLUGIN) $(all_libraries)
#libkuiviewerpart_la_LIBADD  = $(LIB_KPARTS) $(LIB_KFILE) -lqui
#
## this is where the desktop file will go 
#partdesktopdir   = $(kde_servicesdir)
#partdesktop_DATA = kuiviewer_part.desktop
#
## this is where the part's XML-GUI resource file goes
#partrcdir   = $(kde_datadir)/kuiviewerpart
#partrc_DATA = kuiviewer_part.rc
#
#
##########################################################################
## THUMBNAIL SECTION
##########################################################################
#quithumbnail_la_SOURCES = quicreator.cpp
#quithumbnail_la_LIBADD = $(LIB_KDECORE) -lqui
#quithumbnail_la_LDFLAGS = $(all_libraries) -module $(KDE_PLUGIN)
#services_DATA = designerthumbnail.desktop
#servicesdir = $(kde_servicesdir)
#
##########################################################################
## UTILS SECTION
##########################################################################
#.PHONY:	changes
#changes:
#	cvs2cl.pl --window 3600 -w --hide-filenames -I '.desktop'
