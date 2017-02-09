cmake_minimum_required(VERSION 2.8.12)

project(kuiviewer)

add_definitions(-DQT_NO_URL_CAST_FROM_STRING)

find_package(ECM 1.7 REQUIRED NO_MODULE)
set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH} ${ECM_KDE_MODULE_DIR})

set(QT_MIN_VERSION "5.5.0")

include(FeatureSummary)
include(ECMInstallIcons)
include(KDEInstallDirs)
include(KDECMakeSettings)
include(KDECompilerSettings NO_POLICY_SCOPE)
include(ECMAddAppIcon)

find_package(Qt5 ${QT_MIN_VERSION} REQUIRED NO_MODULE
    COMPONENTS
    Core
    Designer
    UiTools
)

find_package(KF5 REQUIRED
    COMPONENTS
    CoreAddons
    JobWidgets
    KIO
    I18n
    Parts
    WidgetsAddons
)

########### next target ###############

set(kuiviewer_SRCS main.cpp kuiviewer.cpp )

file(GLOB ICONS_SRCS "${CMAKE_CURRENT_SOURCE_DIR}/*-apps-kuiviewer.png")
ecm_add_app_icon(kuiviewer_SRCS ICONS ${ICONS_SRCS})

add_executable(kuiviewer ${kuiviewer_SRCS})

target_link_libraries(kuiviewer  KF5::Parts  )

install(TARGETS kuiviewer  ${KDE_INSTALL_TARGETS_DEFAULT_ARGS} )


########### next target ###############

set(kuiviewerpart_PART_SRCS kuiviewer_part.cpp )


add_library(kuiviewerpart MODULE ${kuiviewerpart_PART_SRCS})



target_link_libraries(kuiviewerpart  KF5::Parts Qt5::Designer Qt5::UiTools)

install(TARGETS kuiviewerpart  DESTINATION ${KDE_INSTALL_PLUGINDIR} )


########### next target ###############

set(quithumbnail_PART_SRCS quicreator.cpp )


add_library(quithumbnail MODULE ${quithumbnail_PART_SRCS})



target_link_libraries(quithumbnail KF5::KIOCore KF5::KIOFileWidgets KF5::KIOWidgets KF5::KIONTLM
                                   Qt5::Designer Qt5::UiTools Qt5::Xml Qt5::Gui )

install(TARGETS quithumbnail  DESTINATION ${KDE_INSTALL_PLUGINDIR} )


########### install files ###############

install( PROGRAMS org.kde.kuiviewer.desktop  DESTINATION  ${KDE_INSTALL_APPDIR} )
install( FILES designerthumbnail.desktop kuiviewer_part.desktop  DESTINATION  ${KDE_INSTALL_KSERVICES5DIR} )
install( FILES kuiviewer_part.rc  DESTINATION  ${KDE_INSTALL_KXMLGUI5DIR}/kuiviewerpart )
install( FILES kuiviewerui.rc  DESTINATION  ${KDE_INSTALL_KXMLGUI5DIR}/kuiviewer )

ecm_install_icons(ICONS
    16-apps-kuiviewer.png
    32-apps-kuiviewer.png
    48-apps-kuiviewer.png
    DESTINATION ${KDE_INSTALL_ICONDIR}
    THEME hicolor
)
