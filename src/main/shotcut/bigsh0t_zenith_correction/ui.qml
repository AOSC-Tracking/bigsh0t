/* SPDX-License-Identifier: GPL-2.0-or-later */
#include "../shotcut_ui.qml"

import QtQuick 2.1
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.0
import QtQuick.Dialogs 1.1
import Shotcut.Controls 1.0


Item {
    width: 350
    height: 125 /* 5 rows of 25 pixels */
    property bool blockUpdate: true
    
    PROPERTY_VARIABLES_CHECKBOX(enableSmoothYaw)
    PROPERTY_VARIABLES_COMBOBOX(interpolation)
    PROPERTY_VARIABLES_TEXTFIELD(analysisFile)
    PROPERTY_VARIABLES_TEXTFIELD_NUM(clipOffset)
    PROPERTY_VARIABLES_STATIC(smoothYaw)
    PROPERTY_VARIABLES_STATIC(timeBiasYaw)
        
    PROPERTY_CONNECTIONS_COMBOBOX(interpolation)
    PROPERTY_CONNECTIONS_TEXTFIELD(analysisFile)
    PROPERTY_CONNECTIONS_TEXTFIELD_NUM(clipOffset)
    PROPERTY_CONNECTIONS_STATIC(smoothYaw)
    PROPERTY_CONNECTIONS_STATIC(timeBiasYaw)
        
    Component.onCompleted: {
        ON_COMPLETED_COMBOBOX(interpolation, 1)
        ON_COMPLETED_TEXTFIELD(analysisFile, "")
        ON_COMPLETED_TEXTFIELD_NUM(clipOffset, 0)
        ON_COMPLETED_CHECKBOX(enableSmoothYaw, false)
        ON_COMPLETED_STATIC(smoothYaw, 120)
        ON_COMPLETED_STATIC(timeBiasYaw, 0)
        
        if (filter.isNew) {
            filter.savePreset(preset.parameters)
        }
        setControls()
    }
    
    function setControls() {
        var position = getPosition()
        blockUpdate = true
        
        SET_CONTROLS_COMBOBOX(interpolation)
        SET_CONTROLS_TEXTFIELD(analysisFile)
        SET_CONTROLS_TEXTFIELD_NUM(clipOffset)

        SET_CONTROLS_CHECKBOX(enableSmoothYaw)
        SET_CONTROLS_STATIC(smoothYaw)
        SET_CONTROLS_STATIC(timeBiasYaw)

        blockUpdate = false
    }
    
    UPDATE_PROPERTY_COMBOBOX(interpolation)
    UPDATE_PROPERTY_TEXTFIELD(analysisFile)
    UPDATE_PROPERTY_TEXTFIELD_NUM(clipOffset)
    UPDATE_PROPERTY_CHECKBOX(enableSmoothYaw)
    UPDATE_PROPERTY_STATIC(smoothYaw)
    UPDATE_PROPERTY_STATIC(timeBiasYaw)

    
    function getPosition() {
        return Math.max(producer.position - (filter.in - producer.in), 0)
    }

    function getFrameRate() {
        return producer.getDouble("meta.media.frame_rate_num", getPosition()) / producer.getDouble("meta.media.frame_rate_den", getPosition())
    }

    function getClipOffset() {
        return filter.in
    }

    FileDialog {
        id: selectAnalysisFile
        title: "File for motion analysis"
        folder: shortcuts.home
        modality: Qt.WindowModal
        selectMultiple: false
        selectExisting: true
        selectFolder: false
        nameFilters: ['Theta MP4 (*.mp4)', 'All Files (*)']

        onAccepted: {
            var urlString = selectAnalysisFile.fileUrl.toString()
            analysisFileTextField.text = urlString
            
            updateProperty_analysisFile()
        }
        onRejected: {
        }
    }

    function onClipOffsetUndo() {
        clipOffsetTextField.text = (getClipOffset() / getFrameRate()).toFixed(3)
        updateProperty_clipOffset()
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
            parameters: ["interpolation"]
            Layout.columnSpan: 3
            onBeforePresetLoaded: {
                filter.resetProperty('interpolation')
            }
            onPresetSelected: {
                LOAD_PRESET_COMBOBOX(interpolation)
                setControls(null);                
            }
        }

        Label {
            text: qsTr('File')
            Layout.alignment: Qt.AlignRight
        }
        TextField {
            text: qsTr("")
            id: analysisFileTextField
            Layout.columnSpan: 2
            Layout.fillWidth: true
            Layout.alignment: Qt.AlignLeft
            onEditingFinished: updateProperty_analysisFile()
        }
        Button {
            iconName: 'document-open'
            iconSource: 'qrc:///icons/oxygen/32x32/actions/document-open.png'
            tooltip: qsTr('Browse...')
            implicitWidth: 20
            implicitHeight: 20
            onClicked: selectAnalysisFile.open()
        }

        Label {
            text: qsTr('Start Offset')
            Layout.alignment: Qt.AlignRight
        }
        TextField {
            text: qsTr("0")
            id: clipOffsetTextField
            Layout.columnSpan: 2
            Layout.fillWidth: true
            Layout.alignment: Qt.AlignLeft
            onEditingFinished: updateProperty_clipOffset()
        }
        UndoButton {
            id: clipOffsetUndo
            onClicked: onClipOffsetUndo()
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
        
        /* -------------------------------------------- */
        Label {
            text: qsTr('Yaw')
            Layout.alignment: Qt.AlignRight
        }
        CheckBox {
            text: qsTr('Smooth yaw')
            checked: false
            partiallyCheckedEnabled: false
            id: enableSmoothYawCheckBox
            Layout.columnSpan: 3
            onCheckedChanged: updateProperty_enableSmoothYaw()
        }

        Label {
            text: qsTr('Smoothing')
            Layout.alignment: Qt.AlignRight
        }
        SliderSpinner {
            id: smoothYawSlider
            minimumValue: 1
            maximumValue: 300
            suffix: ' frames'
            decimals: 0
            stepSize: 1
            Layout.columnSpan: 2
            onValueChanged: updateProperty_smoothYaw(getPosition())
        }
        UndoButton {
            id: smoothYawUndo
            onClicked: smoothYawSlider.value = 120
        }
        Label {
            text: qsTr('Time Bias')
            Layout.alignment: Qt.AlignRight
        }
        SliderSpinner {
            id: timeBiasYawSlider
            minimumValue: -100
            maximumValue: 100
            suffix: ' %'
            decimals: 0
            stepSize: 1
            Layout.columnSpan: 2
            onValueChanged: updateProperty_timeBiasYaw(getPosition())
        }
        UndoButton {
            id: timeBiasYawUndo
            onClicked: timeBiasYawSlider.value = 0
        }

        
    }
        
    Connections {
        target: producer
        onPositionChanged: setControls()
    }
}