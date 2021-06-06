/* SPDX-License-Identifier: GPL-2.0-or-later */
#include "../shotcut_ui.qml"

import QtQuick 2.1
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.0
import QtQuick.Dialogs 1.1
import Shotcut.Controls 1.0 as Shotcut


Shotcut.KeyframableFilter {
    width: 350
    height: 600 /* 24 rows of 25 pixels */

    property var allParameters: [
        {
            name: "analyze",
            type: "checkbox",
            def: false
        },
        {
            name: "transformWhenAnalyzing",
            type: "checkbox",
            def: true
        },
        {
            name: "interpolation",
            type: "combobox",
            def: 1
        },
        {
            name: "sampleRadius",
            type: "static",
            def: 16
        },
        {
            name: "searchRadius",
            type: "static",
            def: 24
        },
        {
            name: "offset",
            type: "static",
            def: 64
        },
        {
            name: "subpixels",
            type: "combobox",
            def: 1
        },
        {
            name: "analysisFile",
            type: "textfield",
            def: ""
        },
        {
            name: "clipOffset",
            type: "numtextfield",
            def: 0
        },
        {
            name: "useBackTrackpoints",
            type: "checkbox",
            def: false
        },

        {
            name: "stabilizeYaw",
            type: "static",
            def: 100
        },
        {
            name: "stabilizePitch",
            type: "static",
            def: 100
        },
        {
            name: "stabilizeRoll",
            type: "static",
            def: 100
        },

        {
            name: "smoothYaw",
            type: "static",
            def: 120
        },
        {
            name: "smoothPitch",
            type: "static",
            def: 120
        },
        {
            name: "smoothRoll",
            type: "static",
            def: 120
        },

        {
            name: "timeBiasYaw",
            type: "static",
            def: 0
        },
        {
            name: "timeBiasPitch",
            type: "static",
            def: 0
        },
        {
            name: "timeBiasRoll",
            type: "static",
            def: 0
        }
    ]

    COMMON_FUNCTIONS

    Component.onCompleted: {
        defaultOnCompleted()
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
        Shotcut.Preset {
            id: preset
            parameters: ["sampleRadius", "searchRadius", "offset", "subpixels", "interpolation", "stabilizeYaw", "stabilizePitch", "stabilizeRoll", "smoothYaw", "smoothPitch", "smoothRoll", "timeBiasYaw", "timeBiasPitch", "timeBiasRoll"]
            Layout.columnSpan: 3
            onBeforePresetLoaded: {
               defaultBeforePresetLoaded()
            }
            onPresetSelected: {
                defaultPresetSelected()
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
        Shotcut.UndoButton {
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
        Shotcut.UndoButton {
            id: interpolationUndo
            onClicked: interpolationComboBox.currentIndex = 1
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
        Shotcut.SliderSpinner {
            id: sampleRadiusSlider
            minimumValue: 1
            maximumValue: 64
            suffix: ' px'
            decimals: 0
            stepSize: 1
            Layout.columnSpan: 2
            onValueChanged: updateProperty_sampleRadius(getPosition())
        }
        Shotcut.UndoButton {
            id: sampleRadiusUndo
            onClicked: sampleRadiusSlider.value = 16
        }

        Label {
            text: qsTr('Search Radius')
            Layout.alignment: Qt.AlignRight
        }
        Shotcut.SliderSpinner {
            id: searchRadiusSlider
            minimumValue: 1
            maximumValue: 128
            suffix: ' px'
            decimals: 0
            stepSize: 1
            Layout.columnSpan: 2
            onValueChanged: updateProperty_searchRadius(getPosition())
        }
        Shotcut.UndoButton {
            id: searchRadiusUndo
            onClicked: searchRadiusSlider.value = 24
        }

        Label {
            text: qsTr('Offset')
            Layout.alignment: Qt.AlignRight
        }
        Shotcut.SliderSpinner {
            id: offsetSlider
            minimumValue: 1
            maximumValue: 256
            suffix: ' px'
            decimals: 0
            stepSize: 1
            Layout.columnSpan: 2
            onValueChanged: updateProperty_offset(getPosition())
        }
        Shotcut.UndoButton {
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
        Shotcut.UndoButton {
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
        Shotcut.SliderSpinner {
            id: stabilizeYawSlider
            minimumValue: 0
            maximumValue: 100
            suffix: ' %'
            decimals: 0
            stepSize: 1
            Layout.columnSpan: 2
            onValueChanged: updateProperty_stabilizeYaw(getPosition())
        }
        Shotcut.UndoButton {
            id: stabilizeYawUndo
            onClicked: stabilizeYawSlider.value = 100
        }
        Label {
            text: qsTr('Smoothing')
            Layout.alignment: Qt.AlignRight
        }
        Shotcut.SliderSpinner {
            id: smoothYawSlider
            minimumValue: 1
            maximumValue: 300
            suffix: ' frames'
            decimals: 0
            stepSize: 1
            Layout.columnSpan: 2
            onValueChanged: updateProperty_smoothYaw(getPosition())
        }
        Shotcut.UndoButton {
            id: smoothYawUndo
            onClicked: smoothYawSlider.value = 120
        }
        Label {
            text: qsTr('Time Bias')
            Layout.alignment: Qt.AlignRight
        }
        Shotcut.SliderSpinner {
            id: timeBiasYawSlider
            minimumValue: -100
            maximumValue: 100
            suffix: ' %'
            decimals: 0
            stepSize: 1
            Layout.columnSpan: 2
            onValueChanged: updateProperty_timeBiasYaw(getPosition())
        }
        Shotcut.UndoButton {
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
        Shotcut.SliderSpinner {
            id: stabilizePitchSlider
            minimumValue: 0
            maximumValue: 100
            suffix: ' %'
            decimals: 0
            stepSize: 1
            Layout.columnSpan: 2
            onValueChanged: updateProperty_stabilizePitch(getPosition())
        }
        Shotcut.UndoButton {
            id: stabilizePitchUndo
            onClicked: stabilizePitchSlider.value = 100
        }
        Label {
            text: qsTr('Smoothing')
            Layout.alignment: Qt.AlignRight
        }
        Shotcut.SliderSpinner {
            id: smoothPitchSlider
            minimumValue: 1
            maximumValue: 300
            suffix: ' frames'
            decimals: 0
            stepSize: 1
            Layout.columnSpan: 2
            onValueChanged: updateProperty_smoothPitch(getPosition())
        }
        Shotcut.UndoButton {
            id: smoothPitchUndo
            onClicked: smoothPitchSlider.value = 120
        }
        Label {
            text: qsTr('Time Bias')
            Layout.alignment: Qt.AlignRight
        }
        Shotcut.SliderSpinner {
            id: timeBiasPitchSlider
            minimumValue: -100
            maximumValue: 100
            suffix: ' %'
            decimals: 0
            stepSize: 1
            Layout.columnSpan: 2
            onValueChanged: updateProperty_timeBiasPitch(getPosition())
        }
        Shotcut.UndoButton {
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
        Shotcut.SliderSpinner {
            id: stabilizeRollSlider
            minimumValue: 0
            maximumValue: 100
            suffix: ' %'
            decimals: 0
            stepSize: 1
            Layout.columnSpan: 2
            onValueChanged: updateProperty_stabilizeRoll(getPosition())
        }
        Shotcut.UndoButton {
            id: stabilizeRollUndo
            onClicked: stabilizeRollSlider.value = 100
        }
        Label {
            text: qsTr('Smoothing')
            Layout.alignment: Qt.AlignRight
        }
        Shotcut.SliderSpinner {
            id: smoothRollSlider
            minimumValue: 1
            maximumValue: 300
            suffix: ' frames'
            decimals: 0
            stepSize: 1
            Layout.columnSpan: 2
            onValueChanged: updateProperty_smoothRoll(getPosition())
        }
        Shotcut.UndoButton {
            id: smoothRollUndo
            onClicked: smoothRollSlider.value = 120
        }
        Label {
            text: qsTr('Time Bias')
            Layout.alignment: Qt.AlignRight
        }
        Shotcut.SliderSpinner {
            id: timeBiasRollSlider
            minimumValue: -100
            maximumValue: 100
            suffix: ' %'
            decimals: 0
            stepSize: 1
            Layout.columnSpan: 2
            onValueChanged: updateProperty_timeBiasRoll(getPosition())
        }
        Shotcut.UndoButton {
            id: timeBiasRollUndo
            onClicked: timeBiasRollSlider.value = 0
        }
    }

    COMMON_CONNECTIONS
}