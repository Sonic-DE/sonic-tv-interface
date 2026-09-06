#!/usr/bin/env python3
# SPDX-License-Identifier: GPL-2.0-only
"""Test that the X11 launcher sources common env, calls envmanager, and execs startplasma-x11."""
import os
import shutil
import stat
import subprocess
import sys
import tempfile
import unittest
from pathlib import Path


class TestX11Launcher(unittest.TestCase):

    def setUp(self):
        self.tmpdir = tempfile.mkdtemp(prefix="tv-launcher-test-")
        self.sandbox = Path(self.tmpdir)
        self.bindir = self.sandbox / "bin"
        self.bindir.mkdir(parents=True)

        self.launcher_in = Path(sys.argv[1])
        self.common_env_src = Path(sys.argv[2])

    def tearDown(self):
        shutil.rmtree(self.tmpdir, ignore_errors=True)

    def _write_fake_command(self, name: str, content: str):
        path = self.bindir / name
        path.write_text(content)
        path.chmod(path.stat().st_mode | stat.S_IEXEC | stat.S_IXGRP | stat.S_IXOTH)

    def test_launcher_sources_env_and_execs_startplasma_x11(self):
        # Copy launcher unchanged
        launcher = self.bindir / "plasma-bigscreen-x11"
        launcher.write_text(self.launcher_in.read_text())
        launcher.chmod(launcher.stat().st_mode | stat.S_IEXEC)

        # Copy common env, neutralize /etc/profile sourcing
        common_env = self.bindir / "plasma-bigscreen-common-env"
        env_content = self.common_env_src.read_text()
        env_content = env_content.replace("[ -f /etc/profile ] && . /etc/profile", "true")
        common_env.write_text(env_content)
        common_env.chmod(common_env.stat().st_mode | stat.S_IEXEC)

        # Fake envmanager that records its invocation
        self._write_fake_command("plasma-bigscreen-envmanager", """#!/bin/sh
echo "ENVMANAGER_CALLED $@" >> "$LAUNCHER_TEST_LOG"
""")

        # Fake startplasma-x11 that records environment and exits with distinctive code
        self._write_fake_command("startplasma-x11", """#!/bin/sh
echo "STARTPLASMA_X11_CALLED" >> "$LAUNCHER_TEST_LOG"
echo "PLASMA_PLATFORM=$PLASMA_PLATFORM" >> "$LAUNCHER_TEST_LOG"
echo "QT_FILE_SELECTORS=$QT_FILE_SELECTORS" >> "$LAUNCHER_TEST_LOG"
echo "PLASMA_DEFAULT_SHELL=$PLASMA_DEFAULT_SHELL" >> "$LAUNCHER_TEST_LOG"
echo "PLASMA_INTEGRATION_USE_PORTAL=$PLASMA_INTEGRATION_USE_PORTAL" >> "$LAUNCHER_TEST_LOG"
echo "XDG_CONFIG_DIRS=$XDG_CONFIG_DIRS" >> "$LAUNCHER_TEST_LOG"
exit 42
""")

        log_file = self.sandbox / "test.log"
        env = {
            "PATH": str(self.bindir) + ":" + os.environ.get("PATH", ""),
            "HOME": str(self.sandbox),
            "LAUNCHER_TEST_LOG": str(log_file),
        }

        result = subprocess.run(
            [str(launcher)],
            capture_output=True, text=True, env=env)

        # Launcher should propagate the exit code from startplasma-x11
        self.assertEqual(result.returncode, 42,
                         f"Expected exit code 42, got {result.returncode}: {result.stderr}")

        log = log_file.read_text()

        # envmanager was called once with --apply-settings
        envmgr_lines = [l for l in log.splitlines() if l.startswith("ENVMANAGER_CALLED")]
        self.assertEqual(len(envmgr_lines), 1, f"Expected 1 envmanager call, got {len(envmgr_lines)}")
        self.assertIn("--apply-settings", envmgr_lines[0])

        # startplasma-x11 was called
        self.assertIn("STARTPLASMA_X11_CALLED", log)

        # Environment variables propagated
        self.assertIn("PLASMA_PLATFORM=mediacenter", log)
        self.assertIn("QT_FILE_SELECTORS=mediacenter", log)
        self.assertIn("PLASMA_DEFAULT_SHELL=org.kde.plasma.bigscreen", log)
        self.assertIn("PLASMA_INTEGRATION_USE_PORTAL=1", log)
        self.assertIn("plasma-bigscreen", log)  # XDG_CONFIG_DIRS contains plasma-bigscreen

        # No kwin or plasmashell was invoked
        self.assertNotIn("kwin", log.lower())
        self.assertNotIn("plasmashell", log.lower())


if __name__ == "__main__":
    unittest.main(argv=sys.argv[:1])
