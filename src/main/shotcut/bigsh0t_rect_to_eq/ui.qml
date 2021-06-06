/* SPDX-License-Identifier: GPL-2.0-or-later */
#include "../shotcut_ui.qml"

import QtQuick 2.1
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.0
import Shotcut.Controls 1.0 as Shotcut


Shotcut.KeyframableFilter {
    width: 350
    height: 100 /* 4 rows of 25 pixels */

    keyframableParameters: ["hfov", "vfov"]
    startValues: [0.5, 0.5]
    middleValues: [90.0, 60.0]
    endValues: [0.5, 0.5]

    property var allParameters: [
        {
            name: "hfov",
            type: "simple",
            def: 90
        },
        {
            name: "vfov",
            type: "simple",
            def: 60
        },
        {
            name: "interpolation",
            type: "combobox",
            def: 1
        }
    ]

    COMMON_FUNCTIONS

    Component.onCompleted: {
        defaultOnCompleted()
    }

    UPDATE_PROPERTY(hfov)
    UPDATE_PROPERTY(vfov)
    UPDATE_PROPERTY_COMBOBOX(interpolation)

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
            parameters: ["hfov", "vfov", "interpolation"]
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
            text: qsTr('Horizontal')
            Layout.alignment: Qt.AlignRight
        }
        Shotcut.SliderSpinner {
            id: hfovSlider
            minimumValue: 0
            maximumValue: 180
            DEG_SLIDER_SPINNER_PROPERTIES
            onValueChanged: updateProperty_hfov(getPosition())
        }
        KEYFRAME_BUTTON(hfov)
        Shotcut.UndoButton {
            id: hfovUndo
            onClicked: hfovSlider.value = 90
        }

        Label {
            text: qsTr('Vertical')
            Layout.alignment: Qt.AlignRight
        }
        Shotcut.SliderSpinner {
            id: vfovSlider
            minimumValue: 0
            maximumValue: 180
            DEG_SLIDER_SPINNER_PROPERTIES
            onValueChanged: updateProperty_vfov(getPosition())
        }
        KEYFRAME_BUTTON(vfov)
        Shotcut.UndoButton {
            id: vfovUndo
            onClicked: vfovSlider.value = 60
        }
    }

    COMMON_CONNECTIONS
}