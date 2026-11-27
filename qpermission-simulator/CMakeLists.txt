# SPDX-FileCopyrightText: 2026 Volker Krause <vkrause@kde.org>
# SPDX-License-Identifier: BSD-2-Clause

find_package(Qt6 ${QT_MIN_VERSION} REQUIRED COMPONENTS
    Core
    CorePrivate
    Widgets
)

add_library(qpermissionsimulationplugin MODULE)
target_sources(qpermissionsimulationplugin PRIVATE
    qpermissionsimulatorplugin.cpp
)
target_link_libraries(qpermissionsimulationplugin PRIVATE Qt::CorePrivate Qt::Widgets)

install(TARGETS qpermissionsimulationplugin DESTINATION ${KDE_INSTALL_LIBDIR}/qpermission-simulator/plugins/permissions)

configure_file(config.h.in ${CMAKE_CURRENT_BINARY_DIR}/config.h)
add_executable(qpermission-simulator)
target_sources(qpermission-simulator PRIVATE main.cpp)
target_link_libraries(qpermission-simulator PRIVATE Qt6::Core)

install(TARGETS qpermission-simulator ${KDE_INSTALL_DEFAULT_TARGETS})
