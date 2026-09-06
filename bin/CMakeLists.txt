# SPDX-FileCopyrightText: 2020 Bart Ribbers <bribbers@disroot.org>
# SPDX-FileCopyrightText: 2025 Seshan Ravikumar <seshan@sineware.ca>

# SPDX-License-Identifier: GPL-2.0-or-later

configure_file(plasma-bigscreen-x11.in ${CMAKE_CURRENT_BINARY_DIR}/plasma-bigscreen-x11)
install(PROGRAMS
	${CMAKE_CURRENT_BINARY_DIR}/plasma-bigscreen-x11
	plasma-bigscreen-common-env
	plasma-bigscreen-swap-session
	DESTINATION ${KDE_INSTALL_BINDIR})

configure_file(plasma-bigscreen-x11.desktop.cmake ${CMAKE_CURRENT_BINARY_DIR}/plasma-bigscreen-x11.desktop)
configure_file(plasma-bigscreen-swap-session.desktop.cmake ${CMAKE_CURRENT_BINARY_DIR}/plasma-bigscreen-swap-session.desktop)

install(FILES
	${CMAKE_CURRENT_BINARY_DIR}/plasma-bigscreen-x11.desktop
    DESTINATION ${KDE_INSTALL_DATADIR}/xsessions
)

install(FILES
	${CMAKE_CURRENT_BINARY_DIR}/plasma-bigscreen-swap-session.desktop
	DESTINATION ${KDE_INSTALL_APPDIR}
)

# Dev session for Plasma developers
configure_file(plasma-bigscreen-x11-dev.in ${CMAKE_CURRENT_BINARY_DIR}/plasma-bigscreen-x11-dev)
configure_file(plasma-bigscreen-x11-dev.desktop.cmake ${CMAKE_CURRENT_BINARY_DIR}/plasma-bigscreen-x11-dev.desktop)
configure_file(install-sessions.sh.cmake ${CMAKE_CURRENT_BINARY_DIR}/install-sessions.sh)
