/* SPDX-License-Identifier: GPL-2.0-or-later */
#include "../shotcut_ui.qml"

import QtQuick 2.1
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.0
import QtQuick.Dialogs 1.1
import Shotcut.Controls 1.0


Item {
    width: 350
    height: 575 /* 23 rows of 25 pixels */
    property bool blockUpdate: true
    
    PROPERTY_VARIABLES_COMBOBOX(interpolation)
    PROPERTY_VARIABLES_CHECKBOX(analyze)
    PROPERTY_VARIABLES_CHECKBOX(transformWhenAnalyzing)
    PROPERTY_VARIABLES_STATIC(sampleRadius)
    PROPERTY_VARIABLES_STATIC(searchRadius)
    PROPERTY_VARIABLES_STATIC(offset)
    PROPERTY_VARIABLES_COMBOBOX(subpixels)
    PROPERTY_VARIABLES_TEXTFIELD(analysisFile)
    PROPERTY_VARIABLES_TEXTFIELD_NUM(clipOffset)
    PROPERTY_VARIABLES_CHECKBOX(useBackTrackpoints)
    PROPERTY_VARIABLES_STATIC(stabilizeYaw)
    PROPERTY_VARIABLES_STATIC(stabilizePitch)
    PROPERTY_VARIABLES_STATIC(stabilizeRoll)
    PROPERTY_VARIABLES_STATIC(smoothYaw)
    PROPERTY_VARIABLES_STATIC(smoothPitch)
    PROPERTY_VARIABLES_STATIC(smoothRoll)
    PROPERTY_VARIABLES_STATIC(timeBiasYaw)
    PROPERTY_VARIABLES_STATIC(timeBiasPitch)
    PROPERTY_VARIABLES_STATIC(timeBiasRoll)
    
    PROPERTY_CONNECTIONS_COMBOBOX(interpolation)
    PROPERTY_CONNECTIONS_CHECKBOX(analyze)
    PROPERTY_CONNECTIONS_CHECKBOX(transformWhenAnalyzing)
    PROPERTY_CONNECTIONS_STATIC(sampleRadius)
    PROPERTY_CONNECTIONS_STATIC(searchRadius)
    PROPERTY_CONNECTIONS_STATIC(offset)
    PROPERTY_CONNECTIONS_COMBOBOX(subpixels)
    PROPERTY_CONNECTIONS_TEXTFIELD(analysisFile)
    PROPERTY_CONNECTIONS_TEXTFIELD_NUM(clipOffset)
    PROPERTY_CONNECTIONS_CHECKBOX(useBackTrackpoints)
    PROPERTY_CONNECTIONS_STATIC(stabilizeYaw)
    PROPERTY_CONNECTIONS_STATIC(stabilizePitch)
    PROPERTY_CONNECTIONS_STATIC(stabilizeRoll)
    PROPERTY_CONNECTIONS_STATIC(smoothYaw)
    PROPERTY_CONNECTIONS_STATIC(smoothPitch)
    PROPERTY_CONNECTIONS_STATIC(smoothRoll)
    PROPERTY_CONNECTIONS_STATIC(timeBiasYaw)
    PROPERTY_CONNECTIONS_STATIC(timeBiasPitch)
    PROPERTY_CONNECTIONS_STATIC(timeBiasRoll)
    
    Component.onCompleted: {
        ON_COMPLETED_CHECKBOX(analyze, false)
        ON_COMPLETED_CHECKBOX(transformWhenAnalyzing, true)
        ON_COMPLETED_COMBOBOX(interpolation, 1)
        ON_COMPLETED_STATIC(sampleRadius, 16)
        ON_COMPLETED_STATIC(searchRadius, 24)
        ON_COMPLETED_STATIC(offset, 64)
        ON_COMPLETED_COMBOBOX(subpixels, 1)
        ON_COMPLETED_TEXTFIELD(analysisFile, "")
        ON_COMPLETED_TEXTFIELD_NUM(clipOffset, 0)
        ON_COMPLETED_CHECKBOX(useBackTrackpoints, false)

        ON_COMPLETED_STATIC(stabilizeYaw, 100)
        ON_COMPLETED_STATIC(stabilizePitch, 100)
        ON_COMPLETED_STATIC(stabilizeRoll, 100)
        ON_COMPLETED_STATIC(smoothYaw, 120)
        ON_COMPLETED_STATIC(smoothPitch, 120)
        ON_COMPLETED_STATIC(smoothRoll, 120)
        ON_COMPLETED_STATIC(timeBiasYaw, 0)
        ON_COMPLETED_STATIC(timeBiasPitch, 0)
        ON_COMPLETED_STATIC(timeBiasRoll, 0)
            
        if (filter.isNew) {
            filter.savePreset(preset.parameters)
        }
        setControls()
    }
    
    function setControls() {
        var position = getPosition()
        blockUpdate = true
        SET_CONTROLS_CHECKBOX(analyze)
        SET_CONTROLS_CHECKBOX(transformWhenAnalyzing)
        SET_CONTROLS_COMBOBOX(interpolation)
        SET_CONTROLS_STATIC(sampleRadius)
        SET_CONTROLS_STATIC(searchRadius)
        SET_CONTROLS_STATIC(offset)
        SET_CONTROLS_COMBOBOX(subpixels)
        SET_CONTROLS_TEXTFIELD(analysisFile)
        SET_CONTROLS_TEXTFIELD_NUM(clipOffset)
        SET_CONTROLS_CHECKBOX(useBackTrackpoints)
        SET_CONTROLS_STATIC(stabilizeYaw)
        SET_CONTROLS_STATIC(stabilizePitch)
        SET_CONTROLS_STATIC(stabilizeRoll)
        SET_CONTROLS_STATIC(smoothYaw)
        SET_CONTROLS_STATIC(smoothPitch)
        SET_CONTROLS_STATIC(smoothRoll)
        SET_CONTROLS_STATIC(timeBiasYaw)
        SET_CONTROLS_STATIC(timeBiasPitch)
        SET_CONTROLS_STATIC(timeBiasRoll)

        blockUpdate = false
    }
    
    UPDATE_PROPERTY_CHECKBOX(analyze)
    UPDATE_PROPERTY_CHECKBOX(transformWhenAnalyzing)
    UPDATE_PROPERTY_COMBOBOX(interpolation)
    UPDATE_PROPERTY_STATIC(sampleRadius)
    UPDATE_PROPERTY_STATIC(searchRadius)
    UPDATE_PROPERTY_STATIC(offset)
    UPDATE_PROPERTY_COMBOBOX(subpixels)
    UPDATE_PROPERTY_TEXTFIELD(analysisFile)
    UPDATE_PROPERTY_TEXTFIELD_NUM(clipOffset)
    UPDATE_PROPERTY_CHECKBOX(useBackTrackpoints)
    UPDATE_PROPERTY_STATIC(stabilizeYaw)
    UPDATE_PROPERTY_STATIC(stabilizePitch)
    UPDATE_PROPERTY_STATIC(stabilizeRoll)
    UPDATE_PROPERTY_STATIC(smoothYaw)
    UPDATE_PROPERTY_STATIC(smoothPitch)
    UPDATE_PROPERTY_STATIC(smoothRoll)
    UPDATE_PROPERTY_STATIC(timeBiasYaw)
    UPDATE_PROPERTY_STATIC(timeBiasPitch)
    UPDATE_PROPERTY_STATIC(timeBiasRoll)

    
    function getPosition() {
        return Math.max(producer.position - (filter.in - producer.in), 0)
    }

    function getFrameRate() {
        /* console.log(producer.get("resource", "None")) */
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
        selectExisting: false
        selectFolder: false
        nameFilters: ['Motion Analysis Files (*.bigsh0t360motion)', 'All Files (*)']

        onAccepted: {
            var urlString = selectAnalysisFile.fileUrl.toString()
            analysisFileTextField.text = urlString
            
            updateProperty_analysisFile()
        }
        onRejected: {
        }
    }

    function toggleMode() {
        if (blockUpdate) return;
        if (analyzeCheckBox.checked && analysisFileTextField.text == "") {
            selectAnalysisFile.open()
        }
        updateProperty_analyze()
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
            parameters: ["sampleRadius", "searchRadius", "offset", "subpixels", "interpolation", "stabilizeYaw", "stabilizePitch", "stabilizeRoll", "smoothYaw", "smoothPitch", "smoothRoll", "timeBiasYaw", "timeBiasPitch", "timeBiasRoll"]
            Layout.columnSpan: 3
            onBeforePresetLoaded: {
                filter.resetProperty("sampleRadius")
                filter.resetProperty("searchRadius")
                filter.resetProperty("offset")
                filter.resetProperty("subpixels")
                filter.resetProperty("interpolation")
                filter.resetProperty("stabilizeYaw")
                filter.resetProperty("stabilizePitch")
                filter.resetProperty("stabilizeRoll")
                filter.resetProperty("smoothYaw")
                filter.resetProperty("smoothPitch")
                filter.resetProperty("smoothRoll")
                filter.resetProperty("timeBiasYaw")
                filter.resetProperty("timeBiasPitch")
                filter.resetProperty("timeBiasRoll")
            }
            onPresetSelected: {
                LOAD_PRESET_STATIC(sampleRadius)
                LOAD_PRESET_STATIC(searchRadius)
                LOAD_PRESET_STATIC(offset)
                LOAD_PRESET_COMBOBOX(subpixels)
                LOAD_PRESET_COMBOBOX(interpolation)
                LOAD_PRESET_STATIC(stabilizeYaw)
                LOAD_PRESET_STATIC(stabilizePitch)
                LOAD_PRESET_STATIC(stabilizeRoll)
                LOAD_PRESET_STATIC(smoothYaw)
                LOAD_PRESET_STATIC(smoothPitch)
                LOAD_PRESET_STATIC(smoothRoll)
                LOAD_PRESET_STATIC(timeBiasYaw)
                LOAD_PRESET_STATIC(timeBiasPitch)
                LOAD_PRESET_STATIC(timeBiasRoll)

                setControls(null);
            }
        }

        Label {
            text: qsTr('Mode')
            Layout.alignment: Qt.AlignRight
        }
        CheckBox {
            text: qsTr('Analyze')
            checked: false
            partiallyCheckedEnabled: false
            id: analyzeCheckBox
            Layout.columnSpan: 3
            onCheckedChanged: toggleMode()
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
            text: qsTr('Analysis')
            Layout.alignment: Qt.AlignLeft
            Layout.columnSpan: 4
        }

        Label {
            text: qsTr('')
            Layout.alignment: Qt.AlignRight
        }
        CheckBox {
            text: qsTr('Apply Transform')
            checked: true
            partiallyCheckedEnabled: false
            id: transformWhenAnalyzingCheckBox
            Layout.columnSpan: 3
            onCheckedChanged: updateProperty_transformWhenAnalyzing()
        }

        Label {
            text: qsTr('Sample Radius')
            Layout.alignment: Qt.AlignRight
        }
        SliderSpinner {
            id: sampleRadiusSlider
            minimumValue: 1
            maximumValue: 64
            suffix: ' px'
            decimals: 0
            stepSize: 1
            Layout.columnSpan: 2
            onValueChanged: updateProperty_sampleRadius(getPosition())
        }
        UndoButton {
            id: sampleRadiusUndo
            onClicked: sampleRadiusSlider.value = 16
        }
        
        Label {
            text: qsTr('Search Radius')
            Layout.alignment: Qt.AlignRight
        }
        SliderSpinner {
            id: searchRadiusSlider
            minimumValue: 1
            maximumValue: 128
            suffix: ' px'
            decimals: 0
            stepSize: 1
            Layout.columnSpan: 2
            onValueChanged: updateProperty_searchRadius(getPosition())
        }
        UndoButton {
            id: searchRadiusUndo
            onClicked: searchRadiusSlider.value = 24
        }

        Label {
            text: qsTr('Offset')
            Layout.alignment: Qt.AlignRight
        }
        SliderSpinner {
            id: offsetSlider
            minimumValue: 1
            maximumValue: 256
            suffix: ' px'
            decimals: 0
            stepSize: 1
            Layout.columnSpan: 2
            onValueChanged: updateProperty_offset(getPosition())
        }
        UndoButton {
            id: offsetUndo
            onClicked: offsetSlider.value = 64
        }

        Label {
            text: qsTr('Track Points')
            Layout.alignment: Qt.AlignRight
        }
        CheckBox {
            text: qsTr('Use backwards-facing track points')
            checked: false
            partiallyCheckedEnabled: false
            id: useBackTrackpointsCheckBox
            Layout.columnSpan: 3
            onCheckedChanged: updateProperty_useBackTrackpoints()
        }

        Label {
            text: qsTr('Subpixels')
            Layout.alignment: Qt.AlignRight
        }
        ComboBox {
            currentIndex: 0
            model: ["1x1 (disabled)", "2x2", "4x4", "8x8"]
            id: subpixelsComboBox
            Layout.columnSpan: 2
            onCurrentIndexChanged: updateProperty_subpixels()
        }
        UndoButton {
            id: subpixelsUndo
            onClicked: subpixelsComboBox.currentIndex = 0
        }

        /* -------------------------------------------- */
        Label {
            text: qsTr('Yaw')
            Layout.alignment: Qt.AlignLeft
            Layout.columnSpan: 4
        }

        Label {
            text: qsTr('Amount')
            Layout.alignment: Qt.AlignRight
        }
        SliderSpinner {
            id: stabilizeYawSlider
            minimumValue: 0
            maximumValue: 100
            suffix: ' %'
            decimals: 0
            stepSize: 1
            Layout.columnSpan: 2
            onValueChanged: updateProperty_stabilizeYaw(getPosition())
        }
        UndoButton {
            id: stabilizeYawUndo
            onClicked: stabilizeYawSlider.value = 100
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

        Label {
            text: qsTr('Pitch')
            Layout.alignment: Qt.AlignLeft
            Layout.columnSpan: 4
        }
        Label {
            text: qsTr('Amount')
            Layout.alignment: Qt.AlignRight
        }
        SliderSpinner {
            id: stabilizePitchSlider
            minimumValue: 0
            maximumValue: 100
            suffix: ' %'
            decimals: 0
            stepSize: 1
            Layout.columnSpan: 2
            onValueChanged: updateProperty_stabilizePitch(getPosition())
        }
        UndoButton {
            id: stabilizePitchUndo
            onClicked: stabilizePitchSlider.value = 100
        }
        Label {
            text: qsTr('Smoothing')
            Layout.alignment: Qt.AlignRight
        }
        SliderSpinner {
            id: smoothPitchSlider
            minimumValue: 1
            maximumValue: 300
            suffix: ' frames'
            decimals: 0
            stepSize: 1
            Layout.columnSpan: 2
            onValueChanged: updateProperty_smoothPitch(getPosition())
        }
        UndoButton {
            id: smoothPitchUndo
            onClicked: smoothPitchSlider.value = 120
        }
        Label {
            text: qsTr('Time Bias')
            Layout.alignment: Qt.AlignRight
        }
        SliderSpinner {
            id: timeBiasPitchSlider
            minimumValue: -100
            maximumValue: 100
            suffix: ' %'
            decimals: 0
            stepSize: 1
            Layout.columnSpan: 2
            onValueChanged: updateProperty_timeBiasPitch(getPosition())
        }
        UndoButton {
            id: timeBiasPitchUndo
            onClicked: timeBiasPitchSlider.value = 0
        }

        Label {
            text: qsTr('Roll')
            Layout.alignment: Qt.AlignLeft
            Layout.columnSpan: 4
        }
        Label {
            text: qsTr('Amount')
            Layout.alignment: Qt.AlignRight
        }
        SliderSpinner {
            id: stabilizeRollSlider
            minimumValue: 0
            maximumValue: 100
            suffix: ' %'
            decimals: 0
            stepSize: 1
            Layout.columnSpan: 2
            onValueChanged: updateProperty_stabilizeRoll(getPosition())
        }
        UndoButton {
            id: stabilizeRollUndo
            onClicked: stabilizeRollSlider.value = 100
        }
        Label {
            text: qsTr('Smoothing')
            Layout.alignment: Qt.AlignRight
        }
        SliderSpinner {
            id: smoothRollSlider
            minimumValue: 1
            maximumValue: 300
            suffix: ' frames'
            decimals: 0
            stepSize: 1
            Layout.columnSpan: 2
            onValueChanged: updateProperty_smoothRoll(getPosition())
        }
        UndoButton {
            id: smoothRollUndo
            onClicked: smoothRollSlider.value = 120
        }
        Label {
            text: qsTr('Time Bias')
            Layout.alignment: Qt.AlignRight
        }
        SliderSpinner {
            id: timeBiasRollSlider
            minimumValue: -100
            maximumValue: 100
            suffix: ' %'
            decimals: 0
            stepSize: 1
            Layout.columnSpan: 2
            onValueChanged: updateProperty_timeBiasRoll(getPosition())
        }
        UndoButton {
            id: timeBiasRollUndo
            onClicked: timeBiasRollSlider.value = 0
        }
    }
        
    Connections {
        target: producer
        onPositionChanged: setControls()
    }
}