cmake_minimum_required(VERSION 2.8.12)

find_package(ECM 5.14 REQUIRED NO_MODULE)
set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH} ${ECM_KDE_MODULE_DIR})

include(FeatureSummary)
include(ECMInstallIcons)
include(KDEInstallDirs)
include(KDECMakeSettings)
include(KDECompilerSettings NO_POLICY_SCOPE)
include(ECMQtDeclareLoggingCategory)

# Required for ki18n_install(po) added on tarball generation
find_package(KF5 REQUIRED COMPONENTS I18n)

add_subdirectory(kpartloader)
add_subdirectory(kuiviewer)
