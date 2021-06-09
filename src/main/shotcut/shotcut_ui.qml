/* SPDX-License-Identifier: GPL-2.0-or-later */
#define COMMON_FUNCTIONS \
    function isKeyframeButtonChecked(control){$\
        return filter.animateIn <= 0 && filter.animateOut <= 0 && filter.keyframeCount(control) > 0;$\
    }$\
    $\
    function setSimpleControl(parameter, controlSlider, controlKeyframeButton) {$\
        controlSlider.value = filter.getDouble(parameter, getPosition());$\
        controlKeyframeButton.checked = isKeyframeButtonChecked(parameter);$\
    }$\
    $\
    function setSimpleControlStatic(parameter, controlSlider) {$\
        controlSlider.value = filter.getDouble(parameter, getPosition());$\
    }$\
    $\
    function setSimpleComboBox(parameter, controlComboBox) {$\
        controlComboBox.currentIndex = filter.get(parameter);$\
    }$\
    $\
    function setSimpleCheckBox(parameter, controlCheckBox) {$\
        controlCheckBox.checked = filter.get(parameter) == "1";$\
    }$\
    $\
    function setSimpleTextField(parameter, controlTextField) {$\
        controlTextField.text = filter.get(parameter);$\
    }$\
    $\
    function setSimpleNumTextField(parameter, controlTextField) {$\
        controlTextField.text = filter.getDouble(parameter).toFixed(4);$\
    }$\
    $\
    function getFrameRate() {$\
        return producer.getDouble("meta.media.frame_rate_num", getPosition()) / producer.getDouble("meta.media.frame_rate_den", getPosition());$\
    }$\
    $\
    function getClipOffset() {$\
        return filter.in;$\
    }$\
    function getKeyframesButton(param) {$\
        return this["prop_" + param.name + "KeyframesButton"];$\
    }$\
    $\
    function getControl(param) {$\
        if (param.control != null && param.control !== undefined) {$\
            return this["prop_" + param.control];$\
        } else if (param.type == "simple" || param.type == "static") {$\
            return this["prop_" + param.name + "Slider"];$\
        } else if (param.type == "checkbox") {$\
            return this["prop_" + param.name + "CheckBox"];$\
        } else if (param.type == "combobox") {$\
            return this["prop_" + param.name + "ComboBox"];$\
        } else if (param.type == "textfield" || param.type == "numtextfield") {$\
            return this["prop_" + param.name + "TextField"];$\
        } else {$\
            return null;$\
        }$\
    }$\
    $\
    function enableControls(enabled) {$\
        for (var i = 0; i < allParameters.length; ++i) {$\
            var control = getControl(allParameters[i]);$\
            control.enabled = enabled;$\
        }$\
    }$\
    $\
    function defaultBeforePresetLoaded() {$\
        for (var i in preset.parameters) {$\
            filter.resetProperty(preset.parameters[i]);$\
        }$\
        resetSimpleKeyframes();$\
    }$\
    $\
    function defaultPresetSelected() {$\
        initializeSimpleKeyframes();$\
        setControls();$\
    }$\
    $\
    function setControlsForAllParameters() {$\
        console.log("START");$\
        console.log(allParameters.length);$\
        for (var i = 0; i < allParameters.length; ++i) {$\
            console.log(i);$\
            var param = allParameters[i];$\
            var control = getControl(param);$\
            console.log(param.name);$\
            console.log(param.type);$\
            console.log(control);$\
            if (param.type == "simple") {$\
                setSimpleControl(param.name, control, getKeyframesButton(param));$\
            } else if (param.type == "static") {$\
                setSimpleControlStatic(param.name, control);$\
            } else if (param.type == "combobox") {$\
                setSimpleComboBox(param.name, control);$\
            } else if (param.type == "checkbox") {$\
                setSimpleCheckBox(param.name, control);$\
            } else if (param.type == "textfield") {$\
                setSimpleTextField(param.name, control);$\
            } else if (param.type == "numtextfield") {$\
                setSimpleNumTextField(param.name, control);$\
            }$\
        }$\
    }$\
    function setControls() {$\
        blockUpdate = true;$\
        setControlsForAllParameters();$\
        blockUpdate = false;$\
        enableControls(isSimpleKeyframesActive());$\
    }$\
    $\
    function defaultOnCompleted() {$\
        if (filter.isNew) {$\
            for (var i = 0; i < allParameters.length; ++i) {$\
                var param = allParameters[i];$\
                console.log(param.name);$\
                if (param.def != null) {$\
                    var control = getControl(param);$\
                    filter.set(param.name, param.def);$\
                }$\
            }$\
            filter.savePreset(preset.parameters);$\
        }$\
        setControls();$\
    }$\
    $\
    function updateSimpleKeyframes() {$\
        setControlsForAllParameters();$\
    }$\


#define COMMON_CONNECTIONS \
    Connections {$\
        target: filter$\
        onInChanged: updateSimpleKeyframes();$\
        onOutChanged: updateSimpleKeyframes();$\
        onAnimateInChanged: updateSimpleKeyframes();$\
        onAnimateOutChanged: updateSimpleKeyframes();$\
        onChanged: setControls();$\
    }$\
    $\
    Connections {$\
        target: producer;$\
        onPositionChanged: setControls();$\
    }$\


#define UPDATE_PROPERTY(PROPERTY) function updateProperty_ ## PROPERTY () {$\
        if (!blockUpdate) {$\
            updateFilter(#PROPERTY, PROPERTY ## Slider.value, PROPERTY ## KeyframesButton, getPosition());$\
        }$\
    }$\
    property alias prop_ ## PROPERTY ## Slider : PROPERTY ## Slider; $\
    property alias prop_ ## PROPERTY ## KeyframesButton : PROPERTY ## KeyframesButton; $\

#define UPDATE_PROPERTY_STATIC(PROPERTY) function updateProperty_ ## PROPERTY () {$\
        if (!blockUpdate) {$\
            filter.set(#PROPERTY, PROPERTY ## Slider.value);$\
        }$\
    }$\
    property alias prop_ ## PROPERTY ## Slider : PROPERTY ## Slider; $\

#define UPDATE_PROPERTY_COMBOBOX(PROPERTY) function updateProperty_ ## PROPERTY () {$\
        if (!blockUpdate) {$\
            filter.set(#PROPERTY, PROPERTY ## ComboBox.currentIndex);$\
        }$\
    }$\
    property alias prop_ ## PROPERTY ## ComboBox : PROPERTY ## ComboBox; $\

#define UPDATE_PROPERTY_CHECKBOX(PROPERTY) function updateProperty_ ## PROPERTY () {$\
        if (!blockUpdate) {$\
            filter.set(#PROPERTY, PROPERTY ## CheckBox.checked);$\
        }$\
    }$\
    property alias prop_ ## PROPERTY ## CheckBox : PROPERTY ## CheckBox; $\


#define UPDATE_PROPERTY_TEXTFIELD(PROPERTY) function updateProperty_ ## PROPERTY () {$\
        if (!blockUpdate) {$\
            filter.set(#PROPERTY, PROPERTY ## TextField.text);$\
        }$\
    }$\
    property alias prop_ ## PROPERTY ## TextField : PROPERTY ## TextField; $\


#define UPDATE_PROPERTY_TEXTFIELD_NUM(PROPERTY) function updateProperty_ ## PROPERTY () {$\
        if (!blockUpdate) {$\
            filter.set(#PROPERTY, parseFloat(PROPERTY ## TextField.text));$\
        }$\
    }$\
    property alias prop_ ## PROPERTY ## TextField : PROPERTY ## TextField; $\


#define KEYFRAME_BUTTON(PROPERTY) Shotcut.KeyframesButton {$\
            id: PROPERTY ## KeyframesButton;$\
            checked: isKeyframeButtonChecked(#PROPERTY);$\
            onToggled: {$\
                enableControls(true); $\
                toggleKeyframes(checked, #PROPERTY, PROPERTY ## Slider.value); $\
            }$\
        }

#define ON_COMPLETED_COMBOBOX(PROPERTY,DEFAULT_VALUE) if (filter.isNew) {$\
    filter.set(#PROPERTY, DEFAULT_VALUE); $\
}

#define ON_COMPLETED_CHECKBOX(PROPERTY,DEFAULT_VALUE) if (filter.isNew) {$\
    filter.set(#PROPERTY, DEFAULT_VALUE); $\
}

#define ON_COMPLETED_TEXTFIELD(PROPERTY,DEFAULT_VALUE) if (filter.isNew) {$\
    filter.set(#PROPERTY, DEFAULT_VALUE); $\
}

#define ON_COMPLETED_TEXTFIELD_NUM(PROPERTY,DEFAULT_VALUE) if (filter.isNew) {$\
    filter.set(#PROPERTY, DEFAULT_VALUE); $\
}

#define ON_COMPLETED_STATIC(PROPERTY,DEFAULT_VALUE) if (filter.isNew) {$\
    filter.set(#PROPERTY, DEFAULT_VALUE); $\
}

#define ON_COMPLETED(PROPERTY,DEFAULT_VALUE) if (filter.isNew) {$\
    filter.set(#PROPERTY, DEFAULT_VALUE); $\
}

#define SET_CONTROLS(PROPERTY) setSimpleControl(#PROPERTY, PROPERTY ## Slider, PROPERTY ## KeyframesButton);

#define SET_CONTROLS_STATIC(PROPERTY) PROPERTY ## Slider.value = filter.getDouble(#PROPERTY)

#define SET_CONTROLS_COMBOBOX(PROPERTY) setSimpleComboBox(#PROPERTY, PROPERTY ## ComboBox);

#define SET_CONTROLS_CHECKBOX(PROPERTY) setSimpleCheckBox(#PROPERTY, PROPERTY ## CheckBox);

#define SET_CONTROLS_TEXTFIELD(PROPERTY) PROPERTY ## TextField.text = filter.get(#PROPERTY)

#define SET_CONTROLS_TEXTFIELD_NUM(PROPERTY) PROPERTY ## TextField.text = filter.getDouble(#PROPERTY).toFixed(4);

#define DEG_SLIDER_SPINNER_PROPERTIES spinnerWidth: 120; $\
            suffix: ' deg'; $\
            decimals: 3; $\
            stepSize: 1;

// End of shotcut_ui.qml