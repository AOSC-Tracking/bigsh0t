// SPDX-License-Identifier: GPL-2.0-or-later
import QtQuick
import org.shotcut.qml

Metadata {
    type: Metadata.Filter
    name: qsTr("360: Zenith Correction")
    keywords: qsTr('spherical level', 'search keywords for the 360: Zenith correction filter') + ' 360: zenith correction bigsh0t'
    mlt_service: "frei0r.bigsh0t_zenith_correction"
    objectName: "bigsh0t_zenith_correction"
    qml: "ui.qml"
    icon: "icon.webp"
}
