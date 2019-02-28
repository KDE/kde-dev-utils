project(kpartloader)
########### kpartloader ###############

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

set(kpartloader_SRCS
    kpartloader.cpp
)
qt5_add_resources(kpartloader_SRCS
    kpartloader.qrc
)

add_executable(kpartloader ${kpartloader_SRCS})
target_link_libraries(kpartloader KF5::Parts)
install(TARGETS kpartloader ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})

