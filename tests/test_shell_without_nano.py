#!/usr/bin/env python3
# SPDX-FileCopyrightText: 2026 Joseph Crowell <joseph.w.crowell@gmail.com>
# SPDX-License-Identifier: GPL-2.0-or-later
"""SonicDE doesn't ship plasma-nano, so the TV shell must not need it."""
import json
import re
import sys
import unittest
from pathlib import Path

# Files sonicshell loads from the shell package for every containment and applet.
REQUIRED_SHELL_FILES = [
    "views/Desktop.qml",
    "applet/AppletError.qml",
    "applet/CompactApplet.qml",
    "applet/DefaultCompactRepresentation.qml",
]


class TestShellWithoutNano(unittest.TestCase):

    def setUp(self):
        self.source_dir = Path(sys.argv[1])
        self.shell_dir = self.source_dir / "shell"

    def test_no_qml_imports_nanoshell(self):
        importing = [
            str(path.relative_to(self.source_dir))
            for path in sorted(self.source_dir.rglob("*.qml"))
            if re.search(r"^\s*import\s+org\.kde\.plasma\.private\.nanoshell\b",
                         path.read_text(encoding="utf-8"), re.MULTILINE)
        ]
        self.assertEqual(importing, [])

    def test_shell_has_no_fallback_package(self):
        metadata = json.loads((self.shell_dir / "metadata.json").read_text(encoding="utf-8"))
        self.assertIsNone(metadata.get("X-Plasma-FallbackPackage"))

    def test_shell_provides_required_files(self):
        missing = [name for name in REQUIRED_SHELL_FILES
                   if not (self.shell_dir / "contents" / name).is_file()]
        self.assertEqual(missing, [])


if __name__ == "__main__":
    unittest.main(argv=sys.argv[:1])
