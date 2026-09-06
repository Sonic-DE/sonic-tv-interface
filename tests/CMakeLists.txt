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
