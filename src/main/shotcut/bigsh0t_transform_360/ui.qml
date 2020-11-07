/* SPDX-License-Identifier: GPL-2.0-or-later */
#include "../shotcut_ui.qml"

import QtQuick 2.1
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.0
import Shotcut.Controls 1.0


Item {
    width: 350
    height: 125 /* 5 rows of 25 pixels */
    property bool blockUpdate: true
    
    PROPERTY_VARIABLES(yaw)
    PROPERTY_VARIABLES(pitch)
    PROPERTY_VARIABLES(roll)
    PROPERTY_VARIABLES_COMBOBOX(interpolation)
    PROPERTY_VARIABLES_CHECKBOX(grid)
    
    PROPERTY_CONNECTIONS(yaw)
    PROPERTY_CONNECTIONS(pitch)
    PROPERTY_CONNECTIONS(roll)
    PROPERTY_CONNECTIONS_COMBOBOX(interpolation)
    PROPERTY_CONNECTIONS_CHECKBOX(grid)
    
    Component.onCompleted: {
        ON_COMPLETED(yaw, 0)
        ON_COMPLETED(pitch, 0)
        ON_COMPLETED(roll, 0)
        ON_COMPLETED_COMBOBOX(interpolation, 1)
        ON_COMPLETED_CHECKBOX(grid, false)
            
        if (filter.isNew) {
            filter.savePreset(preset.parameters)
        }
        setControls()
    }
    
    function setControls() {
        var position = getPosition()
        blockUpdate = true
        SET_CONTROLS(yaw)
        SET_CONTROLS(pitch)
        SET_CONTROLS(roll)
        SET_CONTROLS_COMBOBOX(interpolation)
        SET_CONTROLS_CHECKBOX(grid)
        blockUpdate = false
    }
    
    UPDATE_PROPERTY(yaw)
    UPDATE_PROPERTY(pitch)
    UPDATE_PROPERTY(roll)
    UPDATE_PROPERTY_COMBOBOX(interpolation)
    UPDATE_PROPERTY_CHECKBOX(grid)
    
    function getPosition() {
        return Math.max(producer.position - (filter.in - producer.in), 0)
    }
    
    GridLayout {
        columns: 4
        anchors.fill: parent
        anchors.margins: 8
        
        Label {
            text: qsTr('Preset')
            Layout.alignment: Qt.AlignRight
        }
        Preset {
            id: preset
            parameters: ["yaw", "pitch", "roll", "interpolation"]
            Layout.columnSpan: 3
            onBeforePresetLoaded: {
                filter.resetProperty('yaw')
                filter.resetProperty('pitch')
                filter.resetProperty('roll')
                filter.resetProperty('interpolation')
            }
            onPresetSelected: {
                LOAD_PRESET(yaw)
                LOAD_PRESET(pitch)
                LOAD_PRESET(roll)
                LOAD_PRESET_COMBOBOX(interpolation)
                setControls(null);                
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
        UndoButton {
            id: interpolationUndo
            onClicked: interpolationComboBox.currentIndex = 0
        }
        
        Label {
            text: qsTr('Yaw')
            Layout.alignment: Qt.AlignRight
        }
        SliderSpinner {
            id: yawSlider
            minimumValue: -360
            maximumValue: 360
            DEG_SLIDER_SPINNER_PROPERTIES
            onValueChanged: updateProperty_yaw(getPosition())
        }
        KEYFRAME_BUTTON(yaw)
        UndoButton {
            id: yawUndo
            onClicked: yawSlider.value = 0
        }
        
        Label {
            text: qsTr('Pitch')
            Layout.alignment: Qt.AlignRight
        }
        SliderSpinner {
            id: pitchSlider
            minimumValue: -180
            maximumValue: 180
            DEG_SLIDER_SPINNER_PROPERTIES
            onValueChanged: updateProperty_pitch(getPosition())
        }
        KEYFRAME_BUTTON(pitch)
        UndoButton {
            id: pitchUndo
            onClicked: pitchSlider.value = 0
        }
        
        Label {
            text: qsTr('Roll')
            Layout.alignment: Qt.AlignRight
        }
        SliderSpinner {
            id: rollSlider
            minimumValue: -180
            maximumValue: 180
            DEG_SLIDER_SPINNER_PROPERTIES
            onValueChanged: updateProperty_roll(getPosition())
        }
        KEYFRAME_BUTTON(roll)
        UndoButton {
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
        
    Connections {
        target: producer
        onPositionChanged: setControls()
    }
}