/* SPDX-License-Identifier: GPL-2.0-or-later */
#include "../shotcut_ui.qml"

import QtQuick 2.1
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.0
import Shotcut.Controls 1.0


Item {
    width: 350
    height: 100 /* 4 rows of 25 pixels */
    property bool blockUpdate: true

    PROPERTY_VARIABLES(hfov)
    PROPERTY_VARIABLES(vfov)
    PROPERTY_VARIABLES_COMBOBOX(interpolation)

    PROPERTY_CONNECTIONS(hfov)
    PROPERTY_CONNECTIONS(vfov)
    PROPERTY_CONNECTIONS_COMBOBOX(interpolation)
    
    Component.onCompleted: {
        ON_COMPLETED(hfov, 90)
        ON_COMPLETED(vfov, 60)
        ON_COMPLETED_COMBOBOX(interpolation, 1)
            
        if (filter.isNew) {
            filter.savePreset(preset.parameters)
        }
        setControls()
    }
    
    function setControls() {
        var position = getPosition()
        blockUpdate = true
        SET_CONTROLS(hfov)
        SET_CONTROLS(vfov)
        SET_CONTROLS_COMBOBOX(interpolation)
        blockUpdate = false
    }
    
    UPDATE_PROPERTY(hfov)
    UPDATE_PROPERTY(vfov)
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
            parameters: ["hfov", "vfov", "interpolation"]
            Layout.columnSpan: 3
            onBeforePresetLoaded: {
                filter.resetProperty('hfov')
                filter.resetProperty('vfov')
                filter.resetProperty('interpolation')
            }
            onPresetSelected: {
                LOAD_PRESET(hfov)
                LOAD_PRESET(vfov)
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
            text: qsTr('Horizontal')
            Layout.alignment: Qt.AlignRight
        }
        SliderSpinner {
            id: hfovSlider
            minimumValue: 0
            maximumValue: 180
            DEG_SLIDER_SPINNER_PROPERTIES
            onValueChanged: updateProperty_hfov(getPosition())
        }
        KEYFRAME_BUTTON(hfov)
        UndoButton {
            id: hfovUndo
            onClicked: hfovSlider.value = 90
        }
        
        Label {
            text: qsTr('Vertical')
            Layout.alignment: Qt.AlignRight
        }
        SliderSpinner {
            id: vfovSlider
            minimumValue: 0
            maximumValue: 180
            DEG_SLIDER_SPINNER_PROPERTIES
            onValueChanged: updateProperty_vfov(getPosition())
        }
        KEYFRAME_BUTTON(vfov)
        UndoButton {
            id: vfovUndo
            onClicked: vfovSlider.value = 60
        }
    }
        
    Connections {
        target: producer
        onPositionChanged: setControls()
    }
}