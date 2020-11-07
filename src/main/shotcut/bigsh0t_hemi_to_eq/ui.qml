/* SPDX-License-Identifier: GPL-2.0-or-later */
#include "../shotcut_ui.qml"

import QtQuick 2.1
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.0
import Shotcut.Controls 1.0


Item {
    width: 350
    height: 525 /* 21 rows of 25 pixels */
    property bool blockUpdate: true
    
    PROPERTY_VARIABLES(yaw)
    PROPERTY_VARIABLES(pitch)
    PROPERTY_VARIABLES(roll)
    
    PROPERTY_VARIABLES(frontX)
    PROPERTY_VARIABLES(frontY)
    PROPERTY_VARIABLES(frontUp)
    
    PROPERTY_VARIABLES(backX)
    PROPERTY_VARIABLES(backY)
    PROPERTY_VARIABLES(backUp)
    
    PROPERTY_VARIABLES(fov)
    PROPERTY_VARIABLES(radius)
    
    PROPERTY_VARIABLES(nadirRadius)
    PROPERTY_VARIABLES(nadirCorrectionStart)    
    
    PROPERTY_VARIABLES_COMBOBOX(interpolation)
    PROPERTY_VARIABLES_COMBOBOX(projection)
    
    PROPERTY_CONNECTIONS(yaw)
    PROPERTY_CONNECTIONS(pitch)
    PROPERTY_CONNECTIONS(roll)
    
    PROPERTY_CONNECTIONS(frontX)
    PROPERTY_CONNECTIONS(frontY)
    PROPERTY_CONNECTIONS(frontUp)
    
    PROPERTY_CONNECTIONS(backX)
    PROPERTY_CONNECTIONS(backY)
    PROPERTY_CONNECTIONS(backUp)
    
    PROPERTY_CONNECTIONS(fov)
    PROPERTY_CONNECTIONS(radius)
    
    PROPERTY_CONNECTIONS(nadirRadius)
    PROPERTY_CONNECTIONS(nadirCorrectionStart)
    
    PROPERTY_CONNECTIONS_COMBOBOX(interpolation)
    PROPERTY_CONNECTIONS_COMBOBOX(projection)
    
    Component.onCompleted: {
        ON_COMPLETED(yaw, 0)
        ON_COMPLETED(pitch, 0)
        ON_COMPLETED(roll, 0)
        ON_COMPLETED(frontX, 0.75)
        ON_COMPLETED(frontY, 0.5)
        ON_COMPLETED(frontUp, 90)
        ON_COMPLETED(backX, 0.25)
        ON_COMPLETED(backY, 0.5)
        ON_COMPLETED(backUp, 270)
        
        ON_COMPLETED(fov, 180)
        ON_COMPLETED(radius, 0.25)
        
        ON_COMPLETED(nadirRadius, 0.2229)
        ON_COMPLETED(nadirCorrectionStart, 0.8)
        
        ON_COMPLETED_COMBOBOX(interpolation, 1)
        ON_COMPLETED_COMBOBOX(projection, 0)
            
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
        SET_CONTROLS(frontX)
        SET_CONTROLS(frontY)
        SET_CONTROLS(frontUp)
        SET_CONTROLS(backX)
        SET_CONTROLS(backY)
        SET_CONTROLS(backUp)
        SET_CONTROLS(fov)
        SET_CONTROLS(radius)
        SET_CONTROLS(nadirRadius)
        SET_CONTROLS(nadirCorrectionStart)        
        SET_CONTROLS_COMBOBOX(interpolation)
        SET_CONTROLS_COMBOBOX(projection)
        
        blockUpdate = false
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
            parameters: ["yaw", "pitch", "roll", "frontX", "frontY", "frontUp", "backX", "backY", "backUp", "fov", "radius", "nadirRadius", "nadirCorrectionStart", "interpolation", "projection"]
            Layout.columnSpan: 3
            onBeforePresetLoaded: {
                filter.resetProperty('yaw')
                filter.resetProperty('pitch')
                filter.resetProperty('roll')
                filter.resetProperty('frontX')
                filter.resetProperty('frontY')
                filter.resetProperty('frontUp')
                filter.resetProperty('backX')
                filter.resetProperty('backY')
                filter.resetProperty('backUp')
                filter.resetProperty('fov')
                filter.resetProperty('radius')
                filter.resetProperty('nadirRadius')
                filter.resetProperty('nadirCorrectionStart')
                filter.resetProperty('interpolation')
                filter.resetProperty('projection')
            }
            onPresetSelected: {
                LOAD_PRESET(yaw)
                LOAD_PRESET(pitch)
                LOAD_PRESET(roll)
                LOAD_PRESET(frontX)
                LOAD_PRESET(frontY)
                LOAD_PRESET(frontUp)
                LOAD_PRESET(backX)
                LOAD_PRESET(backY)
                LOAD_PRESET(backUp)
                LOAD_PRESET(fov)
                LOAD_PRESET(radius)
                LOAD_PRESET(nadirRadius)
                LOAD_PRESET(nadirCorrectionStart)
                LOAD_PRESET_COMBOBOX(interpolation)
                LOAD_PRESET_COMBOBOX(projection)
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
            text: qsTr('Alignment')
            Layout.alignment: Qt.AlignLeft
            Layout.columnSpan: 4
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
        UndoButton {
            id: projectionUndo
            onClicked: projectionComboBox.currentIndex = 0
        }
        
        Label {
            text: qsTr('FOV')
            Layout.alignment: Qt.AlignRight
        }
        SliderSpinner {
            id: fovSlider
            minimumValue: 0
            maximumValue: 360
            spinnerWidth: 120
            suffix: ' deg'
            decimals: 4
            stepSize: 0.0001
            onValueChanged: updateProperty_fov(getPosition())
        }
        KEYFRAME_BUTTON(fov)
        UndoButton {
            id: fovUndo
            onClicked: fovSlider.value = 180
        }
        
        Label {
            text: qsTr('Radius')
            Layout.alignment: Qt.AlignRight
        }
        SliderSpinner {
            id: radiusSlider
            minimumValue: 0
            maximumValue: 1
            suffix: ' '
            decimals: 4
            stepSize: 0.0001
            onValueChanged: updateProperty_radius(getPosition())
        }
        KEYFRAME_BUTTON(radius)
        UndoButton {
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
        SliderSpinner {
            id: frontXSlider
            minimumValue: 0
            maximumValue: 1
            stepSize: 0.0001
            decimals: 4
            suffix: ' '
            onValueChanged: updateProperty_frontX(getPosition())
        }
        KEYFRAME_BUTTON(frontX)
        UndoButton {
            id: frontXUndo
            onClicked: frontXSlider.value = 0.25
        }
        
        Label {
            text: qsTr('Y')
            Layout.alignment: Qt.AlignRight
        }
        SliderSpinner {
            id: frontYSlider
            minimumValue: 0
            maximumValue: 1
            stepSize: 0.0001
            decimals: 4
            suffix: ' '
            onValueChanged: updateProperty_frontY(getPosition())
        }
        KEYFRAME_BUTTON(frontY)
        UndoButton {
            id: frontYUndo
            onClicked: frontYSlider.value = 0.25
        }
        
        Label {
            text: qsTr('Up')
            Layout.alignment: Qt.AlignRight
        }
        SliderSpinner {
            id: frontUpSlider
            minimumValue: 0
            maximumValue: 360
            DEG_SLIDER_SPINNER_PROPERTIES
            onValueChanged: updateProperty_frontUp(getPosition())
        }
        KEYFRAME_BUTTON(frontUp)
        UndoButton {
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
        SliderSpinner {
            id: backXSlider
            minimumValue: 0
            maximumValue: 1
            stepSize: 0.0001
            decimals: 4
            suffix: ' '
            onValueChanged: updateProperty_backX(getPosition())
        }
        KEYFRAME_BUTTON(backX)
        UndoButton {
            id: backXUndo
            onClicked: backXSlider.value = 0.25
        }
        
        Label {
            text: qsTr('Y')
            Layout.alignment: Qt.AlignRight
        }
        SliderSpinner {
            id: backYSlider
            minimumValue: 0
            maximumValue: 1
            stepSize: 0.0001
            decimals: 4
            suffix: ' '
            onValueChanged: updateProperty_backY(getPosition())
        }
        KEYFRAME_BUTTON(backY)
        UndoButton {
            id: backYUndo
            onClicked: backYSlider.value = 0.25
        }
        
        Label {
            text: qsTr('Up')
            Layout.alignment: Qt.AlignRight
        }
        SliderSpinner {
            id: backUpSlider
            minimumValue: 0
            maximumValue: 360
            DEG_SLIDER_SPINNER_PROPERTIES
            onValueChanged: updateProperty_backUp(getPosition())
        }
        KEYFRAME_BUTTON(backUp)
        UndoButton {
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
        SliderSpinner {
            id: nadirRadiusSlider
            minimumValue: 0
            maximumValue: 1
            suffix: ' '
            decimals: 4
            stepSize: 0.0001
            onValueChanged: updateProperty_nadirRadius(getPosition())
        }
        KEYFRAME_BUTTON(nadirRadius)
        UndoButton {
            id: nadirRadiusUndo
            onClicked: nadirRadiusSlider.value = 0.2229
        }
        
        Label {
            text: qsTr('Start')
            Layout.alignment: Qt.AlignRight
        }
        SliderSpinner {
            id: nadirCorrectionStartSlider
            minimumValue: 0
            maximumValue: 1
            suffix: ' '
            decimals: 4
            stepSize: 0.0001
            onValueChanged: updateProperty_nadirCorrectionStart(getPosition())
        }
        KEYFRAME_BUTTON(nadirCorrectionStart)
        UndoButton {
            id: nadirCorrectionStartUndo
            onClicked: radiusSlider.value = 0.8
        }
    }
        
    Connections {
        target: producer
        onPositionChanged: setControls()
    }
}