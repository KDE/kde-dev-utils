project(kstartperf)



########### next target ###############

set(kstartperf_LIB_SRCS libkstartperf.c )

kde4_automoc(${kstartperf_LIB_SRCS})

kde4_add_library(kstartperf SHARED ${kstartperf_LIB_SRCS})

target_link_libraries(kstartperf dl)

set_target_properties(kstartperf PROPERTIES VERSION 1.0.0 SOVERSION 1 )
install(TARGETS kstartperf  DESTINATION ${LIB_INSTALL_DIR} )


########### next target ###############

set(kstartperf_bin_SRCS kstartperf.cpp )

kde4_automoc(${kstartperf_bin_SRCS})

kde4_add_executable(kstartperf_bin ${kstartperf_bin_SRCS})

set_target_properties(kstartperf_bin PROPERTIES OUTPUT_NAME kstartperf)


target_link_libraries(kstartperf_bin  ${KDE4_KDECORE_LIBS} kstartperf)

install(TARGETS kstartperf_bin  DESTINATION ${BIN_INSTALL_DIR} )
