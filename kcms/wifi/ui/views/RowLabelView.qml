/*
    SPDX-FileCopyrightText: 2019 Aditya Mehra <aix.m@outlook.com>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

import QtQuick
import QtQuick.Effects
import QtQuick.Layouts

import org.kde.plasma.components 3.0 as PlasmaComponents
import org.kde.plasma.extras 2.0 as PlasmaExtras

import org.kde.kirigami as Kirigami

Rectangle {
    id: rowLabel
    Layout.bottomMargin: -Kirigami.Units.gridUnit * 2
    Layout.topMargin: -Kirigami.Units.gridUnit * 0.4
    Layout.leftMargin: -Kirigami.Units.gridUnit * 1
    Layout.preferredWidth: parent.width / 5
    Layout.fillHeight: true
    z: 100
    property alias text: deviceTypeHeading.text

    Kirigami.Heading {
        id: deviceTypeHeading
        anchors.centerIn: parent
        level: 3
    }

    MultiEffect {
        anchors.fill: deviceTypeHeading
        shadowEnabled: true
        shadowHorizontalOffset: 0
        shadowVerticalOffset: 2
        shadowBlur: 1
        blurMax: 8
        shadowColor: Qt.rgba(0,0,0,0.6)
        source: deviceTypeHeading
    }
}
