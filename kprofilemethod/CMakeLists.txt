project(kprofilemethod)
cmake_minimum_required(VERSION 2.8.12)

find_package(ECM 1.7 REQUIRED NO_MODULE)
set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH} ${ECM_KDE_MODULE_DIR})
include(KDEInstallDirs)
include(KDECMakeSettings)

install( FILES kprofilemethod.h DESTINATION ${KDE_INSTALL_INCLUDEDIR} COMPONENT Devel)
