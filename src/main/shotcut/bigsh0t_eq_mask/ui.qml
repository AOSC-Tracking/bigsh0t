/* SPDX-License-Identifier: GPL-2.0-or-later */
#include "../shotcut_ui.qml"

import QtQuick 2.1
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.0
import Shotcut.Controls 1.0


Item {
    width: 350
    height: 100
    property bool blockUpdate: true
    
    PROPERTY_VARIABLES(hfov0)
    PROPERTY_VARIABLES(hfov1)
    PROPERTY_VARIABLES(vfov0)
    PROPERTY_VARIABLES(vfov1)
        
    PROPERTY_CONNECTIONS(hfov0)
    PROPERTY_CONNECTIONS(hfov1)
    PROPERTY_CONNECTIONS(vfov0)
    PROPERTY_CONNECTIONS(vfov1)
    
    Component.onCompleted: {
        ON_COMPLETED(hfov0, 180)
        ON_COMPLETED(hfov1, 200)
        ON_COMPLETED(vfov0, 140)
        ON_COMPLETED(vfov1, 160)
            
        if (filter.isNew) {
            filter.savePreset(preset.parameters)
        }
        setControls()
    }
    
    function setControls() {
        var position = getPosition()
        blockUpdate = true
        SET_CONTROLS(hfov0)
        SET_CONTROLS(hfov1)
        SET_CONTROLS(vfov0)
        SET_CONTROLS(vfov1)
        blockUpdate = false
    }
    
    UPDATE_PROPERTY(hfov0)
    UPDATE_PROPERTY(hfov1)
    UPDATE_PROPERTY(vfov0)
    UPDATE_PROPERTY(vfov1)
    
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
            parameters: ["hfov0", "hfov1", "vfov0", "vfov1"]
            Layout.columnSpan: 3
            onBeforePresetLoaded: {
                filter.resetProperty('hfov0')
                filter.resetProperty('hfov1')
                filter.resetProperty('vfov0')
                filter.resetProperty('vfov1')
            }
            onPresetSelected: {
                LOAD_PRESET(hfov0)
                LOAD_PRESET(hfov1)
                LOAD_PRESET(vfov0)
                LOAD_PRESET(vfov1)
                setControls(null);                
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
        SliderSpinner {
            id: hfov0Slider
            minimumValue: 0
            maximumValue: 360
            DEG_SLIDER_SPINNER_PROPERTIES
            onValueChanged: updateProperty_hfov0(getPosition())
        }
        KEYFRAME_BUTTON(hfov0)
        UndoButton {
            id: hfov0Undo
            onClicked: hfov0Slider.value = 180
        }
        
        Label {
            text: qsTr('End')
            Layout.alignment: Qt.AlignRight
        }
        SliderSpinner {
            id: hfov1Slider
            minimumValue: 0
            maximumValue: 360
            DEG_SLIDER_SPINNER_PROPERTIES
            onValueChanged: updateProperty_hfov1(getPosition())
        }
        KEYFRAME_BUTTON(hfov1)
        UndoButton {
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
        SliderSpinner {
            id: vfov0Slider
            minimumValue: 0
            maximumValue: 360
            DEG_SLIDER_SPINNER_PROPERTIES
            onValueChanged: updateProperty_vfov0(getPosition())
        }
        KEYFRAME_BUTTON(vfov0)
        UndoButton {
            id: vfov0Undo
            onClicked: vfov0Slider.value = 140
        }
        
        Label {
            text: qsTr('End')
            Layout.alignment: Qt.AlignRight
        }
        SliderSpinner {
            id: vfov1Slider
            minimumValue: 0
            maximumValue: 360
            DEG_SLIDER_SPINNER_PROPERTIES
            onValueChanged: updateProperty_vfov1(getPosition())
        }
        KEYFRAME_BUTTON(vfov1)
        UndoButton {
            id: vfov1Undo
            onClicked: vfov1Slider.value = 160
        }
    }
        
    Connections {
        target: producer
        onPositionChanged: setControls()
    }
}