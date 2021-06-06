/* SPDX-License-Identifier: GPL-2.0-or-later */
import QtQuick 2.0
import org.shotcut.qml 1.0

Metadata {
    type: Metadata.Filter
    name: qsTr("360: Equirectangular Mask")
    mlt_service: "frei0r.bigsh0t_eq_mask"
    objectName: "bigsh0t_eq_mask"
    qml: "ui.qml"
    keyframes {
        allowAnimateIn: true
        allowAnimateOut: true
        simpleProperties: []
        parameters: [
            Parameter {
                name: qsTr('hfov0')
                property: 'hfov0'
                isSimple: true
                isCurve: true
                minimum: 0
                maximum: 360
            },
            Parameter {
                name: qsTr('hfov1')
                property: 'hfov1'
                isSimple: true
                isCurve: true
                minimum: 0
                maximum: 360
            },
            Parameter {
                name: qsTr('vfov0')
                property: 'vfov0'
                isSimple: true
                isCurve: true
                minimum: 0
                maximum: 360
            },
            Parameter {
                name: qsTr('vfov1')
                property: 'vfov1'
                isSimple: true
                isCurve: true
                minimum: 0
                maximum: 360
            }
        ]
    }
}
