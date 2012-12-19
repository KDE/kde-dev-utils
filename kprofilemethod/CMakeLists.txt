project(kprofilemethod)

find_package(KDE4 REQUIRED)
include(KDE4Defaults)
include(MacroLibrary)
include_directories (${CMAKE_SOURCE_DIR} ${CMAKE_BINARY_DIR} ${KDE4_INCLUDES})

install( FILES kprofilemethod.h DESTINATION ${INCLUDE_INSTALL_DIR} COMPONENT Devel)
