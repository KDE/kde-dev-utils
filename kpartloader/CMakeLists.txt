########### kpartloader ###############

set(kpartloader_SRCS
 kpartloader.cpp
)

kde4_add_executable(kpartloader ${kpartloader_SRCS})
target_link_libraries(kpartloader ${KDE4_KPARTS_LIBS})
install(TARGETS kpartloader DESTINATION ${BIN_INSTALL_DIR})
install(FILES kpartloaderui.rc DESTINATION ${DATA_INSTALL_DIR}/kpartloader)

