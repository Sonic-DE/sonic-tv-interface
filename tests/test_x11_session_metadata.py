#!/usr/bin/env python3
# SPDX-License-Identifier: GPL-2.0-only
"""Test X11 session desktop file metadata."""
import configparser
import re
import subprocess
import sys
import unittest
from pathlib import Path


class TestX11SessionMetadata(unittest.TestCase):

    def setUp(self):
        self.prod_desktop = Path(sys.argv[1])
        self.dev_desktop = Path(sys.argv[2])
        self.install_script = Path(sys.argv[3])

    def _parse_desktop(self, path: Path) -> configparser.RawConfigParser:
        parser = configparser.RawConfigParser(interpolation=None)
        parser.optionxform = str  # preserve case
        parser.read(str(path))
        return parser

    def test_production_desktop_fields(self):
        parser = self._parse_desktop(self.prod_desktop)
        self.assertTrue(parser.has_option("Desktop Entry", "Type"))
        self.assertEqual(parser.get("Desktop Entry", "Type"), "XSession")
        self.assertIn("plasma-bigscreen-x11", parser.get("Desktop Entry", "Exec"))
        self.assertIn("plasma-dbus-run-session-if-needed", parser.get("Desktop Entry", "Exec"))
        self.assertIn("plasma-bigscreen-x11", parser.get("Desktop Entry", "TryExec"))
        self.assertEqual(parser.get("Desktop Entry", "DesktopNames"), "KDE")
        self.assertEqual(parser.get("Desktop Entry", "OnlyShowIn"), "KDE")
        self.assertTrue(parser.has_option("Desktop Entry", "X-KDE-PluginInfo-Version"))

    def test_development_desktop_fields(self):
        parser = self._parse_desktop(self.dev_desktop)
        self.assertTrue(parser.has_option("Desktop Entry", "Type"))
        self.assertEqual(parser.get("Desktop Entry", "Type"), "XSession")
        self.assertIn("plasma-bigscreen-x11-dev", parser.get("Desktop Entry", "Exec"))
        self.assertIn("plasma-dbus-run-session-if-needed", parser.get("Desktop Entry", "Exec"))
        self.assertIn("plasma-bigscreen-x11-dev", parser.get("Desktop Entry", "TryExec"))
        self.assertEqual(parser.get("Desktop Entry", "DesktopNames"), "KDE")
        self.assertTrue(parser.has_option("Desktop Entry", "X-KDE-PluginInfo-Version"))

    def test_install_script_uses_xsessions_and_bindir(self):
        content = self.install_script.read_text()
        self.assertIn("/usr/share/xsessions/", content)
        self.assertNotIn("wayland-sessions", content)
        self.assertNotIn("CMAKE_INSTALL_FULL_LIBEXECDIR", content)
        self.assertIn("plasma-bigscreen-x11-dev", content)
        # The configured script should install the launcher into a bin directory
        # (the CMake template @CMAKE_INSTALL_FULL_BINDIR@ is substituted at configure time)
        self.assertIn("/bin", content)


if __name__ == "__main__":
    unittest.main(argv=sys.argv[:1])
