/* SPDX-License-Identifier: GPL-2.0-or-later */
#include "../shotcut_ui.qml"

import QtQuick 2.1
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.0
import Shotcut.Controls 1.0 as Shotcut


Shotcut.KeyframableFilter {
    width: 350
    height: 175 /* 7 rows of 25 pixels */

    keyframableParameters: ["hfov0", "hfov1", "vfov0", "vfov1"]
    startValues: [0.5, 0.5, 0.5, 0.5]
    middleValues: [180.0, 200.0, 140.0, 160.0]
    endValues: [0.5, 0.5, 0.5, 0.5]

    property var allParameters: [
        {
            name: "hfov0",
            type: "simple",
            def: 180
        },
        {
            name: "hfov1",
            type: "simple",
            def: 200
        },
        {
            name: "vfov0",
            type: "simple",
            def: 140
        },
        {
            name: "vfov1",
            type: "simple",
            def: 160
        }
    ]

    COMMON_FUNCTIONS

    Component.onCompleted: {
        defaultOnCompleted()
    }

    UPDATE_PROPERTY(hfov0)
    UPDATE_PROPERTY(hfov1)
    UPDATE_PROPERTY(vfov0)
    UPDATE_PROPERTY(vfov1)

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
            parameters: ["hfov0", "hfov1", "vfov0", "vfov1"]
            Layout.columnSpan: 3
            onBeforePresetLoaded: {
                defaultBeforePresetLoaded()
            }
            onPresetSelected: {
                defaultPresetSelected()
            }
        }

        Label {
            text: qsTr('Horizontal')
            Layout.alignment: Qt.AlignLeft
            Layout.columnSpan: 4
        }

        Label {
            text: qsTr('Start')
            Layout.alignment: Qt.AlignRight
        }
        Shotcut.SliderSpinner {
            id: hfov0Slider
            minimumValue: 0
            maximumValue: 360
            DEG_SLIDER_SPINNER_PROPERTIES
            onValueChanged: updateProperty_hfov0()
        }
        KEYFRAME_BUTTON(hfov0)
        Shotcut.UndoButton {
            id: hfov0Undo
            onClicked: hfov0Slider.value = 180
        }

        Label {
            text: qsTr('End')
            Layout.alignment: Qt.AlignRight
        }
        Shotcut.SliderSpinner {
            id: hfov1Slider
            minimumValue: 0
            maximumValue: 360
            DEG_SLIDER_SPINNER_PROPERTIES
            onValueChanged: updateProperty_hfov1()
        }
        KEYFRAME_BUTTON(hfov1)
        Shotcut.UndoButton {
            id: hfov1Undo
            onClicked: hfov1Slider.value = 200
        }

        Label {
            text: qsTr('Vertical')
            Layout.alignment: Qt.AlignLeft
            Layout.columnSpan: 4
        }

        Label {
            text: qsTr('Start')
            Layout.alignment: Qt.AlignRight
        }
        Shotcut.SliderSpinner {
            id: vfov0Slider
            minimumValue: 0
            maximumValue: 360
            DEG_SLIDER_SPINNER_PROPERTIES
            onValueChanged: updateProperty_vfov0()
        }
        KEYFRAME_BUTTON(vfov0)
        Shotcut.UndoButton {
            id: vfov0Undo
            onClicked: vfov0Slider.value = 140
        }

        Label {
            text: qsTr('End')
            Layout.alignment: Qt.AlignRight
        }
        Shotcut.SliderSpinner {
            id: vfov1Slider
            minimumValue: 0
            maximumValue: 360
            DEG_SLIDER_SPINNER_PROPERTIES
            onValueChanged: updateProperty_vfov1()
        }
        KEYFRAME_BUTTON(vfov1)
        Shotcut.UndoButton {
            id: vfov1Undo
            onClicked: vfov1Slider.value = 160
        }
    }

    COMMON_CONNECTIONS
}