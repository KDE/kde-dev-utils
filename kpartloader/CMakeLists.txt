# Bump KPARTLOADER_BASE_VERSION once new features are added
set(KPARTLOADER_BASE_VERSION "1.1")
set(KPARTLOADER_VERSION "${KPARTLOADER_BASE_VERSION}.${RELEASE_SERVICE_COMPACT_VERSION}")

find_package(Qt6 ${QT_MIN_VERSION} REQUIRED COMPONENTS
    Core
)

find_package(KF6 ${KF_MIN_VERSION} REQUIRED COMPONENTS
    I18n
    CoreAddons
    Parts
    WidgetsAddons
)

# added by KDE_COMPILERSETTINGS_LEVEL 5.85
remove_definitions(
    -DQT_NO_CAST_FROM_ASCII
)

ecm_setup_version(${KPARTLOADER_VERSION}
    VARIABLE_PREFIX KPARTLOADER
    VERSION_HEADER kpartloader_version.h
)

add_executable(kpartloader)

target_sources(kpartloader PRIVATE
    kpartloader.cpp

    kpartloader.qrc
)

target_link_libraries(kpartloader
    KF6::Parts
    KF6::I18n
)
install(TARGETS kpartloader ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})
