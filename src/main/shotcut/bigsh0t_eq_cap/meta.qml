/* SPDX-License-Identifier: GPL-2.0-or-later */
import QtQuick 2.0
import org.shotcut.qml 1.0

Metadata {
    type: Metadata.Filter
    name: qsTr("360: Cap top and bottom")
    mlt_service: "frei0r.bigsh0t_eq_cap"
    objectName: "bigsh0t_eq_cap"
    qml: "ui.qml"
    keyframes {
        allowAnimateIn: true
        allowAnimateOut: true
        simpleProperties: [
            "topStart",
            "topEnd",
            "topBlendIn",
            "topBlendOut",
            "topFadeIn",
            "topBlurWidthStart",
            "topBlurWidthEnd",
            "topBlurHeightStart",
            "topBlurHeightEnd",

            "bottomStart",
            "bottomEnd",
            "bottomBlendIn",
            "bottomBlendOut",
            "bottomFadeIn",
            "bottomBlurWidthStart",
            "bottomBlurWidthEnd",
            "bottomBlurHeightStart",
            "bottomBlurHeightEnd"
        ]
        parameters: [
            Parameter {
                name: qsTr('topStart')
                property: 'topStart'
                isSimple: true
                isCurve: true
                minimum: 0
                maximum: 90
            },
            Parameter {
                name: qsTr('topEnd')
                property: 'topEnd'
                isSimple: true
                isCurve: true
                minimum: 0
                maximum: 90
            },
            Parameter {
                name: qsTr('topBlendIn')
                property: 'topBlendIn'
                isSimple: true
                isCurve: true
                minimum: 0
                maximum: 90
            },
            Parameter {
                name: qsTr('topBlendOut')
                property: 'topBlendOut'
                isSimple: true
                isCurve: true
                minimum: 0
                maximum: 90
            },
            Parameter {
                name: qsTr('topFadeIn')
                property: 'topFadeIn'
                isSimple: true
                isCurve: true
                minimum: 0
                maximum: 90
            },
            Parameter {
                name: qsTr('topBlurWidthStart')
                property: 'topBlurWidthStart'
                isSimple: true
                isCurve: true
                minimum: 0
                maximum: 360
            },
            Parameter {
                name: qsTr('topBlurWidthEnd')
                property: 'topBlurWidthEnd'
                isSimple: true
                isCurve: true
                minimum: 0
                maximum: 360
            },
            Parameter {
                name: qsTr('topBlurHeightStart')
                property: 'topBlurHeightStart'
                isSimple: true
                isCurve: true
                minimum: 0
                maximum: 90
            },
            Parameter {
                name: qsTr('topBlurHeightEnd')
                property: 'topBlurHeightEnd'
                isSimple: true
                isCurve: true
                minimum: 0
                maximum: 90
            },

            Parameter {
                name: qsTr('bottomStart')
                property: 'bottomStart'
                isSimple: true
                isCurve: true
                minimum: 0
                maximum: 90
            },
            Parameter {
                name: qsTr('bottomEnd')
                property: 'bottomEnd'
                isSimple: true
                isCurve: true
                minimum: 0
                maximum: 90
            },
            Parameter {
                name: qsTr('bottomBlendIn')
                property: 'bottomBlendIn'
                isSimple: true
                isCurve: true
                minimum: 0
                maximum: 90
            },
            Parameter {
                name: qsTr('bottomBlendOut')
                property: 'bottomBlendOut'
                isSimple: true
                isCurve: true
                minimum: 0
                maximum: 90
            },
            Parameter {
                name: qsTr('bottomFadeIn')
                property: 'bottomFadeIn'
                isSimple: true
                isCurve: true
                minimum: 0
                maximum: 90
            },
            Parameter {
                name: qsTr('bottomBlurWidthStart')
                property: 'bottomBlurWidthStart'
                isSimple: true
                isCurve: true
                minimum: 0
                maximum: 360
            },
            Parameter {
                name: qsTr('bottomBlurWidthEnd')
                property: 'bottomBlurWidthEnd'
                isSimple: true
                isCurve: true
                minimum: 0
                maximum: 360
            },
            Parameter {
                name: qsTr('bottomBlurHeightStart')
                property: 'bottomBlurHeightStart'
                isSimple: true
                isCurve: true
                minimum: 0
                maximum: 90
            },
            Parameter {
                name: qsTr('bottomBlurHeightEnd')
                property: 'bottomBlurHeightEnd'
                isSimple: true
                isCurve: true
                minimum: 0
                maximum: 90
            }
        ]
    }
}
