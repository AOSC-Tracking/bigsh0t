/* SPDX-License-Identifier: GPL-2.0-or-later */
#include "../shotcut_ui.qml"

import QtQuick 2.1
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.0
import Shotcut.Controls 1.0 as Shotcut


Shotcut.KeyframableFilter {
    width: 350
    height: 125 /* 5 rows of 25 pixels */

    keyframableParameters: ["yaw", "pitch", "roll"]
    startValues: [0.5, 0.5, 0.5]
    middleValues: [0.0, 0.0, 0.0]
    endValues: [0.5, 0.5, 0.5]

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
            name: "interpolation",
            type: "combobox",
            def: 1
        },
        {
            name: "grid",
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
    UPDATE_PROPERTY_COMBOBOX(interpolation)
    UPDATE_PROPERTY_CHECKBOX(grid)

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
            parameters: ["yaw", "pitch", "roll", "interpolation"]
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
            text: qsTr('Grid')
            Layout.alignment: Qt.AlignRight
        }
        CheckBox {
            text: qsTr('Show')
            checked: false
            partiallyCheckedEnabled: false
            id: gridCheckBox
            Layout.columnSpan: 3
            onCheckedChanged: updateProperty_grid()
        }

    }

    COMMON_CONNECTIONS
}