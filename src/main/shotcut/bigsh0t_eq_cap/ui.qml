/* SPDX-License-Identifier: GPL-2.0-or-later */
#include "../shotcut_ui.qml"

import QtQuick 2.1
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.0
import QtQuick.Dialogs 1.1
import Shotcut.Controls 1.0 as Shotcut


Shotcut.KeyframableFilter {
    width: 350
    height: 650 /* 26 rows of 25 pixels */

    keyframableParameters: [
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
    startValues: [
        0.5,
        0.5,
        0.5,
        0.5,
        0.5,
        0.5,
        0.5,
        0.5,
        0.5,
        0.5,
        0.5,
        0.5,
        0.5,
        0.5,
        0.5,
        0.5,
        0.5,
        0.5,
    ]
    middleValues: [
        45,
        80,
        0,
        10,
        10,
        0,
        360,
        0,
        2,

        45,
        80,
        0,
        10,
        10,
        0,
        360,
        0,
        2
    ]
    endValues: [
        0.5,
        0.5,
        0.5,
        0.5,
        0.5,
        0.5,
        0.5,
        0.5,
        0.5,
        0.5,
        0.5,
        0.5,
        0.5,
        0.5,
        0.5,
        0.5,
        0.5,
        0.5,
    ]

    property var allParameters: [
        {
            name: "interpolation",
            type: "combobox",
            def: 1
        },
        {
            name: "topStart",
            type: "simple",
            def: 45
        },
        {
            name: "topEnd",
            type: "simple",
            def: 80
        },
        {
            name: "topFadeIn",
            type: "simple",
            def: 10
        },
        {
            name: "topBlendIn",
            type: "simple",
            def: 0
        },
        {
            name: "topBlendOut",
            type: "simple",
            def: 10
        },
        {
            name: "topBlurWidthStart",
            type: "simple",
            def: 0
        },
        {
            name: "topBlurHeightStart",
            type: "simple",
            def: 0
        },
        {
            name: "topBlurWidthEnd",
            type: "simple",
            def: 360
        },
        {
            name: "topBlurHeightEnd",
            type: "simple",
            def: 2
        },

        {
            name: "bottomStart",
            type: "simple",
            def: 45
        },
        {
            name: "bottomEnd",
            type: "simple",
            def: 80
        },
        {
            name: "bottomFadeIn",
            type: "simple",
            def: 10
        },
        {
            name: "bottomBlendIn",
            type: "simple",
            def: 0
        },
        {
            name: "bottomBlendOut",
            type: "simple",
            def: 10
        },
        {
            name: "bottomBlurWidthStart",
            type: "simple",
            def: 0
        },
        {
            name: "bottomBlurHeightStart",
            type: "simple",
            def: 0
        },
        {
            name: "bottomBlurWidthEnd",
            type: "simple",
            def: 360
        },
        {
            name: "bottomBlurHeightEnd",
            type: "simple",
            def: 2
        }
    ]

    COMMON_FUNCTIONS

    Component.onCompleted: {
        defaultOnCompleted()
    }

    UPDATE_PROPERTY_COMBOBOX(interpolation)
    UPDATE_PROPERTY_CHECKBOX(topEnabled)
    UPDATE_PROPERTY(topStart)
    UPDATE_PROPERTY(topEnd)
    UPDATE_PROPERTY(topFadeIn)
    UPDATE_PROPERTY(topBlendIn)
    UPDATE_PROPERTY(topBlendOut)
    UPDATE_PROPERTY(topBlurWidthStart)
    UPDATE_PROPERTY(topBlurHeightStart)
    UPDATE_PROPERTY(topBlurWidthEnd)
    UPDATE_PROPERTY(topBlurHeightEnd)

    UPDATE_PROPERTY_CHECKBOX(bottomEnabled)
    UPDATE_PROPERTY(bottomStart)
    UPDATE_PROPERTY(bottomEnd)
    UPDATE_PROPERTY(bottomFadeIn)
    UPDATE_PROPERTY(bottomBlendIn)
    UPDATE_PROPERTY(bottomBlendOut)
    UPDATE_PROPERTY(bottomBlurWidthStart)
    UPDATE_PROPERTY(bottomBlurHeightStart)
    UPDATE_PROPERTY(bottomBlurWidthEnd)
    UPDATE_PROPERTY(bottomBlurHeightEnd)

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
            parameters: [
                "interpolation",

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
            onClicked: interpolationComboBox.currentIndex = 1
        }

        /* -------------------------------------------- */

        Label {
            text: qsTr('Top')
            Layout.alignment: Qt.AlignLeft
        }
        CheckBox {
            text: qsTr('')
            checked: true
            partiallyCheckedEnabled: false
            id: topEnabledCheckBox
            Layout.columnSpan: 3
            onCheckedChanged: updateProperty_topEnabled()
        }

        Label {
            text: qsTr('Start')
            Layout.alignment: Qt.AlignRight
        }
        Shotcut.SliderSpinner {
            id: topStartSlider
            minimumValue: 0
            maximumValue: 90
            DEG_SLIDER_SPINNER_PROPERTIES
            onValueChanged: updateProperty_topStart()
        }
        KEYFRAME_BUTTON(topStart)
        Shotcut.UndoButton {
            id: topStartUndo
            onClicked: topStartSlider.value = 45
        }

        Label {
            text: qsTr('End')
            Layout.alignment: Qt.AlignRight
        }
        Shotcut.SliderSpinner {
            id: topEndSlider
            minimumValue: 0
            maximumValue: 90
            DEG_SLIDER_SPINNER_PROPERTIES
            onValueChanged: updateProperty_topEnd()
        }
        KEYFRAME_BUTTON(topEnd)
        Shotcut.UndoButton {
            id: topEndUndo
            onClicked: topEndSlider.value = 80
        }

        Label {
            text: qsTr('Fade')
            Layout.alignment: Qt.AlignRight
        }
        Shotcut.SliderSpinner {
            id: topFadeInSlider
            minimumValue: 0
            maximumValue: 90
            DEG_SLIDER_SPINNER_PROPERTIES
            onValueChanged: updateProperty_topFadeIn()
        }
        KEYFRAME_BUTTON(topFadeIn)
        Shotcut.UndoButton {
            id: topFadeInUndo
            onClicked: topFadeInSlider.value = 10
        }

        Label {
            text: qsTr('Blend')
            Layout.alignment: Qt.AlignRight
        }
        Label {
            text: qsTr('')
            Layout.alignment: Qt.AlignRight
            Layout.columnSpan: 3
        }

        Label {
            text: qsTr('In')
            Layout.alignment: Qt.AlignRight
        }
        Shotcut.SliderSpinner {
            id: topBlendInSlider
            minimumValue: 0
            maximumValue: 90
            DEG_SLIDER_SPINNER_PROPERTIES
            onValueChanged: updateProperty_topBlendIn()
        }
        KEYFRAME_BUTTON(topBlendIn)
        Shotcut.UndoButton {
            id: topBlendInUndo
            onClicked: topBlendInSlider.value = 10
        }

        Label {
            text: qsTr('Out')
            Layout.alignment: Qt.AlignRight
        }
        Shotcut.SliderSpinner {
            id: topBlendOutSlider
            minimumValue: 0
            maximumValue: 90
            DEG_SLIDER_SPINNER_PROPERTIES
            onValueChanged: updateProperty_topBlendOut()
        }
        KEYFRAME_BUTTON(topBlendOut)
        Shotcut.UndoButton {
            id: topBlendOutUndo
            onClicked: topBlendOutSlider.value = 10
        }

        Label {
            text: qsTr('Blur')
            Layout.alignment: Qt.AlignRight
        }
        Label {
            text: qsTr('')
            Layout.alignment: Qt.AlignRight
            Layout.columnSpan: 3
        }

        Label {
            text: qsTr('Width at start')
            Layout.alignment: Qt.AlignRight
        }
        Shotcut.SliderSpinner {
            id: topBlurWidthStartSlider
            minimumValue: 0
            maximumValue: 360
            DEG_SLIDER_SPINNER_PROPERTIES
            onValueChanged: updateProperty_topBlurWidthStart()
        }
        KEYFRAME_BUTTON(topBlurWidthStart)
        Shotcut.UndoButton {
            id: topBlurWidthStartUndo
            onClicked: topBlurWidthStartSlider.value = 0
        }

        Label {
            text: qsTr('Height at start')
            Layout.alignment: Qt.AlignRight
        }
        Shotcut.SliderSpinner {
            id: topBlurHeightStartSlider
            minimumValue: 0
            maximumValue: 90
            DEG_SLIDER_SPINNER_PROPERTIES
            onValueChanged: updateProperty_topBlurHeightStart()
        }
        KEYFRAME_BUTTON(topBlurHeightStart)
        Shotcut.UndoButton {
            id: topBlurHeightStartUndo
            onClicked: topBlurHeightStartSlider.value = 0
        }

        Label {
            text: qsTr('Width at end')
            Layout.alignment: Qt.AlignRight
        }
        Shotcut.SliderSpinner {
            id: topBlurWidthEndSlider
            minimumValue: 0
            maximumValue: 360
            DEG_SLIDER_SPINNER_PROPERTIES
            onValueChanged: updateProperty_topBlurWidthEnd()
        }
        KEYFRAME_BUTTON(topBlurWidthEnd)
        Shotcut.UndoButton {
            id: topBlurWidthEndUndo
            onClicked: topBlurWidthEndSlider.value = 360
        }

        Label {
            text: qsTr('Height at end')
            Layout.alignment: Qt.AlignRight
        }
        Shotcut.SliderSpinner {
            id: topBlurHeightEndSlider
            minimumValue: 0
            maximumValue: 90
            DEG_SLIDER_SPINNER_PROPERTIES
            onValueChanged: updateProperty_topBlurHeightEnd()
        }
        KEYFRAME_BUTTON(topBlurHeightEnd)
        Shotcut.UndoButton {
            id: topBlurHeightEndUndo
            onClicked: topBlurHeightEndSlider.value = 2
        }




        Label {
            text: qsTr('Bottom')
            Layout.alignment: Qt.AlignLeft
        }
        CheckBox {
            text: qsTr('')
            checked: true
            partiallyCheckedEnabled: false
            id: bottomEnabledCheckBox
            Layout.columnSpan: 3
            onCheckedChanged: updateProperty_bottomEnabled()
        }

        Label {
            text: qsTr('Start')
            Layout.alignment: Qt.AlignRight
        }
        Shotcut.SliderSpinner {
            id: bottomStartSlider
            minimumValue: 0
            maximumValue: 90
            DEG_SLIDER_SPINNER_PROPERTIES
            onValueChanged: updateProperty_bottomStart()
        }
        KEYFRAME_BUTTON(bottomStart)
        Shotcut.UndoButton {
            id: bottomStartUndo
            onClicked: bottomStartSlider.value = 45
        }

        Label {
            text: qsTr('End')
            Layout.alignment: Qt.AlignRight
        }
        Shotcut.SliderSpinner {
            id: bottomEndSlider
            minimumValue: 0
            maximumValue: 90
            DEG_SLIDER_SPINNER_PROPERTIES
            onValueChanged: updateProperty_bottomEnd()
        }
        KEYFRAME_BUTTON(bottomEnd)
        Shotcut.UndoButton {
            id: bottomEndUndo
            onClicked: bottomEndSlider.value = 80
        }

        Label {
            text: qsTr('Fade')
            Layout.alignment: Qt.AlignRight
        }
        Shotcut.SliderSpinner {
            id: bottomFadeInSlider
            minimumValue: 0
            maximumValue: 90
            DEG_SLIDER_SPINNER_PROPERTIES
            onValueChanged: updateProperty_bottomFadeIn()
        }
        KEYFRAME_BUTTON(bottomFadeIn)
        Shotcut.UndoButton {
            id: bottomFadeInUndo
            onClicked: bottomFadeInSlider.value = 10
        }

        Label {
            text: qsTr('Blend')
            Layout.alignment: Qt.AlignRight
        }
        Label {
            text: qsTr('')
            Layout.alignment: Qt.AlignRight
            Layout.columnSpan: 3
        }

        Label {
            text: qsTr('In')
            Layout.alignment: Qt.AlignRight
        }
        Shotcut.SliderSpinner {
            id: bottomBlendInSlider
            minimumValue: 0
            maximumValue: 90
            DEG_SLIDER_SPINNER_PROPERTIES
            onValueChanged: updateProperty_bottomBlendIn()
        }
        KEYFRAME_BUTTON(bottomBlendIn)
        Shotcut.UndoButton {
            id: bottomBlendInUndo
            onClicked: bottomBlendInSlider.value = 10
        }

        Label {
            text: qsTr('Out')
            Layout.alignment: Qt.AlignRight
        }
        Shotcut.SliderSpinner {
            id: bottomBlendOutSlider
            minimumValue: 0
            maximumValue: 90
            DEG_SLIDER_SPINNER_PROPERTIES
            onValueChanged: updateProperty_bottomBlendOut()
        }
        KEYFRAME_BUTTON(bottomBlendOut)
        Shotcut.UndoButton {
            id: bottomBlendOutUndo
            onClicked: bottomBlendOutSlider.value = 10
        }

        Label {
            text: qsTr('Blur')
            Layout.alignment: Qt.AlignRight
        }
        Label {
            text: qsTr('')
            Layout.alignment: Qt.AlignRight
            Layout.columnSpan: 3
        }

        Label {
            text: qsTr('Width at start')
            Layout.alignment: Qt.AlignRight
        }
        Shotcut.SliderSpinner {
            id: bottomBlurWidthStartSlider
            minimumValue: 0
            maximumValue: 360
            DEG_SLIDER_SPINNER_PROPERTIES
            onValueChanged: updateProperty_bottomBlurWidthStart()
        }
        KEYFRAME_BUTTON(bottomBlurWidthStart)
        Shotcut.UndoButton {
            id: bottomBlurWidthStartUndo
            onClicked: bottomBlurWidthStartSlider.value = 0
        }

        Label {
            text: qsTr('Height at start')
            Layout.alignment: Qt.AlignRight
        }
        Shotcut.SliderSpinner {
            id: bottomBlurHeightStartSlider
            minimumValue: 0
            maximumValue: 90
            DEG_SLIDER_SPINNER_PROPERTIES
            onValueChanged: updateProperty_bottomBlurHeightStart()
        }
        KEYFRAME_BUTTON(bottomBlurHeightStart)
        Shotcut.UndoButton {
            id: bottomBlurHeightStartUndo
            onClicked: bottomBlurHeightStartSlider.value = 0
        }

        Label {
            text: qsTr('Width at end')
            Layout.alignment: Qt.AlignRight
        }
        Shotcut.SliderSpinner {
            id: bottomBlurWidthEndSlider
            minimumValue: 0
            maximumValue: 360
            DEG_SLIDER_SPINNER_PROPERTIES
            onValueChanged: updateProperty_bottomBlurWidthEnd()
        }
        KEYFRAME_BUTTON(bottomBlurWidthEnd)
        Shotcut.UndoButton {
            id: bottomBlurWidthEndUndo
            onClicked: bottomBlurWidthEndSlider.value = 360
        }

        Label {
            text: qsTr('Height at end')
            Layout.alignment: Qt.AlignRight
        }
        Shotcut.SliderSpinner {
            id: bottomBlurHeightEndSlider
            minimumValue: 0
            maximumValue: 90
            DEG_SLIDER_SPINNER_PROPERTIES
            onValueChanged: updateProperty_bottomBlurHeightEnd()
        }
        KEYFRAME_BUTTON(bottomBlurHeightEnd)
        Shotcut.UndoButton {
            id: bottomBlurHeightEndUndo
            onClicked: bottomBlurHeightEndSlider.value = 2
        }

    }

    COMMON_CONNECTIONS
}