/* SPDX-License-Identifier: GPL-2.0-or-later */
import QtQuick 2.0
import org.shotcut.qml 1.0

Metadata {
    type: Metadata.Filter
    name: qsTr("Rectilinear to Equirectangular")
    mlt_service: "frei0r.rect_to_eq"
    objectName: "rect_to_eq"
    qml: "ui.qml"
    keyframes {
        allowAnimateIn: true
        allowAnimateOut: true
        simpleProperties: ["hfov", "vfov"]
        parameters: [
            Parameter {
                name: qsTr('hfov')
                property: 'hfov'
                isSimple: true
                isCurve: true
                minimum: 0
                maximum: 180
            },
            Parameter {
                name: qsTr('vfov')
                property: 'vfov'
                isSimple: true
                isCurve: true
                minimum: 0
                maximum: 180
            }
        ]
    }
}
