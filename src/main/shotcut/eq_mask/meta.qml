/* SPDX-License-Identifier: GPL-2.0-or-later */
import QtQuick 2.0
import org.shotcut.qml 1.0

Metadata {
    type: Metadata.Filter
    name: qsTr("Equirectangular Mask")
    mlt_service: "frei0r.eq_mask"
    objectName: "eq_mask"
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
