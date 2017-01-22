project(kmtrace)

cmake_minimum_required(VERSION 2.8.12)

find_package(ECM 1.7 REQUIRED NO_MODULE)
set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH} ${ECM_KDE_MODULE_DIR})

set(QT_MIN_VERSION "5.5.0")

include(FeatureSummary)
include(KDEInstallDirs)
include(KDECMakeSettings)
include(KDECompilerSettings NO_POLICY_SCOPE)
include(CheckIncludeFile)
include(CheckIncludeFileCXX)
include(CheckLibraryExists)
include(CheckCSourceCompiles)

find_package(Qt5 ${QT_MIN_VERSION} REQUIRED NO_MODULE
    COMPONENTS
    Core
)

find_package(KF5 REQUIRED
    COMPONENTS
    CoreAddons
    KDELibs4Support
)

check_c_source_compiles("
#include <stdlib.h>

int main() {
#ifndef __GLIBC__
  choke me
#endif
  return 0;
}" LIBC_IS_GLIBC)

# Check if libiberty is available
find_library(LIBIBERTY_LIBRARY NAMES iberty)
if (LIBIBERTY_LIBRARY AND LIBC_IS_GLIBC)

  include_directories(   ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR}  )
  configure_file(${CMAKE_CURRENT_SOURCE_DIR}/kminspector.cmake ${CMAKE_CURRENT_BINARY_DIR}/kminspector )
  INSTALL(PROGRAMS ${CMAKE_CURRENT_BINARY_DIR}/kminspector DESTINATION ${KDE_INSTALL_BINDIR})
  ########### kmtrace ###############

  set(kmtrace_SRCS kmtrace.cpp)


  add_executable(kmtrace ${kmtrace_SRCS})

  target_link_libraries(kmtrace ${LIBIBERTY_LIBRARY} KF5::KDELibs4Support ${QT_QT3SUPPORT_LIBRARY})

install(TARGETS kmtrace ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})

  ########### demangle ###############

  set(demangle_SRCS demangle.cpp)


  add_executable(demangle ${demangle_SRCS})

  target_link_libraries(demangle   ${LIBIBERTY_LIBRARY} Qt5::Core)

install(TARGETS demangle ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})


  ########### kmmatch ###############

  set(kmmatch_SRCS match.cpp)


  add_executable(kmmatch ${kmmatch_SRCS})

  target_link_libraries(kmmatch   Qt5::Core)

install(TARGETS kmmatch ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})


  ########### ktrace ###############

  set(ktrace_LIB_SRCS ksotrace.cpp ktrace.c)


  add_library(ktrace SHARED ${ktrace_LIB_SRCS})

  target_link_libraries(ktrace   dl)

  set_target_properties(ktrace PROPERTIES VERSION ${GENERIC_LIB_VERSION} SOVERSION ${GENERIC_LIB_SOVERSION})
install(TARGETS ktrace ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})

  ########### install files ###############

install( FILES kde.excludes  DESTINATION  ${KDE_INSTALL_DATADIR}/kmtrace )
  install( FILES ktrace.h DESTINATION ${KDE_INSTALL_INCLUDEDIR} COMPONENT Devel )

add_subdirectory( doc )

endif ()
