/* SPDX-License-Identifier: GPL-2.0-or-later */
#include "../shotcut_ui.qml"

import QtQuick 2.1
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.0
import Shotcut.Controls 1.0


Item {
    width: 350
    height: 175 /* 7 rows of 25 pixels */
    property bool blockUpdate: true
    
    PROPERTY_VARIABLES(yaw)
    PROPERTY_VARIABLES(pitch)
    PROPERTY_VARIABLES(roll)
    PROPERTY_VARIABLES(fov)
	PROPERTY_VARIABLES(fisheye)
    PROPERTY_VARIABLES_COMBOBOX(interpolation)
    
    PROPERTY_CONNECTIONS(yaw)
    PROPERTY_CONNECTIONS(pitch)
    PROPERTY_CONNECTIONS(roll)
    PROPERTY_CONNECTIONS(fov)
	PROPERTY_CONNECTIONS(fisheye)
    PROPERTY_CONNECTIONS_COMBOBOX(interpolation)
    
    Component.onCompleted: {
        ON_COMPLETED(yaw, 0)
        ON_COMPLETED(pitch, 0)
        ON_COMPLETED(roll, 0)
        ON_COMPLETED(fov, 90)
		ON_COMPLETED(fisheye, 0)
        ON_COMPLETED_COMBOBOX(interpolation, 1)
            
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
        SET_CONTROLS(fov)
		SET_CONTROLS(fisheye)
        SET_CONTROLS_COMBOBOX(interpolation)
        blockUpdate = false
    }
    
    UPDATE_PROPERTY(yaw)
    UPDATE_PROPERTY(pitch)
    UPDATE_PROPERTY(roll)
    UPDATE_PROPERTY(fov)
	UPDATE_PROPERTY(fisheye)
    UPDATE_PROPERTY_COMBOBOX(interpolation)
    
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
            parameters: ["yaw", "pitch", "roll", "fov", "interpolation", "fisheye"]
            Layout.columnSpan: 3
            onBeforePresetLoaded: {
                filter.resetProperty('yaw')
                filter.resetProperty('pitch')
                filter.resetProperty('roll')
                filter.resetProperty('fov')
				filter.resetProperty('fisheye')
                filter.resetProperty('interpolation')
            }
            onPresetSelected: {
                LOAD_PRESET(yaw)
                LOAD_PRESET(pitch)
                LOAD_PRESET(roll)
                LOAD_PRESET(fov)
				LOAD_PRESET(fisheye)
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
            text: qsTr('FOV')
            Layout.alignment: Qt.AlignRight
        }
        SliderSpinner {
            id: fovSlider
            minimumValue: 0
            maximumValue: 720
            DEG_SLIDER_SPINNER_PROPERTIES
            onValueChanged: updateProperty_fov(getPosition())
        }
        KEYFRAME_BUTTON(fov)
        UndoButton {
            id: fovUndo
            onClicked: fovSlider.value = 90
        }

		Label {
            text: qsTr('Fisheye')
            Layout.alignment: Qt.AlignRight
        }
        SliderSpinner {
            id: fisheyeSlider
            minimumValue: 0
            maximumValue: 100
            suffix: ' %'
            decimals: 0
            stepSize: 1
            onValueChanged: updateProperty_fisheye(getPosition())
        }
        KEYFRAME_BUTTON(fisheye)
        UndoButton {
            id: fisheyeUndo
            onClicked: fisheyeSlider.value = 0
        }
    }
        
    Connections {
        target: producer
        onPositionChanged: setControls()
    }
}