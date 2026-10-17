# SPDX-License-Identifier: GPL-2.0-only

find_package(Python3 REQUIRED COMPONENTS Interpreter)
find_package(Qt6 ${QT_MIN_VERSION} CONFIG REQUIRED COMPONENTS Test)
find_package(KF6Config REQUIRED)

# Launcher behavior test
add_test(
    NAME test_x11_launcher
    COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/test_x11_launcher.py
        ${CMAKE_SOURCE_DIR}/bin/plasma-bigscreen-x11.in
        ${CMAKE_SOURCE_DIR}/bin/plasma-bigscreen-common-env
)

# Session metadata test
add_test(
    NAME test_x11_session_metadata
    COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/test_x11_session_metadata.py
        ${CMAKE_BINARY_DIR}/bin/plasma-bigscreen-x11.desktop
        ${CMAKE_BINARY_DIR}/bin/plasma-bigscreen-x11-dev.desktop
        ${CMAKE_BINARY_DIR}/bin/install-sessions.sh
)

# Envmanager policy test
add_executable(envmanager_config_test envmanager_config_test.cpp)
target_include_directories(envmanager_config_test PRIVATE ${CMAKE_SOURCE_DIR}/envmanager)
target_link_libraries(envmanager_config_test PRIVATE Qt6::Test KF6::ConfigCore)
set_target_properties(envmanager_config_test PROPERTIES AUTOMOC ON)
add_test(NAME envmanager_config_test COMMAND envmanager_config_test)

# Exercise the real input/rumble policy with SDL virtual gamepads, without hardware.
add_executable(inputhandler_rumble_test
    inputhandler_rumble_test.cpp
    ${CMAKE_SOURCE_DIR}/inputhandler/controllermanager.cpp
    ${CMAKE_SOURCE_DIR}/inputhandler/sdlcontroller.cpp
    ${CMAKE_SOURCE_DIR}/inputhandler/device.cpp
    ${CMAKE_SOURCE_DIR}/inputhandler/devicewatcher.cpp
    ${CMAKE_SOURCE_DIR}/inputhandler/xdgremotedesktopsystem.cpp
)
target_include_directories(inputhandler_rumble_test PRIVATE ${CMAKE_SOURCE_DIR}/inputhandler)
kconfig_add_kcfg_files(inputhandler_rumble_test GENERATE_MOC ${CMAKE_SOURCE_DIR}/inputhandler/inputhandlersettings.kcfgc)
target_link_libraries(inputhandler_rumble_test PRIVATE Qt6::Test Qt6::DBus KF6::ConfigCore SDL3::SDL3)
add_test(NAME inputhandler_rumble_test COMMAND inputhandler_rumble_test)

if(TARGET Qt6::qmllint)
    add_test(NAME homeoverlay_qmllint
        COMMAND Qt6::qmllint -W 0
            -I ${CMAKE_BINARY_DIR}/containments/homescreen/plugin/qml
            -I ${CMAKE_BINARY_DIR}/bin
            -I ${KDE_INSTALL_FULL_QMLDIR}
            ${CMAKE_SOURCE_DIR}/containments/homescreen/package/contents/ui/homeoverlay/HomeOverlaySidebar.qml
            ${CMAKE_SOURCE_DIR}/containments/homescreen/package/contents/ui/homeoverlay/HomeOverlayWindow.qml
            ${CMAKE_SOURCE_DIR}/containments/homescreen/package/contents/ui/homeoverlay/TasksView.qml
    )
endif()
