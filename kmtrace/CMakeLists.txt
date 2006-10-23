project(kmtrace)
# Check if libiberty is available
find_library(LIBIBERTY_LIBRARY NAMES iberty)
if (LIBIBERTY_LIBRARY)

  include_directories( ${KDE4_INCLUDE_DIR} ${QT_INCLUDES} ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR}  )
  configure_file(${CMAKE_CURRENT_SOURCE_DIR}/kminspector.cmake ${CMAKE_CURRENT_BINARY_DIR}/kminspector )
  INSTALL(PROGRAMS ${CMAKE_CURRENT_BINARY_DIR}/kminspector DESTINATION ${BIN_INSTALL_DIR})
  ########### kmtrace ###############

  set(kmtrace_SRCS kmtrace.cpp )

  kde4_automoc(${kmtrace_SRCS})

  kde4_add_executable(kmtrace ${kmtrace_SRCS})

  target_link_libraries(kmtrace  ${KDE4_KDECORE_LIBS} ${LIBIBERTY_LIBRARY})

install(TARGETS kmtrace  DESTINATION ${BIN_INSTALL_DIR} )

  ########### demangle ###############

  set(demangle_SRCS demangle.cpp )

  kde4_automoc(${demangle_SRCS})

  kde4_add_executable(demangle ${demangle_SRCS})

  target_link_libraries(demangle  ${KDE4_KDECORE_LIBS} ${LIBIBERTY_LIBRARY})

install(TARGETS demangle  DESTINATION ${BIN_INSTALL_DIR} )


  ########### kmmatch ###############

  set(kmmatch_SRCS match.cpp )

  kde4_automoc(${kmmatch_SRCS})

  kde4_add_executable(kmmatch ${kmmatch_SRCS})

  target_link_libraries(kmmatch  ${KDE4_KDECORE_LIBS} )

install(TARGETS kmmatch  DESTINATION ${BIN_INSTALL_DIR} )


  ########### ktrace ###############

  set(ktrace_LIB_SRCS ksotrace.cpp ktrace.c )

  kde4_automoc(${ktrace_LIB_SRCS})

  kde4_add_library(ktrace SHARED ${ktrace_LIB_SRCS})

  target_link_libraries(ktrace  ${KDE4_KDECORE_LIBS} )

  set_target_properties(ktrace PROPERTIES VERSION 1.0.0 SOVERSION 1 )
install(TARGETS ktrace  DESTINATION ${LIB_INSTALL_DIR} )

  ########### install files ###############

install( FILES kde.excludes  DESTINATION  ${DATA_INSTALL_DIR}/kmtrace )
  install( FILES ktrace.h DESTINATION ${INCLUDE_INSTALL_DIR} )

endif (LIBIBERTY_LIBRARY)
