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

add_executable(kpartloader ${kpartloader_SRCS})
target_link_libraries(kpartloader KF5::Parts)
install(TARGETS kpartloader ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})
install(FILES kpartloaderui.rc DESTINATION ${KDE_INSTALL_KXMLGUI5DIR}/kpartloader)

