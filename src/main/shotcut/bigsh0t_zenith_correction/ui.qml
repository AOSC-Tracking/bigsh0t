/* SPDX-License-Identifier: GPL-2.0-or-later */
#include "../shotcut_ui.qml"

import QtQuick 2.1
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.0
import QtQuick.Dialogs 1.1
import Shotcut.Controls 1.0 as Shotcut


Shotcut.KeyframableFilter {
    width: 350
    height: 125 /* 5 rows of 25 pixels */

    keyframableParameters: []
    startValues: []
    middleValues: []
    endValues: []

    property var allParameters: [
        {
            name: "interpolation",
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
            name: "enableSmoothYaw",
            type: "checkbox",
            def: false
        },
        {
            name: "smoothYaw",
            type: "static",
            def: 120
        },
        {
            name: "timeBiasYaw",
            type: "static",
            def: 0
        }
    ]

    COMMON_FUNCTIONS

    Component.onCompleted: {
        defaultOnCompleted()
    }

    UPDATE_PROPERTY_COMBOBOX(interpolation)
    UPDATE_PROPERTY_TEXTFIELD(analysisFile)
    UPDATE_PROPERTY_TEXTFIELD_NUM(clipOffset)
    UPDATE_PROPERTY_CHECKBOX(enableSmoothYaw)
    UPDATE_PROPERTY_STATIC(smoothYaw)
    UPDATE_PROPERTY_STATIC(timeBiasYaw)

    FileDialog {
        id: selectAnalysisFile
        title: "Video file for zenith correction"
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
        Shotcut.Preset {
            id: preset
            parameters: ["interpolation"]
            Layout.columnSpan: 3
            onBeforePresetLoaded: {
                defaultBeforePresetLoaded()
            }
            onPresetSelected: {
                defaultPresetSelected()
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
        Shotcut.SliderSpinner {
            id: smoothYawSlider
            minimumValue: 1
            maximumValue: 300
            suffix: ' frames'
            decimals: 0
            stepSize: 1
            Layout.columnSpan: 2
            onValueChanged: updateProperty_smoothYaw()
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
            onValueChanged: updateProperty_timeBiasYaw()
        }
        Shotcut.UndoButton {
            id: timeBiasYawUndo
            onClicked: timeBiasYawSlider.value = 0
        }
    }

    COMMON_CONNECTIONS
}