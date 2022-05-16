/* SPDX-License-Identifier: GPL-2.0-or-later */
import QtQuick 2.0
import org.shotcut.qml 1.0

Metadata {
    type: Metadata.Filter
    name: qsTr("360: Hemispherical to Equirectangular")
    mlt_service: "frei0r.bigsh0t_hemi_to_eq"
    objectName: "bigsh0t_hemi_to_eq"
    qml: "ui.qml"
    keyframes {
        allowAnimateIn: true
        allowAnimateOut: true
        simpleProperties: ['yaw', "pitch", "roll", "frontX", "frontY", "frontUp", "backX", "backY", "backUp", "fov", "radius", "nadirRadius", "nadirCorrectionStart",
            "distortionA", "distortionB", "distortionC", "distortionRadius",
            "vignettingA", "vignettingB", "vignettingC", "vignettingD", "vignettingRadius",
            "emorH1", "emorH2", "emorH3", "emorH4", "emorH5"]
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
            },
            Parameter {
                name: qsTr('distortionA')
                property: 'distortionA'
                isSimple: true
                isCurve: true
                minimum: -2
                maximum: 2
            },
            Parameter {
                name: qsTr('distortionB')
                property: 'distortionB'
                isSimple: true
                isCurve: true
                minimum: -2
                maximum: 2
            },
            Parameter {
                name: qsTr('distortionC')
                property: 'distortionC'
                isSimple: true
                isCurve: true
                minimum: -2
                maximum: 2
            },
            Parameter {
                name: qsTr('distortionRadius')
                property: 'distortionRadius'
                isSimple: true
                isCurve: true
                minimum: 0
                maximum: 1
            },
            Parameter {
                name: qsTr('vignettingA')
                property: 'vignettingA'
                isSimple: true
                isCurve: true
                minimum: -10
                maximum: 10
            },
            Parameter {
                name: qsTr('vignettingB')
                property: 'vignettingB'
                isSimple: true
                isCurve: true
                minimum: -10
                maximum: 10
            },
            Parameter {
                name: qsTr('vignettingC')
                property: 'vignettingC'
                isSimple: true
                isCurve: true
                minimum: -10
                maximum: 10
            },
            Parameter {
                name: qsTr('vignettingD')
                property: 'vignettingD'
                isSimple: true
                isCurve: true
                minimum: -10
                maximum: 10
            },
            Parameter {
                name: qsTr('vignettingRadius')
                property: 'vignettingRadius'
                isSimple: true
                isCurve: true
                minimum: 0
                maximum: 2
            },
            Parameter {
                name: qsTr('emorH1')
                property: 'emorH1'
                isSimple: true
                isCurve: true
                minimum: -10
                maximum: 10
            },
            Parameter {
                name: qsTr('emorH2')
                property: 'emorH2'
                isSimple: true
                isCurve: true
                minimum: -10
                maximum: 10
            },
            Parameter {
                name: qsTr('emorH3')
                property: 'emorH3'
                isSimple: true
                isCurve: true
                minimum: -10
                maximum: 10
            },
            Parameter {
                name: qsTr('emorH4')
                property: 'emorH4'
                isSimple: true
                isCurve: true
                minimum: -10
                maximum: 10
            },
            Parameter {
                name: qsTr('emorH5')
                property: 'emorH5'
                isSimple: true
                isCurve: true
                minimum: -10
                maximum: 10
            }
        ]
    }
}
