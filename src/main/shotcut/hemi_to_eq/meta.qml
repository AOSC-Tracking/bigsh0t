/* SPDX-License-Identifier: GPL-2.0-or-later */
import QtQuick 2.0
import org.shotcut.qml 1.0

Metadata {
    type: Metadata.Filter
    name: qsTr("Hemispherical to Equirectangular")
    mlt_service: "frei0r.hemi_to_eq"
    objectName: "hemi_to_eq"
    qml: "ui.qml"
    keyframes {
        allowAnimateIn: true
        allowAnimateOut: true
        simpleProperties: ['yaw', "pitch", "roll", "frontX", "frontY", "frontUp", "backX", "backY", "backUp", "fov", "radius", "nadirRadius", "nadirCorrectionStart"]
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
            },
            Parameter {
                name: qsTr('FrontX')
                property: 'frontX'
                isSimple: true
                isCurve: true
                minimum: 0
                maximum: 1
            },
            Parameter {
                name: qsTr('FrontY')
                property: 'frontY'
                isSimple: true
                isCurve: true
                minimum: 0
                maximum: 1
            },
            Parameter {
                name: qsTr('FrontUp')
                property: 'frontUp'
                isSimple: true
                isCurve: true
                minimum: -360
                maximum: 360
            },
            Parameter {
                name: qsTr('BackX')
                property: 'backX'
                isSimple: true
                isCurve: true
                minimum: 0
                maximum: 1
            },
            Parameter {
                name: qsTr('BackY')
                property: 'backY'
                isSimple: true
                isCurve: true
                minimum: 0
                maximum: 1
            },
            Parameter {
                name: qsTr('BackUp')
                property: 'backUp'
                isSimple: true
                isCurve: true
                minimum: -360
                maximum: 360
            },
            Parameter {
                name: qsTr('Fov')
                property: 'fov'
                isSimple: true
                isCurve: true
                minimum: 0
                maximum: 360
            },
            Parameter {
                name: qsTr('Radius')
                property: 'radius'
                isSimple: true
                isCurve: true
                minimum: 0
                maximum: 1
            },
            Parameter {
                name: qsTr('NadirRadius')
                property: 'nadirRadius'
                isSimple: true
                isCurve: true
                minimum: 0
                maximum: 1
            },
            Parameter {
                name: qsTr('nadirCorrectionStart')
                property: 'nadirCorrectionStart'
                isSimple: true
                isCurve: true
                minimum: 0
                maximum: 1
            }
        ]
    }
}
