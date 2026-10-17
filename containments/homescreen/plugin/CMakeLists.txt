# SPDX-FileCopyrightText: 2019 Marco Martin <mart@kde.org>
# SPDX-FileCopyrightText: 2019 Aditya Mehra <aix.m@outlook.com>
# SPDX-License-Identifier: GPL-2.0-or-later

kde_enable_exceptions()

add_definitions(-DTRANSLATION_DOMAIN=\"biglauncherhomescreen\")

set(homescreen_SRCS
    biglauncherhomescreen.cpp biglauncherhomescreen.h
    biglauncher_dbus.cpp biglauncher_dbus.h
    applicationlistmodel.cpp applicationlistmodel.h
    favslistmodel.cpp favslistmodel.h
    shortcuts.cpp shortcuts.h
)

# DBus Interface
qt_generate_dbus_interface(
    ${CMAKE_CURRENT_SOURCE_DIR}/biglauncher_dbus.h
    org.kde.biglauncher.xml
    OPTIONS -s -m -P
)
qt_add_dbus_adaptor(homescreen_SRCS ${CMAKE_CURRENT_BINARY_DIR}/org.kde.biglauncher.xml
                    ${CMAKE_CURRENT_SOURCE_DIR}/biglauncher_dbus.h BigLauncherDbusAdapterInterface)
qt_add_dbus_interface(homescreen_SRCS ${CMAKE_CURRENT_BINARY_DIR}/org.kde.biglauncher.xml biglauncherinterface)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/org.kde.biglauncher.xml DESTINATION ${KDE_INSTALL_DBUSINTERFACEDIR})
# ---

kcoreaddons_add_plugin(org.kde.bigscreen.homescreen INSTALL_NAMESPACE "plasma/applets")
target_sources(org.kde.bigscreen.homescreen PRIVATE ${homescreen_SRCS})

# The applet registers this module manually when its containment is created.
# Generate tooling metadata without compiling automatic registrations or
# installing a second runtime module.
qt_extract_metatypes(org.kde.bigscreen.homescreen OUTPUT_FILES homescreen_metatypes)
set_target_properties(org.kde.bigscreen.homescreen PROPERTIES QT_CONSUMES_METATYPES TRUE)
set(homescreen_foreign_types "$<REMOVE_DUPLICATES:$<FILTER:$<TARGET_PROPERTY:org.kde.bigscreen.homescreen,SOURCES>,INCLUDE,metatypes.json$>>")
file(GENERATE OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/homescreen_foreign_types.rsp"
    CONTENT "--foreign-types=$<JOIN:${homescreen_foreign_types},$<COMMA>>\n")
set(homescreen_lint_dir "${CMAKE_CURRENT_BINARY_DIR}/qml/org/kde/private/biglauncher")
file(GENERATE OUTPUT "${homescreen_lint_dir}/qmldir" CONTENT
    "module org.kde.private.biglauncher\ntypeinfo biglauncher.qmltypes\n")
add_custom_command(
    OUTPUT "${homescreen_lint_dir}/biglauncher.qmltypes"
    BYPRODUCTS "${CMAKE_CURRENT_BINARY_DIR}/biglauncher_tooling_registrations.cpp"
    COMMAND Qt6::qmltyperegistrar
        "@${CMAKE_CURRENT_BINARY_DIR}/homescreen_foreign_types.rsp"
        --generate-qmltypes "${homescreen_lint_dir}/biglauncher.qmltypes"
        --import-name org.kde.private.biglauncher
        --major-version 1 --minor-version 0
        -o "${CMAKE_CURRENT_BINARY_DIR}/biglauncher_tooling_registrations.cpp"
        ${homescreen_metatypes}
    DEPENDS ${homescreen_metatypes} ${homescreen_foreign_types}
        "${CMAKE_CURRENT_BINARY_DIR}/homescreen_foreign_types.rsp"
        Qt6::qmltyperegistrar
    VERBATIM
)
add_custom_target(homescreen_qml_tooling ALL
    DEPENDS "${homescreen_lint_dir}/biglauncher.qmltypes")

find_package(LibTaskManager REQUIRED)

target_link_libraries(org.kde.bigscreen.homescreen
    Qt::Gui
    Qt::Core
    Qt::Qml
    Qt::DBus
    Qt::Network
    Qt::Quick
    Plasma::Plasma
    KF6::I18n
    KF6::Notifications
    KF6::Service
    KF6::KIOWidgets
    KF6::KCMUtilsQuick
    KF6::GlobalAccel
    Plasma::Activities
    PW::LibTaskManager
    PW::KWorkspace
    Plasma::ActivitiesStats
)
