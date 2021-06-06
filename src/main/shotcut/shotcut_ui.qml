/* SPDX-License-Identifier: GPL-2.0-or-later */
#define COMMON_FUNCTIONS \
    function isKeyframeButtonChecked(control){$\
        return filter.animateIn <= 0 && filter.animateOut <= 0 && filter.keyframeCount(control) > 0$\
    }$\
    $\
    function setSimpleControl(parameter, controlSlider, controlKeyframeButton) {$\
        controlSlider.value = filter.getDouble(parameter, getPosition())$\
        controlKeyframeButton.checked = isKeyframeButtonChecked(parameter)$\
    }$\
    $\
    function setSimpleControlStatic(parameter, controlSlider) {$\
        controlSlider.value = filter.getDouble(parameter, getPosition())$\
    }$\
    $\
    function setSimpleComboBox(parameter, controlComboBox) {$\
        controlComboBox.currentIndex = filter.get(parameter)$\
    }$\
    $\
    function setSimpleCheckBox(parameter, controlCheckBox) {$\
        controlCheckBox.checked = filter.get(parameter) == "1"$\
    }$\
    $\
    function setSimpleTextField(parameter, controlTextField) {$\
        controlTextField.text = filter.get(parameter)$\
    }$\
    $\
    function setSimpleNumTextField(parameter, controlTextField) {$\
        controlTextField.text = filter.getDouble(parameter).toFixed(4);$\
    }$\
    $\
    function getFrameRate() {$\
        return producer.getDouble("meta.media.frame_rate_num", getPosition()) / producer.getDouble("meta.media.frame_rate_den", getPosition())$\
    }$\
    $\
    function getClipOffset() {$\
        return filter.in$\
    }$\
    function getKeyframesButton(param) {$\
        return this[param.name + "KeyframesButton"]$\
    }$\
    $\
    function getControl(param) {$\
        if (param.control != null && param.control !== undefined) {$\
            return this[param.control]$\
        } else if (param.type == "simple" || param.type == "static") {$\
            return this[param.name + "Slider"]$\
        } else if (param.type == "checkbox") {$\
            return this[param.name + "CheckBox"]$\
        } else if (param.type == "combobox") {$\
            return this[param.name + "ComboBox"]$\
        } else if (param.type == "textfield" || param.type == "numtextfield") {$\
            return this[param.name + "TextField"]$\
        } else {$\
            return null$\
        }$\
    }$\
    $\
    function enableControls(enabled) {$\
        for (var i in allParameters) {$\
            var control = getControl(allParameters[i])$\
            control.enabled = enabled$\
        }$\
    }$\
    $\
    function defaultBeforePresetLoaded() {$\
        for (var i in preset.parameters) {$\
            filter.resetProperty(preset.parameters[i])$\
        }$\
        resetSimpleKeyframes()$\
    }$\
    $\
    function defaultPresetSelected() {$\
        initializeSimpleKeyframes()$\
        setControls()$\
    }$\
    function setControlsForAllParameters() {$\
        for (var i in allParameters) {$\
            var param = allParameters[i];$\
            var control = getControl(param)$\
            if (param.type == "simple") {$\
                setSimpleControl(param.name, control, getKeyframesButton(param))$\
            } else if (param.type == "static") {$\
                setSimpleControlStatic(param.name, control)$\
            } else if (param.type == "combobox") {$\
                setSimpleComboBox(param.name, control)$\
            } else if (param.type == "checkbox") {$\
                setSimpleCheckBox(param.name, control)$\
            } else if (param.type == "textfield") {$\
                setSimpleTextField(param.name, control)$\
            } else if (param.type == "numtextfield") {$\
                setSimpleNumTextField(param.name, control)$\
            }$\
        }$\
    }$\
    function setControls() {$\
        blockUpdate = true$\
        setControlsForAllParameters()$\
        blockUpdate = false$\
        enableControls(isSimpleKeyframesActive())$\
    }$\
    $\
    function defaultOnCompleted() {$\
        if (filter.isNew) {$\
            for (var i in allParameters) {$\
                var param = allParameters[i];$\
                if (param.def != null) {$\
                    var control = getControl(param)$\
                    filter.set(param.name, param.def)$\
                }$\
            }$\
            filter.savePreset(preset.parameters)$\
        }$\
        setControls()$\
    }$\
    function updateSimpleKeyframes() {$\
        setControlsForAllParameters()$\
    }$\


#define COMMON_CONNECTIONS \
    Connections {$\
        target: filter$\
        onInChanged: updateSimpleKeyframes()$\
        onOutChanged: updateSimpleKeyframes()$\
        onAnimateInChanged: updateSimpleKeyframes()$\
        onAnimateOutChanged: updateSimpleKeyframes()$\
        onChanged: setControls()$\
    }$\
    $\
    Connections {$\
        target: producer$\
        onPositionChanged: setControls()$\
    }$\


#define PROPERTY_VARIABLES(PROPERTY)

#define PROPERTY_VARIABLES_STATIC(PROPERTY)

#define PROPERTY_VARIABLES_COMBOBOX(PROPERTY)

#define PROPERTY_VARIABLES_CHECKBOX(PROPERTY)

#define PROPERTY_VARIABLES_LABEL(PROPERTY)

#define PROPERTY_VARIABLES_TEXTFIELD(PROPERTY)

#define PROPERTY_VARIABLES_TEXTFIELD_NUM(PROPERTY)

#define PROPERTY_CONNECTIONS(PROPERTY)

#define PROPERTY_CONNECTIONS_STATIC(PROPERTY)

#define PROPERTY_CONNECTIONS_COMBOBOX(PROPERTY)

#define PROPERTY_CONNECTIONS_CHECKBOX(PROPERTY)

#define PROPERTY_CONNECTIONS_LABEL(PROPERTY)

#define PROPERTY_CONNECTIONS_TEXTFIELD(PROPERTY)

#define PROPERTY_CONNECTIONS_TEXTFIELD_NUM(PROPERTY)

#define UPDATE_PROPERTY(PROPERTY) function updateProperty_ ## PROPERTY () {$\
        if (!blockUpdate) {$\
            updateFilter(#PROPERTY, PROPERTY ## Slider.value, PROPERTY ## KeyframesButton, getPosition())$\
        }$\
    }$\

#define UPDATE_PROPERTY_STATIC(PROPERTY) function updateProperty_ ## PROPERTY () {$\
        if (!blockUpdate) {$\
            updateFilter(#PROPERTY, PROPERTY ## Slider.value, PROPERTY ## KeyframesButton, getPosition())$\
        }$\
    }$\

#define UPDATE_PROPERTY_COMBOBOX(PROPERTY) function updateProperty_ ## PROPERTY () {$\
        if (!blockUpdate) {$\
            filter.set(#PROPERTY, PROPERTY ## ComboBox.currentIndex);$\
        }$\
    }$\

#define UPDATE_PROPERTY_CHECKBOX(PROPERTY) function updateProperty_ ## PROPERTY () {$\
        if (!blockUpdate) {$\
            filter.set(#PROPERTY, PROPERTY ## CheckBox.checked);$\
        }$\
    }$\


#define UPDATE_PROPERTY_TEXTFIELD(PROPERTY) function updateProperty_ ## PROPERTY () {$\
        if (!blockUpdate) {$\
            filter.set(#PROPERTY, PROPERTY ## TextField.text);$\
        }$\
    }$\


#define UPDATE_PROPERTY_TEXTFIELD_NUM(PROPERTY) function updateProperty_ ## PROPERTY () {$\
        if (!blockUpdate) {$\
            filter.set(#PROPERTY, parseFloat(PROPERTY ## TextField.text));$\
        }$\
    }$\


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

#define LOAD_PRESET(PROPERTY) PROPERTY ## Middle = filter.getDouble(#PROPERTY, filter.animateIn); $\
if (filter.animateIn > 0) { $\
    PROPERTY ## Start = filter.getDouble(#PROPERTY, 0); $\
} $\
if (filter.animateOut > 0) { $\
    PROPERTY ## End = filter.getDouble(#PROPERTY, filter.duration - 1); $\
}

#define LOAD_PRESET_STATIC(PROPERTY) PROPERTY ## Value = filter.getDouble(#PROPERTY);

#define LOAD_PRESET_COMBOBOX(PROPERTY) PROPERTY ## Value = filter.get(#PROPERTY);

#define LOAD_PRESET_TEXTFIELD(PROPERTY) PROPERTY ## Value = filter.get(#PROPERTY);

#define LOAD_PRESET_TEXTFIELD_NUM(PROPERTY) PROPERTY ## Value = filter.get(#PROPERTY);

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