/* SPDX-License-Identifier: GPL-2.0-or-later */
#include "../shotcut_ui.qml"

import QtQuick 2.1
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.0
import Shotcut.Controls 1.0 as Shotcut


Shotcut.KeyframableFilter {
    width: 350
    height: 950 /* 38 rows of 25 pixels */

    keyframableParameters: [
        "yaw", "pitch", "roll", "frontX", "frontY", "frontUp", "backX", "backY", "backUp", "fov", "radius", "nadirRadius", "nadirCorrectionStart",
        "distortionA", "distortionB", "distortionC", "distortionRadius",
        "vignettingA", "vignettingB", "vignettingC", "vignettingD", "vignettingRadius",
        "emorH1", "emorH2", "emorH3", "emorH4", "emorH5"
        ]
    startValues:  [0.5, 0.5, 0.5, 0.5,  0.5,  0.5, 0.5,  0.5,   0.5,   0.5, 0.5,  0.5,    0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5]
    middleValues: [0.0, 0.0, 0.0, 0.75, 0.5, 90,   0.25, 0.5, 270,   180,   0.25, 0.2229, 0.8, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0]
    endValues:    [0.5, 0.5, 0.5, 0.5,  0.5,  0.5, 0.5,  0.5,   0.5,   0.5, 0.5,  0.5,    0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5]

    property var allParameters: [
        {
            name: "yaw",
            type: "simple",
            def: 0
        },
        {
            name: "pitch",
            type: "simple",
            def: 0
        },
        {
            name: "roll",
            type: "simple",
            def: 0
        },
        {
            name: "frontX",
            type: "simple",
            def: 0.75
        },
        {
            name: "frontY",
            type: "simple",
            def: 0.5
        },
        {
            name: "frontUp",
            type: "simple",
            def: 90
        },
        {
            name: "backX",
            type: "simple",
            def: 0.25
        },
        {
            name: "backY",
            type: "simple",
            def: 0.5
        },
        {
            name: "backUp",
            type: "simple",
            def: 270
        },
        {
            name: "fov",
            type: "simple",
            def: 180
        },
        {
            name: "radius",
            type: "simple",
            def: 0.25
        },
        {
            name: "nadirRadius",
            type: "simple",
            def: 0.2229
        },
        {
            name: "nadirCorrectionStart",
            type: "simple",
            def: 0.8
        },
        {
            name: "interpolation",
            type: "combobox",
            def: 1
        },
        {
            name: "projection",
            type: "combobox",
            def: 0
        },
        {
            name: "distortionA",
            type: "simple",
            def: 0.0
        },
        {
            name: "distortionB",
            type: "simple",
            def: 0.0
        },
        {
            name: "distortionC",
            type: "simple",
            def: 0.0
        },
        {
            name: "distortionRadius",
            type: "simple",
            def: 0.0
        },
        {
            name: "vignettingA",
            type: "simple",
            def: 0.0
        },
        {
            name: "vignettingB",
            type: "simple",
            def: 0.0
        },
        {
            name: "vignettingC",
            type: "simple",
            def: 0.0
        },
        {
            name: "vignettingD",
            type: "simple",
            def: 0.0
        },
        {
            name: "vignettingRadius",
            type: "simple",
            def: 0.0
        },
        {
            name: "emorH1",
            type: "simple",
            def: 0.0
        },
        {
            name: "emorH2",
            type: "simple",
            def: 0.0
        },
        {
            name: "emorH3",
            type: "simple",
            def: 0.0
        },
        {
            name: "emorH4",
            type: "simple",
            def: 0.0
        },
        {
            name: "emorH5",
            type: "simple",
            def: 0.0
        },
        {
            name: "emorEnabled",
            type: "checkbox",
            def: false
        }
    ]

    COMMON_FUNCTIONS

    Component.onCompleted: {
        defaultOnCompleted()
    }

    UPDATE_PROPERTY(yaw)
    UPDATE_PROPERTY(pitch)
    UPDATE_PROPERTY(roll)

    UPDATE_PROPERTY(frontX)
    UPDATE_PROPERTY(frontY)
    UPDATE_PROPERTY(frontUp)

    UPDATE_PROPERTY(backX)
    UPDATE_PROPERTY(backY)
    UPDATE_PROPERTY(backUp)

    UPDATE_PROPERTY(fov)
    UPDATE_PROPERTY(radius)
    UPDATE_PROPERTY(nadirRadius)
    UPDATE_PROPERTY(nadirCorrectionStart)

    UPDATE_PROPERTY_COMBOBOX(interpolation)
    UPDATE_PROPERTY_COMBOBOX(projection)

    UPDATE_PROPERTY(distortionA)
    UPDATE_PROPERTY(distortionB)
    UPDATE_PROPERTY(distortionC)
    UPDATE_PROPERTY(distortionRadius)

    UPDATE_PROPERTY(vignettingA)
    UPDATE_PROPERTY(vignettingB)
    UPDATE_PROPERTY(vignettingC)
    UPDATE_PROPERTY(vignettingD)
    UPDATE_PROPERTY(vignettingRadius)

    UPDATE_PROPERTY(emorH1)
    UPDATE_PROPERTY(emorH2)
    UPDATE_PROPERTY(emorH3)
    UPDATE_PROPERTY(emorH4)
    UPDATE_PROPERTY(emorH5)
    UPDATE_PROPERTY_CHECKBOX(emorEnabled)

    GridLayout {
        columns: 4
        anchors.fill: parent
        anchors.margins: 8

        Label {
            text: qsTr('Preset')
            Layout.alignment: Qt.AlignRight
        }
        Shotcut.Preset {
            id: preset
            parameters: ["yaw", "pitch", "roll", "frontX", "frontY", "frontUp", "backX", "backY", "backUp", "fov", "radius", "nadirRadius", "nadirCorrectionStart", "interpolation", "projection",
                "distortionA", "distortionB", "distortionC", "distortionRadius",
                "vignettingA", "vignettingB", "vignettingC", "vignettingD", "vignettingRadius",
                "emorH1", "emorH2", "emorH3", "emorH4", "emorH5", "emorEnabled"]
            Layout.columnSpan: 3
            onBeforePresetLoaded: {
                defaultBeforePresetLoaded()
            }
            onPresetSelected: {
                defaultPresetSelected()
            }
        }

        Label {
            text: qsTr('Interpolation')
            Layout.alignment: Qt.AlignRight
        }
        ComboBox {
            currentIndex: 0
            model: ["Nearest-neighbor", "Bilinear"]
            id: interpolationComboBox
            Layout.columnSpan: 2
            onCurrentIndexChanged: updateProperty_interpolation()
        }
        Shotcut.UndoButton {
            id: interpolationUndo
            onClicked: interpolationComboBox.currentIndex = 0
        }

        Label {
            text: qsTr('Alignment')
            Layout.alignment: Qt.AlignLeft
            Layout.columnSpan: 4
        }


        Label {
            text: qsTr('Yaw')
            Layout.alignment: Qt.AlignRight
        }
        Shotcut.SliderSpinner {
            id: yawSlider
            minimumValue: -360
            maximumValue: 360
            DEG_SLIDER_SPINNER_PROPERTIES
            onValueChanged: updateProperty_yaw()
        }
        KEYFRAME_BUTTON(yaw)
        Shotcut.UndoButton {
            id: yawUndo
            onClicked: yawSlider.value = 0
        }

        Label {
            text: qsTr('Pitch')
            Layout.alignment: Qt.AlignRight
        }
        Shotcut.SliderSpinner {
            id: pitchSlider
            minimumValue: -180
            maximumValue: 180
            DEG_SLIDER_SPINNER_PROPERTIES
            onValueChanged: updateProperty_pitch()
        }
        KEYFRAME_BUTTON(pitch)
        Shotcut.UndoButton {
            id: pitchUndo
            onClicked: pitchSlider.value = 0
        }

        Label {
            text: qsTr('Roll')
            Layout.alignment: Qt.AlignRight
        }
        Shotcut.SliderSpinner {
            id: rollSlider
            minimumValue: -180
            maximumValue: 180
            DEG_SLIDER_SPINNER_PROPERTIES
            onValueChanged: updateProperty_roll()
        }
        KEYFRAME_BUTTON(roll)
        Shotcut.UndoButton {
            id: rollUndo
            onClicked: rollSlider.value = 0
        }

        Label {
            text: qsTr('Lens')
            Layout.alignment: Qt.AlignLeft
            Layout.columnSpan: 4
        }

        Label {
            text: qsTr('Projection')
            Layout.alignment: Qt.AlignRight
        }
        ComboBox {
            currentIndex: 0
            model: ["Equidistant Fisheye"]
            id: projectionComboBox
            Layout.columnSpan: 2
            onCurrentIndexChanged: updateProperty_projection()
        }
        Shotcut.UndoButton {
            id: projectionUndo
            onClicked: projectionComboBox.currentIndex = 0
        }

        Label {
            text: qsTr('FOV')
            Layout.alignment: Qt.AlignRight
        }
        Shotcut.SliderSpinner {
            id: fovSlider
            minimumValue: 0
            maximumValue: 360
            spinnerWidth: 120
            suffix: ' deg'
            decimals: 4
            stepSize: 0.0001
            onValueChanged: updateProperty_fov()
        }
        KEYFRAME_BUTTON(fov)
        Shotcut.UndoButton {
            id: fovUndo
            onClicked: fovSlider.value = 180
        }

        Label {
            text: qsTr('Radius')
            Layout.alignment: Qt.AlignRight
        }
        Shotcut.SliderSpinner {
            id: radiusSlider
            minimumValue: 0
            maximumValue: 1
            suffix: ' '
            decimals: 4
            stepSize: 0.0001
            onValueChanged: updateProperty_radius()
        }
        KEYFRAME_BUTTON(radius)
        Shotcut.UndoButton {
            id: radiusUndo
            onClicked: radiusSlider.value = 0.25
        }

        /* front parameters */
        Label {
            text: qsTr('Front')
            Layout.alignment: Qt.AlignLeft
            Layout.columnSpan: 4
        }

        Label {
            text: qsTr('X')
            Layout.alignment: Qt.AlignRight
        }
        Shotcut.SliderSpinner {
            id: frontXSlider
            minimumValue: 0
            maximumValue: 1
            stepSize: 0.0001
            decimals: 4
            suffix: ' '
            onValueChanged: updateProperty_frontX()
        }
        KEYFRAME_BUTTON(frontX)
        Shotcut.UndoButton {
            id: frontXUndo
            onClicked: frontXSlider.value = 0.25
        }

        Label {
            text: qsTr('Y')
            Layout.alignment: Qt.AlignRight
        }
        Shotcut.SliderSpinner {
            id: frontYSlider
            minimumValue: 0
            maximumValue: 1
            stepSize: 0.0001
            decimals: 4
            suffix: ' '
            onValueChanged: updateProperty_frontY()
        }
        KEYFRAME_BUTTON(frontY)
        Shotcut.UndoButton {
            id: frontYUndo
            onClicked: frontYSlider.value = 0.25
        }

        Label {
            text: qsTr('Up')
            Layout.alignment: Qt.AlignRight
        }
        Shotcut.SliderSpinner {
            id: frontUpSlider
            minimumValue: 0
            maximumValue: 360
            DEG_SLIDER_SPINNER_PROPERTIES
            onValueChanged: updateProperty_frontUp()
        }
        KEYFRAME_BUTTON(frontUp)
        Shotcut.UndoButton {
            id: frontUpUndo
            onClicked: frontUpSlider.value = 90
        }

        /* back parameters */
        Label {
            text: qsTr('Back')
            Layout.alignment: Qt.AlignLeft
            Layout.columnSpan: 4
        }

        Label {
            text: qsTr('X')
            Layout.alignment: Qt.AlignRight
        }
        Shotcut.SliderSpinner {
            id: backXSlider
            minimumValue: 0
            maximumValue: 1
            stepSize: 0.0001
            decimals: 4
            suffix: ' '
            onValueChanged: updateProperty_backX()
        }
        KEYFRAME_BUTTON(backX)
        Shotcut.UndoButton {
            id: backXUndo
            onClicked: backXSlider.value = 0.25
        }

        Label {
            text: qsTr('Y')
            Layout.alignment: Qt.AlignRight
        }
        Shotcut.SliderSpinner {
            id: backYSlider
            minimumValue: 0
            maximumValue: 1
            stepSize: 0.0001
            decimals: 4
            suffix: ' '
            onValueChanged: updateProperty_backY()
        }
        KEYFRAME_BUTTON(backY)
        Shotcut.UndoButton {
            id: backYUndo
            onClicked: backYSlider.value = 0.25
        }

        Label {
            text: qsTr('Up')
            Layout.alignment: Qt.AlignRight
        }
        Shotcut.SliderSpinner {
            id: backUpSlider
            minimumValue: 0
            maximumValue: 360
            DEG_SLIDER_SPINNER_PROPERTIES
            onValueChanged: updateProperty_backUp()
        }
        KEYFRAME_BUTTON(backUp)
        Shotcut.UndoButton {
            id: backUpUndo
            onClicked: backUpSlider.value = 90
        }

        /* Nadir */
        Label {
            text: qsTr('Nadir')
            Layout.alignment: Qt.AlignLeft
            Layout.columnSpan: 4
        }


        Label {
            text: qsTr('Radius')
            Layout.alignment: Qt.AlignRight
        }
        Shotcut.SliderSpinner {
            id: nadirRadiusSlider
            minimumValue: 0
            maximumValue: 1
            suffix: ' '
            decimals: 4
            stepSize: 0.0001
            onValueChanged: updateProperty_nadirRadius()
        }
        KEYFRAME_BUTTON(nadirRadius)
        Shotcut.UndoButton {
            id: nadirRadiusUndo
            onClicked: nadirRadiusSlider.value = 0.2229
        }

        Label {
            text: qsTr('Start')
            Layout.alignment: Qt.AlignRight
        }
        Shotcut.SliderSpinner {
            id: nadirCorrectionStartSlider
            minimumValue: 0
            maximumValue: 1
            suffix: ' '
            decimals: 4
            stepSize: 0.0001
            onValueChanged: updateProperty_nadirCorrectionStart()
        }
        KEYFRAME_BUTTON(nadirCorrectionStart)
        Shotcut.UndoButton {
            id: nadirCorrectionStartUndo
            onClicked: nadirCorrectionStartSlider.value = 0.8
        }

        /* Lens Distortion */
        Label {
            text: qsTr('Lens Distortion')
            Layout.alignment: Qt.AlignLeft
            Layout.columnSpan: 4
        }

        Label {
            text: qsTr('A')
            Layout.alignment: Qt.AlignRight
        }
        Shotcut.SliderSpinner {
            id: distortionASlider
            minimumValue: -2
            maximumValue: 2
            suffix: ' '
            decimals: 4
            stepSize: 0.0001
            onValueChanged: updateProperty_distortionA()
        }
        KEYFRAME_BUTTON(distortionA)
        Shotcut.UndoButton {
            id: distortionAUndo
            onClicked: distortionASlider.value = 0.0
        }

        Label {
            text: qsTr('B')
            Layout.alignment: Qt.AlignRight
        }
        Shotcut.SliderSpinner {
            id: distortionBSlider
            minimumValue: -2
            maximumValue: 2
            suffix: ' '
            decimals: 4
            stepSize: 0.0001
            onValueChanged: updateProperty_distortionB()
        }
        KEYFRAME_BUTTON(distortionB)
        Shotcut.UndoButton {
            id: distortionBUndo
            onClicked: distortionBSlider.value = 0.0
        }

        Label {
            text: qsTr('C')
            Layout.alignment: Qt.AlignRight
        }
        Shotcut.SliderSpinner {
            id: distortionCSlider
            minimumValue: -2
            maximumValue: 2
            suffix: ' '
            decimals: 4
            stepSize: 0.0001
            onValueChanged: updateProperty_distortionC()
        }
        KEYFRAME_BUTTON(distortionC)
        Shotcut.UndoButton {
            id: distortionCUndo
            onClicked: distortionCSlider.value = 0.0
        }

        Label {
            text: qsTr('Radius')
            Layout.alignment: Qt.AlignRight
        }
        Shotcut.SliderSpinner {
            id: distortionRadiusSlider
            minimumValue: 0
            maximumValue: 1
            suffix: ' '
            decimals: 4
            stepSize: 0.0001
            onValueChanged: updateProperty_distortionRadius()
        }
        KEYFRAME_BUTTON(distortionRadius)
        Shotcut.UndoButton {
            id: distortionRadiusUndo
            onClicked: distortionRadiusSlider.value = 0.0
        }

        /* Lens Vignetting */
        Label {
            text: qsTr('Lens Vignetting')
            Layout.alignment: Qt.AlignLeft
            Layout.columnSpan: 4
        }

        Label {
            text: qsTr('A')
            Layout.alignment: Qt.AlignRight
        }
        Shotcut.SliderSpinner {
            id: vignettingASlider
            minimumValue: -10
            maximumValue: 10
            suffix: ' '
            decimals: 4
            stepSize: 0.0001
            onValueChanged: updateProperty_vignettingA()
        }
        KEYFRAME_BUTTON(vignettingA)
        Shotcut.UndoButton {
            id: vignettingAUndo
            onClicked: vignettingASlider.value = 0.0
        }

        Label {
            text: qsTr('B')
            Layout.alignment: Qt.AlignRight
        }
        Shotcut.SliderSpinner {
            id: vignettingBSlider
            minimumValue: -10
            maximumValue: 10
            suffix: ' '
            decimals: 4
            stepSize: 0.0001
            onValueChanged: updateProperty_vignettingB()
        }
        KEYFRAME_BUTTON(vignettingB)
        Shotcut.UndoButton {
            id: vignettingBUndo
            onClicked: vignettingBSlider.value = 0.0
        }

        Label {
            text: qsTr('C')
            Layout.alignment: Qt.AlignRight
        }
        Shotcut.SliderSpinner {
            id: vignettingCSlider
            minimumValue: -10
            maximumValue: 10
            suffix: ' '
            decimals: 4
            stepSize: 0.0001
            onValueChanged: updateProperty_vignettingC()
        }
        KEYFRAME_BUTTON(vignettingC)
        Shotcut.UndoButton {
            id: vignettingCUndo
            onClicked: vignettingCSlider.value = 0.0
        }

        Label {
            text: qsTr('D')
            Layout.alignment: Qt.AlignRight
        }
        Shotcut.SliderSpinner {
            id: vignettingDSlider
            minimumValue: -10
            maximumValue: 10
            suffix: ' '
            decimals: 4
            stepSize: 0.0001
            onValueChanged: updateProperty_vignettingD()
        }
        KEYFRAME_BUTTON(vignettingD)
        Shotcut.UndoButton {
            id: vignettingDUndo
            onClicked: vignettingDSlider.value = 0.0
        }

        Label {
            text: qsTr('Radius')
            Layout.alignment: Qt.AlignRight
        }
        Shotcut.SliderSpinner {
            id: vignettingRadiusSlider
            minimumValue: 0
            maximumValue: 2
            suffix: ' '
            decimals: 4
            stepSize: 0.0001
            onValueChanged: updateProperty_vignettingRadius()
        }
        KEYFRAME_BUTTON(vignettingRadius)
        Shotcut.UndoButton {
            id: vignettingRadiusUndo
            onClicked: vignettingRadiusSlider.value = 0.0
        }

        /* Sensor Response */
        Label {
            text: qsTr('Sensor Response')
            Layout.alignment: Qt.AlignLeft
            Layout.columnSpan: 4
        }

        Label {
            text: qsTr('Enabled')
            Layout.alignment: Qt.AlignLeft
        }
        CheckBox {
            text: qsTr('')
            checked: false
            partiallyCheckedEnabled: false
            id: emorEnabledCheckBox
            Layout.columnSpan: 3
            onCheckedChanged: updateProperty_emorEnabled()
        }

        Label {
            text: qsTr('EMoR h(1)')
            Layout.alignment: Qt.AlignRight
        }
        Shotcut.SliderSpinner {
            id: emorH1Slider
            minimumValue: -10
            maximumValue: 10
            suffix: ' '
            decimals: 4
            stepSize: 0.0001
            onValueChanged: updateProperty_emorH1()
        }
        KEYFRAME_BUTTON(emorH1)
        Shotcut.UndoButton {
            id: emorH1Undo
            onClicked: emorH1Slider.value = 0.0
        }

        Label {
            text: qsTr('EMoR h(2)')
            Layout.alignment: Qt.AlignRight
        }
        Shotcut.SliderSpinner {
            id: emorH2Slider
            minimumValue: -10
            maximumValue: 10
            suffix: ' '
            decimals: 4
            stepSize: 0.0001
            onValueChanged: updateProperty_emorH2()
        }
        KEYFRAME_BUTTON(emorH2)
        Shotcut.UndoButton {
            id: emorH2Undo
            onClicked: emorH2Slider.value = 0.0
        }

        Label {
            text: qsTr('EMoR h(3)')
            Layout.alignment: Qt.AlignRight
        }
        Shotcut.SliderSpinner {
            id: emorH3Slider
            minimumValue: -10
            maximumValue: 10
            suffix: ' '
            decimals: 4
            stepSize: 0.0001
            onValueChanged: updateProperty_emorH3()
        }
        KEYFRAME_BUTTON(emorH3)
        Shotcut.UndoButton {
            id: emorH3Undo
            onClicked: emorH3Slider.value = 0.0
        }

        Label {
            text: qsTr('EMoR h(4)')
            Layout.alignment: Qt.AlignRight
        }
        Shotcut.SliderSpinner {
            id: emorH4Slider
            minimumValue: -10
            maximumValue: 10
            suffix: ' '
            decimals: 4
            stepSize: 0.0001
            onValueChanged: updateProperty_emorH4()
        }
        KEYFRAME_BUTTON(emorH4)
        Shotcut.UndoButton {
            id: emorH4Undo
            onClicked: emorH4Slider.value = 0.0
        }

        Label {
            text: qsTr('EMoR h(5)')
            Layout.alignment: Qt.AlignRight
        }
        Shotcut.SliderSpinner {
            id: emorH5Slider
            minimumValue: -10
            maximumValue: 10
            suffix: ' '
            decimals: 4
            stepSize: 0.0001
            onValueChanged: updateProperty_emorH5()
        }
        KEYFRAME_BUTTON(emorH5)
        Shotcut.UndoButton {
            id: emorH5Undo
            onClicked: emorH5Slider.value = 0.0
        }
    }

    COMMON_CONNECTIONS
}