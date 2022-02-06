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
                isSimple: true
                isCurve: true
                minimum: -180
                maximum: 180
            },
            Parameter {
                name: qsTr('hfov1')
                property: 'hfov1'
                isSimple: true
                isCurve: true
                minimum: -180
                maximum: 180
            },
            Parameter {
                name: qsTr('vfov0')
                property: 'vfov0'
                isSimple: true
                isCurve: true
                minimum: -90
                maximum: 90
            },
            Parameter {
                name: qsTr('vfov1')
                property: 'vfov1'
                isSimple: true
                isCurve: true
                minimum: -90
                maximum: 90
            },
            Parameter {
                name: qsTr('blurStart')
                property: 'blurStart'
                isSimple: true
                isCurve: true
                minimum: 0.0
                maximum: 2.0
            },
            Parameter {
                name: qsTr('blurEnd')
                property: 'blurEnd'
                isSimple: true
                isCurve: true
                minimum: 0.0
                maximum: 2.0
            }
        ]
    }
}
