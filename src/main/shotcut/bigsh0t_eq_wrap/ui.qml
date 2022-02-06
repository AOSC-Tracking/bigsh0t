/* SPDX-License-Identifier: GPL-2.0-or-later */
#include "../shotcut_ui.qml"

import QtQuick 2.1
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.0
import Shotcut.Controls 1.0 as Shotcut


Shotcut.KeyframableFilter {
    width: 350
    height: 225 /* 9 rows of 25 pixels */

    keyframableParameters: ["hfov0", "hfov1", "vfov0", "vfov1", "blurStart", "blurEnd"]
    startValues: [0.5, 0.5, 0.5, 0.5, 0.5, 0.5]
    middleValues: [-90.0, 90.0, -45.0, 45.0, 0.1, 1.0]
    endValues: [0.5, 0.5, 0.5, 0.5, 0.5, 0.5]

    property var allParameters: [
        {
            name: "hfov0",
            type: "simple",
            def: -90
        },
        {
            name: "hfov1",
            type: "simple",
            def: 90
        },
        {
            name: "vfov0",
            type: "simple",
            def: -45
        },
        {
            name: "vfov1",
            type: "simple",
            def: 45
        },
        {
            name: "blurStart",
            type: "simple",
            def: 0.1
        },
        {
            name: "blurEnd",
            type: "simple",
            def: 1.0
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
    UPDATE_PROPERTY(blurStart)
    UPDATE_PROPERTY(blurEnd)

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
            parameters: ["hfov0", "hfov1", "vfov0", "vfov1", "blurStart", "blurEnd"]
            Layout.columnSpan: 3
            onBeforePresetLoaded: {
                defaultBeforePresetLoaded()
            }
            onPresetSelected: {
                defaultPresetSelected()
            }
        }

        Label {
            text: qsTr('Left')
            Layout.alignment: Qt.AlignRight
        }
        Shotcut.SliderSpinner {
            id: hfov0Slider
            minimumValue: -180
            maximumValue: 180
            DEG_SLIDER_SPINNER_PROPERTIES
            onValueChanged: updateProperty_hfov0()
        }
        KEYFRAME_BUTTON(hfov0)
        Shotcut.UndoButton {
            id: hfov0Undo
            onClicked: hfov0Slider.value = -90
        }

        Label {
            text: qsTr('Right')
            Layout.alignment: Qt.AlignRight
        }
        Shotcut.SliderSpinner {
            id: hfov1Slider
            minimumValue: -180
            maximumValue: 180
            DEG_SLIDER_SPINNER_PROPERTIES
            onValueChanged: updateProperty_hfov1()
        }
        KEYFRAME_BUTTON(hfov1)
        Shotcut.UndoButton {
            id: hfov1Undo
            onClicked: hfov1Slider.value = 90
        }

        Label {
            text: qsTr('Top')
            Layout.alignment: Qt.AlignRight
        }
        Shotcut.SliderSpinner {
            id: vfov0Slider
            minimumValue: -90
            maximumValue: 90
            DEG_SLIDER_SPINNER_PROPERTIES
            onValueChanged: updateProperty_vfov0()
        }
        KEYFRAME_BUTTON(vfov0)
        Shotcut.UndoButton {
            id: vfov0Undo
            onClicked: vfov0Slider.value = -45
        }

        Label {
            text: qsTr('Bottom')
            Layout.alignment: Qt.AlignRight
        }
        Shotcut.SliderSpinner {
            id: vfov1Slider
            minimumValue: -90
            maximumValue: 90
            DEG_SLIDER_SPINNER_PROPERTIES
            onValueChanged: updateProperty_vfov1()
        }
        KEYFRAME_BUTTON(vfov1)
        Shotcut.UndoButton {
            id: vfov1Undo
            onClicked: vfov1Slider.value = 45
        }

        Label {
            text: qsTr('Blur Start')
            Layout.alignment: Qt.AlignRight
        }
        Shotcut.SliderSpinner {
            id: blurStartSlider
            minimumValue: 0
            maximumValue: 2.0
            spinnerWidth: 120;
            suffix: '';
            decimals: 2;
            stepSize: 0.01;
            onValueChanged: updateProperty_blurStart()
        }
        KEYFRAME_BUTTON(blurStart)
        Shotcut.UndoButton {
            id: blurStartUndo
            onClicked: blurStartSlider.value = 0.2
        }

        Label {
            text: qsTr('Blur End')
            Layout.alignment: Qt.AlignRight
        }
        Shotcut.SliderSpinner {
            id: blurEndSlider
            minimumValue: 0
            maximumValue: 2.0
            spinnerWidth: 120;
            suffix: '';
            decimals: 2;
            stepSize: 0.01;
            onValueChanged: updateProperty_blurEnd()
        }
        KEYFRAME_BUTTON(blurEnd)
        Shotcut.UndoButton {
            id: blurEndUndo
            onClicked: blurEndSlider.value = 1.5
        }
    }

    COMMON_CONNECTIONS
}