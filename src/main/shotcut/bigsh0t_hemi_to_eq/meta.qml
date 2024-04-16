// SPDX-License-Identifier: GPL-2.0-or-later
import QtQuick
import org.shotcut.qml

Metadata {
    type: Metadata.Filter
    name: qsTr("360: Hemispherical to Equirectangular")
    mlt_service: "frei0r.bigsh0t_hemi_to_eq"
    keywords: qsTr('spherical projection dual fisheye', 'search keywords for the 360: Hemispherical to Equirectangular video filter') + ' 360: hemispherical to equirectangular bigsh0t'
    objectName: "bigsh0t_hemi_to_eq"
    qml: "ui.qml"
    icon: "icon.webp"

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
                isCurve: true
                minimum: -360
                maximum: 360
            },
            Parameter {
                name: qsTr('Pitch', 'rotation around the side-to-side axis (roll, pitch, yaw)')
                property: 'pitch'
                isCurve: true
                minimum: -180
                maximum: 180
            },
            Parameter {
                name: qsTr('Roll')
                property: 'roll'
                isCurve: true
                minimum: -180
                maximum: 180
            },
            Parameter {
                name: qsTr('FOV', 'field of view')
                property: 'fov'
                isCurve: true
                minimum: 0
                maximum: 360
            },
            Parameter {
                name: qsTr('Radius')
                property: 'radius'
                isCurve: true
                minimum: 0
                maximum: 1
            },
            Parameter {
                name: qsTr('Front X')
                property: 'frontX'
                isCurve: true
                minimum: 0
                maximum: 1
            },
            Parameter {
                name: qsTr('Front Y')
                property: 'frontY'
                isCurve: true
                minimum: 0
                maximum: 1
            },
            Parameter {
                name: qsTr('Front Up')
                property: 'frontUp'
                isCurve: true
                minimum: -360
                maximum: 360
            },
            Parameter {
                name: qsTr('Back X')
                property: 'backX'
                isCurve: true
                minimum: 0
                maximum: 1
            },
            Parameter {
                name: qsTr('Back Y')
                property: 'backY'
                isCurve: true
                minimum: 0
                maximum: 1
            },
            Parameter {
                name: qsTr('Back Up')
                property: 'backUp'
                isCurve: true
                minimum: -360
                maximum: 360
            },
            Parameter {
                name: qsTr('Nadir Radius')
                property: 'nadirRadius'
                isCurve: true
                minimum: 0
                maximum: 1
            },
            Parameter {
                name: qsTr('Nadir Start')
                property: 'nadirCorrectionStart'
                isCurve: true
                minimum: 0
                maximum: 1
            },
            Parameter {
                name: qsTr('distortionA')
                property: 'distortionA'
                isCurve: true
                minimum: -2
                maximum: 2
            },
            Parameter {
                name: qsTr('distortionB')
                property: 'distortionB'
                isCurve: true
                minimum: -2
                maximum: 2
            },
            Parameter {
                name: qsTr('distortionC')
                property: 'distortionC'
                isCurve: true
                minimum: -2
                maximum: 2
            },
            Parameter {
                name: qsTr('distortionRadius')
                property: 'distortionRadius'
                isCurve: true
                minimum: 0
                maximum: 1
            },
            Parameter {
                name: qsTr('vignettingA')
                property: 'vignettingA'
                isCurve: true
                minimum: -10
                maximum: 10
            },
            Parameter {
                name: qsTr('vignettingB')
                property: 'vignettingB'
                isCurve: true
                minimum: -10
                maximum: 10
            },
            Parameter {
                name: qsTr('vignettingC')
                property: 'vignettingC'
                isCurve: true
                minimum: -10
                maximum: 10
            },
            Parameter {
                name: qsTr('vignettingD')
                property: 'vignettingD'
                isCurve: true
                minimum: -10
                maximum: 10
            },
            Parameter {
                name: qsTr('vignettingRadius')
                property: 'vignettingRadius'
                isCurve: true
                minimum: 0
                maximum: 2
            },
            Parameter {
                name: qsTr('emorH1')
                property: 'emorH1'
                isCurve: true
                minimum: -10
                maximum: 10
            },
            Parameter {
                name: qsTr('emorH2')
                property: 'emorH2'
                isCurve: true
                minimum: -10
                maximum: 10
            },
            Parameter {
                name: qsTr('emorH3')
                property: 'emorH3'
                isCurve: true
                minimum: -10
                maximum: 10
            },
            Parameter {
                name: qsTr('emorH4')
                property: 'emorH4'
                isCurve: true
                minimum: -10
                maximum: 10
            },
            Parameter {
                name: qsTr('emorH5')
                property: 'emorH5'
                isCurve: true
                minimum: -10
                maximum: 10
            }
        ]
    }
}
