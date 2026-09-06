<!--
- SPDX-FileCopyrightText: None
- SPDX-License-Identifier: CC0-1.0
-->

# Plasma Bigscreen

This repository contains shell components for Plasma Bigscreen.

* Project page: [plasma-bigscreen.org](https://plasma-bigscreen.org)
* Repository: [invent.kde.org/plasma/plasma-bigscreen](https://invent.kde.org/plasma/plasma-bigscreen)
* Documentation: [invent.kde.org/plasma/plasma-bigscreen/-/wikis/home](https://invent.kde.org/plasma/plasma-bigscreen/-/wikis/home)
* Development channel: [matrix.to/#/#plasma-bigscreen:kde.org](https://matrix.to/#/#plasma-bigscreen:kde.org)

Plasma Bigscreen is a user-friendly, open-source, X11 desktop shell designed for devices like HTPCs and SBCs connected to TVs and projectors. It provides an intuitive experience that allows for easy navigation from a distance using remote controls. Discover an engaging environment that adapts to your preferences, offering the safety and privacy protection that come with free and open source software!

<img src="lookandfeel/contents/splash/images/logo-big.svg" width=100px/>

### Locations

* [components](components) - Shell components & controls libraries
* [containments](containments) - Shell components (homescreen)
* [envmanager](envmanager) - Utility that sets Plasma configuration
* [inputhandler](inputhandler) - Daemon that interprets controllers and TV remotes as keyboard input
* [kcms](kcms) - Settings modules
* [lookandfeel](lookandfeel/contents) - Plasma look-and-feel package
* [settingsapp](settingsapp) - The standalone settings application
* [shell](shell) - Plasma shell package, provides implementations for applet and containment configuration dialogs
* [uvcviewer](uvcviewer) - Standalone application to view camera input, paired with a UVC adapter it can view input from other devices (ex. consoles)
* [webapp-viewer](webapp-viewer) - Application to view webapps added from the settings

<img src="/screenshots/homescreen.png" width=500px/>

### CEC and controller support

The `plasma-bigscreen-inputhandler` utility daemon handles CEC and game controller support. You can manually run it with:

```bash
PLASMA_PLATFORM=mediacenter plasma-bigscreen-inputhandler
```

For CEC support, ensure that your user has permission to access the device:

```bash
usermod -aG input $USER
```

### Test on a development machine

It is recommended to use `kde-builder` to build this from source.
See [this wiki page](https://invent.kde.org/plasma/plasma-bigscreen/-/wikis/Building-and-Testing-Locally) in order to set it up.

<details>
<summary><b>Click here to see dependencies</b></summary>

### KDE Plasma Dependencies

- [Plasma Nano](https://invent.kde.org/plasma/plasma-nano)
- [Plasma NM](https://invent.kde.org/plasma/plasma-nm)
- [Plasma PA](https://invent.kde.org/plasma/plasma-pa)
- [Milou](https://invent.kde.org/plasma/milou)
- [KScreen](https://invent.kde.org/plasma/libkscreen)
- [KWin](https://invent.kde.org/plasma/kwin)

### KDE Frameworks Dependencies

- Activities
- ActivitiesStats
- Plasma
- BluezQt
- I18n
- [Kirigami](https://invent.kde.org/frameworks/kirigami)
- KCMUtils
- GlobalAccel
- Notifications
- PlasmaQuick
- KIO
- Wayland
- WindowSystem
- [KDE Connect](https://invent.kde.org/network/kdeconnect-kde)
- SVG

### Other dependencies

- QCoro
- SDL (for game controller support)
- libcec (optional: for TV Controller support)

</details>

To start the Bigscreen session for development, use the development launcher after installing the development session:

```bash
# Install the development session (requires sudo for system directories)
sudo ./build/bin/install-sessions.sh

# Or run the development launcher directly
./build/bin/plasma-bigscreen-x11-dev
```

Bigscreen is the desktop shell selected by `PLASMA_DEFAULT_SHELL`, while `startplasma-x11` owns Sonic Win/KWin and the single `plasmashell` process.

<br/>

<img src="https://invent.kde.org/plasma/plasma-bigscreen/-/wikis/uploads/92914bdc119ad89fb0436c1ad59e1375/image.png" width=300px>
