# Bump KUIVIEWER_BASE_VERSION once new features are added
set(KUIVIEWER_BASE_VERSION "0.3")
set(KUIVIEWER_VERSION "${KUIVIEWER_BASE_VERSION}.${RELEASE_SERVICE_COMPACT_VERSION}")

include(ECMAddAppIcon)

find_package(Qt5 ${QT_MIN_VERSION} REQUIRED NO_MODULE
    COMPONENTS
    Core
    Designer
    UiTools
)

find_package(KF5 ${KF_MIN_VERSION} REQUIRED
    COMPONENTS
    CoreAddons
    KIO
    Parts
)

add_definitions(
    -DQT_DEPRECATED_WARNINGS
    -DQT_DISABLE_DEPRECATED_BEFORE=0x050900
    -DQT_NO_SIGNALS_SLOTS_KEYWORDS
    -DQT_STRICT_ITERATORS
    -DQT_USE_QSTRINGBUILDER
    -DQT_NO_CAST_TO_ASCII
    -DQT_NO_CAST_FROM_ASCII
    -DQT_NO_CAST_FROM_BYTEARRAY
    -DQT_NO_URL_CAST_FROM_STRING
)

ecm_setup_version(${KUIVIEWER_VERSION}
    VARIABLE_PREFIX KUIVIEWER
    VERSION_HEADER kuiviewer_version.h
)

set(kuiviewer_SRCS main.cpp kuiviewer.cpp )

qt5_add_resources(kuiviewer_SRCS
    kuiviewer.qrc
)

file(GLOB ICONS_SRCS "${CMAKE_CURRENT_SOURCE_DIR}/*-apps-kuiviewer.png")
ecm_add_app_icon(kuiviewer_SRCS ICONS ${ICONS_SRCS})

add_executable(kuiviewer ${kuiviewer_SRCS})

target_link_libraries(kuiviewer  KF5::Parts  )

install(TARGETS kuiviewer  ${KDE_INSTALL_TARGETS_DEFAULT_ARGS} )


configure_file(kuiviewer_part.desktop ${CMAKE_CURRENT_BINARY_DIR}/kuiviewer_part.desktop @ONLY)

set(kuiviewerpart_PART_SRCS kuiviewer_part.cpp )

qt5_add_resources(kuiviewerpart_PART_SRCS
    kuiviewer_part.qrc
)

ecm_qt_declare_logging_category(kuiviewerpart_PART_SRCS
    HEADER kuiviewer_part_debug.h
    IDENTIFIER KUIVIEWERPART
    CATEGORY_NAME "kuiviewerpart"
)

add_library(kuiviewerpart MODULE ${kuiviewerpart_PART_SRCS})
kcoreaddons_desktop_to_json(kuiviewerpart ${CMAKE_CURRENT_BINARY_DIR}/kuiviewer_part.desktop)

target_link_libraries(kuiviewerpart  KF5::Parts Qt5::Designer Qt5::UiTools)

install(TARGETS kuiviewerpart  DESTINATION ${KDE_INSTALL_PLUGINDIR}/kf5/parts )
install(FILES org.kde.kuiviewerpart.metainfo.xml DESTINATION ${KDE_INSTALL_METAINFODIR})


set(quithumbnail_PART_SRCS quicreator.cpp )

add_library(quithumbnail MODULE ${quithumbnail_PART_SRCS})

target_link_libraries(quithumbnail
    KF5::KIOWidgets
    Qt5::Designer
    Qt5::UiTools
)

install(TARGETS quithumbnail  DESTINATION ${KDE_INSTALL_PLUGINDIR} )

install( PROGRAMS org.kde.kuiviewer.desktop  DESTINATION  ${KDE_INSTALL_APPDIR} )
install(FILES
    designerthumbnail.desktop
    ${CMAKE_CURRENT_BINARY_DIR}/kuiviewer_part.desktop
    DESTINATION  ${KDE_INSTALL_KSERVICES5DIR}
)
install(FILES org.kde.kuiviewer.metainfo.xml DESTINATION ${KDE_INSTALL_METAINFODIR})

ecm_install_icons(ICONS
    16-apps-kuiviewer.png
    32-apps-kuiviewer.png
    48-apps-kuiviewer.png
    64-apps-kuiviewer.png
    128-apps-kuiviewer.png
    sc-apps-kuiviewer.svg
    DESTINATION ${KDE_INSTALL_ICONDIR}
    THEME hicolor
)
