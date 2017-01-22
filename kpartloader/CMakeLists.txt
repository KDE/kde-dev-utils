cmake_minimum_required(VERSION 2.8.12)

project(kpartloader)
########### kpartloader ###############

find_package(ECM 1.7 REQUIRED NO_MODULE)
set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH} ${ECM_KDE_MODULE_DIR})

set(QT_MIN_VERSION "5.5.0")

include(FeatureSummary)
include(ECMInstallIcons)
include(KDEInstallDirs)
include(KDECMakeSettings)
include(KDECompilerSettings NO_POLICY_SCOPE)

find_package(Qt5 ${QT_MIN_VERSION} REQUIRED NO_MODULE
    COMPONENTS
    Core
)

find_package(KF5 REQUIRED
    COMPONENTS
    CoreAddons
    I18n
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

