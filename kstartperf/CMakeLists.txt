project(kstartperf)



########### next target ###############

set(kstartperf_LIB_SRCS libkstartperf.c )


kde4_add_library(kstartperf SHARED ${kstartperf_LIB_SRCS})

target_link_libraries(kstartperf dl)

set_target_properties(kstartperf PROPERTIES VERSION ${GENERIC_LIB_VERSION} SOVERSION ${GENERIC_LIB_SOVERSION} )
install(TARGETS kstartperf ${INSTALL_TARGETS_DEFAULT_ARGS} )


########### next target ###############

set(kstartperf_bin_SRCS kstartperf.cpp )


kde4_add_executable(kstartperf_bin NOGUI ${kstartperf_bin_SRCS})

set_target_properties(kstartperf_bin PROPERTIES OUTPUT_NAME kstartperf)


target_link_libraries(kstartperf_bin  ${KDE4_KDECORE_LIBS} kstartperf)

install(TARGETS kstartperf_bin  ${INSTALL_TARGETS_DEFAULT_ARGS} )
