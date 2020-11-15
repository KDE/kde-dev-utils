# Bump KPARTLOADER_BASE_VERSION once new features are added
set(KPARTLOADER_BASE_VERSION "1.1")
set(KPARTLOADER_VERSION "${KPARTLOADER_BASE_VERSION}.${RELEASE_SERVICE_COMPACT_VERSION}")

find_package(Qt5 ${QT_MIN_VERSION} REQUIRED NO_MODULE
    COMPONENTS
    Core
)

find_package(KF5 ${KF_MIN_VERSION} REQUIRED
    COMPONENTS
    CoreAddons
    Parts
    WidgetsAddons
)

ecm_setup_version(${KPARTLOADER_VERSION}
    VARIABLE_PREFIX KPARTLOADER
    VERSION_HEADER kpartloader_version.h
)

set(kpartloader_SRCS
    kpartloader.cpp
)
qt5_add_resources(kpartloader_SRCS
    kpartloader.qrc
)

add_executable(kpartloader ${kpartloader_SRCS})
target_link_libraries(kpartloader KF5::Parts)
install(TARGETS kpartloader ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})

