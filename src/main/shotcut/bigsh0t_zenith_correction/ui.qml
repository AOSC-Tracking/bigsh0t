import QtQuick
import QtQuick.Controls
import QtQuick.Dialogs
import QtQuick.Layouts
import Shotcut.Controls as Shotcut
import org.shotcut.qml as Shotcut

Item {
    property bool blockUpdate: true
    property int interpolationValue: 0
    property string analysisFileValue: ""
    property bool enableSmoothYawValue: false
    property double smoothYawValue: 0
    property double timeBiasYawValue: 0
    property double clipOffsetValue: 0

    function setControls() {
        var position = getPosition();
        blockUpdate = true;
        interpolationComboBox.currentIndex = filter.get("interpolation");
        analysisFileTextField.text = filter.get("analysisFile");
        enableSmoothYawCheckBox.checked = filter.get("enableSmoothYaw") == '1';
        smoothYawSlider.value = filter.getDouble("smoothYaw");
        timeBiasYawSlider.value = filter.getDouble("timeBiasYaw");
        clipOffsetTextField.text = filter.getDouble("clipOffset").toFixed(4);
        blockUpdate = false;
    }

    function updateProperty_interpolation() {
        if (blockUpdate)
            return;
        var value = interpolationComboBox.currentIndex;
        filter.set("interpolation", value);
    }

    function updateProperty_analysisFile() {
        if (blockUpdate)
            return;
        var value = analysisFileTextField.text;
        filter.set("analysisFile", value);
    }

    function updateProperty_enableSmoothYaw() {
        if (blockUpdate)
            return;
        var value = enableSmoothYawCheckBox.checked;
        filter.set("enableSmoothYaw", value);
    }

    function updateProperty_smoothYaw(position) {
        if (blockUpdate)
            return;
        var value = smoothYawSlider.value;
        filter.set("smoothYaw", value);
    }

    function updateProperty_timeBiasYaw(position) {
        if (blockUpdate)
            return;
        var value = timeBiasYawSlider.value;
        filter.set("timeBiasYaw", value);
    }

    function updateProperty_clipOffset(position) {
        if (blockUpdate)
            return;
        var value = parseFloat(clipOffsetTextField.text);
        filter.set("clipOffset", value);
    }

    function getPosition() {
        return Math.max(producer.position - (filter.in - producer.in), 0);
    }

    function getFrameRate() {
        return producer.getDouble("meta.media.frame_rate_num", getPosition()) / producer.getDouble("meta.media.frame_rate_den", getPosition());
    }

    function getClipOffset() {
        return filter.in;
    }

    function onClipOffsetUndo() {
        clipOffsetTextField.text = (getClipOffset() / getFrameRate()).toFixed(4);
        updateProperty_clipOffset();
    }

    width: 350
    height: 625
    Component.onCompleted: {
        if (filter.isNew)
            filter.set("interpolation", 1);
        else
            interpolationValue = filter.get("interpolation");
        if (filter.isNew)
            filter.set("analysisFile", "");
        else
            analysisFileValue = filter.get("analysisFile");
        if (filter.isNew)
            filter.set("enableSmoothYaw", false);
        else
            enableSmoothYawValue = filter.get("enableSmoothYaw");
        if (filter.isNew)
            filter.set("smoothYaw", 120);
        else
            smoothYawValue = filter.getDouble("smoothYaw");
        if (filter.isNew)
            filter.set("timeBiasYaw", 0);
        else
            timeBiasYawValue = filter.getDouble("timeBiasYaw");
        if (filter.isNew)
            filter.set("clipOffset", 0);
        else
            clipOffsetValue = filter.getDouble("clipOffset");
        if (filter.isNew)
            filter.savePreset(preset.parameters);
        setControls();
    }

    Shotcut.File {
        id: analysisFile
    }

    Shotcut.FileDialog {
        id: selectAnalysisFile

        title: qsTr("File for zenith correction")
        nameFilters: ['Theta video (*.mp4)', 'All Files (*)']
        onAccepted: {
            analysisFile.url = selectAnalysisFile.selectedFile;
            analysisFileTextField.text = analysisFile.filePath;
            updateProperty_analysisFile();
            settings.openPath = analysisFile.path;
        }
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

            parameters: ["interpolation", "enableSmoothYaw", "smoothYaw", "timeBiasYaw"]
            Layout.columnSpan: 3
            onBeforePresetLoaded: {
                filter.resetProperty("interpolation");
                filter.resetProperty("enableSmoothYaw");
                filter.resetProperty("smoothYaw");
                filter.resetProperty("timeBiasYaw");
                filter.resetProperty("clipOffset");
            }
            onPresetSelected: {
                interpolationValue = filter.get("interpolation");
                enableSmoothYawValue = filter.getDouble("enableSmoothYaw");
                smoothYawValue = filter.getDouble("smoothYaw");
                timeBiasYawValue = filter.getDouble("timeBiasYaw");
                clipOffsetValue = filter.get("clipOffset");
                setControls(null);
            }
        }

        Label {
            text: qsTr('File')
            Layout.alignment: Qt.AlignRight
        }

        TextField {
            id: analysisFileTextField

            text: qsTr("")
            Layout.columnSpan: 2
            Layout.fillWidth: true
            Layout.alignment: Qt.AlignLeft
            selectByMouse: true
            onEditingFinished: updateProperty_analysisFile()
        }

        Shotcut.Button {
            icon.name: 'document-open'
            icon.source: 'qrc:///icons/oxygen/32x32/actions/document-open.png'
            implicitWidth: 20
            implicitHeight: 20
            onClicked: selectAnalysisFile.open()

            Shotcut.HoverTip {
                text: qsTr('Browse...')
            }
        }

        Label {
            text: qsTr('Start Offset')
            Layout.alignment: Qt.AlignRight
        }

        TextField {
            id: clipOffsetTextField

            selectByMouse: true
            onEditingFinished: updateProperty_clipOffset()
        }

        Label {
            text: qsTr('seconds')
            Layout.fillWidth: true
        }

        Shotcut.UndoButton {
            id: clipOffsetUndo

            onClicked: onClipOffsetUndo()
        }

        Label {
            text: qsTr('Interpolation')
            Layout.alignment: Qt.AlignRight
        }

        Shotcut.ComboBox {
            id: interpolationComboBox

            currentIndex: 0
            model: ["Nearest-neighbor", "Bilinear"]
            Layout.columnSpan: 2
            onCurrentIndexChanged: updateProperty_interpolation()
        }

        Shotcut.UndoButton {
            id: interpolationUndo

            onClicked: interpolationComboBox.currentIndex = 0
        }

        Label {
            text: qsTr('Yaw')
            Layout.alignment: Qt.AlignRight
        }

        CheckBox {
            id: enableSmoothYawCheckBox

            text: qsTr('Smooth yaw instead of locking it')
            checked: false
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

        Item {
            Layout.fillHeight: true
        }
    }

    Connections {
        function onChanged() {
            setControls();
        }

        function onInChanged() {
            setControls();
        }

        function onOutChanged() {
            setControls();
        }

        function onAnimateInChanged() {
            setControls();
        }

        function onAnimateOutChanged() {
            setControls();
        }

        function onPropertyChanged(name) {
            setControls();
        }

        target: filter
    }

    Connections {
        function onPositionChanged() {
            setControls();
        }

        target: producer
    }
}
