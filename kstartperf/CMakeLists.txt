project(kstartperf)
cmake_minimum_required(VERSION 2.8.12)

find_package(ECM 1.7 REQUIRED NO_MODULE)
set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH} ${ECM_KDE_MODULE_DIR})

set(QT_MIN_VERSION "5.5.0")

include(FeatureSummary)
include(CheckIncludeFile)
include(KDEInstallDirs)
include(KDECMakeSettings)
include(KDECompilerSettings NO_POLICY_SCOPE)
include(ECMMarkNonGuiExecutable)

find_package(Qt5 ${QT_MIN_VERSION} REQUIRED NO_MODULE
    COMPONENTS
    Core
)

find_package(KF5 REQUIRED
    COMPONENTS
    CoreAddons
    KDELibs4Support
)


# By default in kde3 kstartpref was not compiled, but well, if it compiles...
check_include_file(ltdl.h HAVE_LTDL_H)
if(HAVE_LTDL_H AND NOT APPLE AND NOT WIN32)
########### next target ###############

set(kstartperf_LIB_SRCS libkstartperf.c )


add_library(kstartperf MODULE ${kstartperf_LIB_SRCS})

target_link_libraries(kstartperf ${CMAKE_DL_LIBS})

install(TARGETS kstartperf DESTINATION ${KDE_INSTALL_PLUGINDIR} )


########### next target ###############

set(kstartperf_bin_SRCS kstartperf.cpp )


add_executable(kstartperf_bin ${kstartperf_bin_SRCS})
ecm_mark_nongui_executable(kstartperf_bin)

set_target_properties(kstartperf_bin PROPERTIES OUTPUT_NAME kstartperf)


target_link_libraries(kstartperf_bin KF5::CoreAddons KF5::KDELibs4Support)

install(TARGETS kstartperf_bin  ${KDE_INSTALL_TARGETS_DEFAULT_ARGS} )
endif()
