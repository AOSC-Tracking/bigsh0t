/* SPDX-License-Identifier: GPL-2.0-or-later */
import QtQuick 2.0
import org.shotcut.qml 1.0

Metadata {
    type: Metadata.Filter
    name: qsTr("Zenith Correction")
    mlt_service: "frei0r.bigsh0t_zenith_correction"
    objectName: "bigsh0t_zenith_correction"
    qml: "ui.qml"
    keyframes {
        allowAnimateIn: true
        allowAnimateOut: true
        simpleProperties: ['yaw', "pitch", "roll"]
        parameters: [
            Parameter {
                name: qsTr('Yaw')
                property: 'yaw'
                isSimple: true
                isCurve: true
                minimum: -360
                maximum: 360
            },
            Parameter {
                name: qsTr('Pitch')
                property: 'pitch'
                isSimple: true
                isCurve: true
                minimum: -180
                maximum: 180
            },
            Parameter {
                name: qsTr('Roll')
                property: 'roll'
                isSimple: true
                isCurve: true
                minimum: -180
                maximum: 180
            }
        ]
    }
}
