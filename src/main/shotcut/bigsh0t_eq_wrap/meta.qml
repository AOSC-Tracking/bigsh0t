/* SPDX-License-Identifier: GPL-2.0-or-later */
import QtQuick 2.0
import org.shotcut.qml 1.0

Metadata {
    type: Metadata.Filter
    name: qsTr("360: Equirectangular Wrap")
    mlt_service: "frei0r.bigsh0t_eq_wrap"
    objectName: "bigsh0t_eq_wrap"
    qml: "ui.qml"
    keyframes {
        allowAnimateIn: true
        allowAnimateOut: true
        simpleProperties: ["hfov0", "hfov1", "vfov0", "vfov1", "blurStart", "blurEnd"]
        parameters: [
            Parameter {
                name: qsTr('hfov0')
                property: 'hfov0'
                isCurve: true
                minimum: -180
                maximum: 180
            },
            Parameter {
                name: qsTr('hfov1')
                property: 'hfov1'
                isCurve: true
                minimum: -180
                maximum: 180
            },
            Parameter {
                name: qsTr('vfov0')
                property: 'vfov0'
                isCurve: true
                minimum: -90
                maximum: 90
            },
            Parameter {
                name: qsTr('vfov1')
                property: 'vfov1'
                isCurve: true
                minimum: -90
                maximum: 90
            },
            Parameter {
                name: qsTr('blurStart')
                property: 'blurStart'
                isCurve: true
                minimum: 0.0
                maximum: 2.0
            },
            Parameter {
                name: qsTr('blurEnd')
                property: 'blurEnd'
                isCurve: true
                minimum: 0.0
                maximum: 2.0
            }
        ]
    }
}
