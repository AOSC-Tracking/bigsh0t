/* SPDX-License-Identifier: GPL-2.0-or-later */
#define PROPERTY_VARIABLES(PROPERTY) property double PROPERTY ## Start : 0.0;\
    property double PROPERTY ## Middle : 0.0;\
    property double PROPERTY ## End : 0.0;

#define PROPERTY_VARIABLES_STATIC(PROPERTY) property double PROPERTY ## Value : 0.0;
    
#define PROPERTY_VARIABLES_COMBOBOX(PROPERTY) property int PROPERTY ## Value : 0;

#define PROPERTY_VARIABLES_CHECKBOX(PROPERTY) property bool PROPERTY ## Value : false;

#define PROPERTY_VARIABLES_LABEL(PROPERTY) property alias PROPERTY ## Value : PROPERTY ## Label.text;

#define PROPERTY_VARIABLES_TEXTFIELD(PROPERTY) property string PROPERTY ## Value : "";

#define PROPERTY_CONNECTIONS(PROPERTY) Connections {\
        target: filter; \
        onChanged: setControls(); \
        onInChanged: { updateProperty_ ## PROPERTY (null); } \
        onOutChanged: { updateProperty_ ## PROPERTY (null); } \
        onAnimateInChanged: { updateProperty_ ## PROPERTY (null); } \
        onAnimateOutChanged: { updateProperty_ ## PROPERTY (null); } \
    }

#define PROPERTY_CONNECTIONS_STATIC(PROPERTY) Connections {\
        target: filter; \
        onChanged: setControls(); \
        onInChanged: { updateProperty_ ## PROPERTY (null); } \
        onOutChanged: { updateProperty_ ## PROPERTY (null); } \
        onAnimateInChanged: { updateProperty_ ## PROPERTY (null); } \
        onAnimateOutChanged: { updateProperty_ ## PROPERTY (null); } \
    }

#define PROPERTY_CONNECTIONS_COMBOBOX(PROPERTY) Connections {\
        target: filter; \
        onChanged: setControls(); \
        onInChanged: { updateProperty_ ## PROPERTY (); } \
        onOutChanged: { updateProperty_ ## PROPERTY (); } \
        onAnimateInChanged: { updateProperty_ ## PROPERTY (); } \
        onAnimateOutChanged: { updateProperty_ ## PROPERTY (); } \
    }

#define PROPERTY_CONNECTIONS_CHECKBOX(PROPERTY) Connections {\
        target: filter; \
        onChanged: setControls(); \
    }

#define PROPERTY_CONNECTIONS_LABEL(PROPERTY) Connections {\
        target: filter; \
        onChanged: setControls(); \
    }

#define PROPERTY_CONNECTIONS_TEXTFIELD(PROPERTY) Connections {\
        target: filter; \
        onChanged: setControls(); \
    }

#define UPDATE_PROPERTY(PROPERTY) function updateProperty_ ## PROPERTY (position) {\
        if (blockUpdate) return;\
        var value = PROPERTY ## Slider.value;\
        \
        if (position !== null) {\
            if (position <= 0 && filter.animateIn > 0) {\
                PROPERTY ## Start = value;\
            } else if (position >= filter.duration - 1 && filter.animateOut > 0) {\
                PROPERTY ## End = value;\
            } else {\
                PROPERTY ## Middle = value;\
            }\
        }\
        \
        if (filter.animateIn > 0 || filter.animateOut > 0) {\
            filter.resetProperty(#PROPERTY);\
            PROPERTY ## KeyframesButton.checked = false;\
            if (filter.animateIn > 0) {\
                filter.set(#PROPERTY, PROPERTY ## Start, 0);\
                filter.set(#PROPERTY, PROPERTY ## Middle, filter.animateIn - 1);\
            }\
            if (filter.animateOut > 0) {\
                filter.set(#PROPERTY, PROPERTY ## Middle, filter.duration - filter.animateOut);\
                filter.set(#PROPERTY, PROPERTY ## End, filter.duration - 1);\
            }\
        } else if (!PROPERTY ## KeyframesButton.checked) {\
            filter.resetProperty(#PROPERTY);\
            filter.set(#PROPERTY, PROPERTY ## Middle);\
        } else if (position !== null) {\
            filter.set(#PROPERTY, value, position);\
        }\
    }

#define UPDATE_PROPERTY_STATIC(PROPERTY) function updateProperty_ ## PROPERTY (position) {\
        if (blockUpdate) return;\
        var value = PROPERTY ## Slider.value;\
		filter.set(#PROPERTY, value);\
    }

#define UPDATE_PROPERTY_COMBOBOX(PROPERTY) function updateProperty_ ## PROPERTY () {\
        if (blockUpdate) return;\
        var value = PROPERTY ## ComboBox.currentIndex;\
        filter.set(#PROPERTY, value);\
    }
    
#define UPDATE_PROPERTY_CHECKBOX(PROPERTY) function updateProperty_ ## PROPERTY () {\
        if (blockUpdate) return;\
        var value = PROPERTY ## CheckBox.checked;\
        filter.set(#PROPERTY, value);\
    }

#define UPDATE_PROPERTY_TEXTFIELD(PROPERTY) function updateProperty_ ## PROPERTY () {\
        if (blockUpdate) return;\
        var value = PROPERTY ## TextField.text;\
        filter.set(#PROPERTY, value);\
    }

#define KEYFRAME_BUTTON(PROPERTY) KeyframesButton {\
            id: PROPERTY ## KeyframesButton;\
            checked: filter.animateIn <= 0 && filter.animateOut <= 0 && filter.keyframeCount(#PROPERTY) > 0;\
            onToggled: {\
                var value = PROPERTY ## Slider.value;\
                if (checked) {\
                    blockUpdate = true;\
                    if (filter.animateIn > 0 || filter.animateOut > 0) {\
                        filter.resetProperty(#PROPERTY);\
                        PROPERTY ## Slider.enabled = true;\
                    }\
                    filter.clearSimpleAnimation(#PROPERTY);\
                    blockUpdate = false;\
                    filter.set(#PROPERTY, value, getPosition());\
                } else {\
                    filter.resetProperty(#PROPERTY);\
                    filter.set(#PROPERTY, value);\
                }\
            }\
        }
        
#define ON_COMPLETED_COMBOBOX(PROPERTY,DEFAULT_VALUE) if (filter.isNew) {\
    filter.set(#PROPERTY, DEFAULT_VALUE); \
} else { \
    PROPERTY ## Value = filter.get(#PROPERTY); \
}

#define ON_COMPLETED_CHECKBOX(PROPERTY,DEFAULT_VALUE) if (filter.isNew) {\
    filter.set(#PROPERTY, DEFAULT_VALUE); \
} else { \
    PROPERTY ## Value = filter.get(#PROPERTY); \
}

#define ON_COMPLETED_TEXTFIELD(PROPERTY,DEFAULT_VALUE) if (filter.isNew) {\
    filter.set(#PROPERTY, DEFAULT_VALUE); \
} else { \
    PROPERTY ## Value = filter.get(#PROPERTY); \
}

#define ON_COMPLETED_STATIC(PROPERTY,DEFAULT_VALUE) if (filter.isNew) {\
    filter.set(#PROPERTY, DEFAULT_VALUE); \
} else { \
    PROPERTY ## Value = filter.getDouble(#PROPERTY);\
}

#define ON_COMPLETED(PROPERTY,DEFAULT_VALUE) if (filter.isNew) {\
    filter.set(#PROPERTY, DEFAULT_VALUE); \
} else { \
    PROPERTY ## Middle = filter.getDouble(#PROPERTY, filter.animateIn); \
    if (filter.animateIn > 0) { \
        PROPERTY ## Start = filter.getDouble(#PROPERTY, 0); \
    } \
    if (filter.animateOut > 0) { \
        PROPERTY ## End = filter.getDouble(#PROPERTY, filter.duration - 1); \
    } \
}

#define LOAD_PRESET(PROPERTY) PROPERTY ## Middle = filter.getDouble(#PROPERTY, filter.animateIn); \
if (filter.animateIn > 0) { \
    PROPERTY ## Start = filter.getDouble(#PROPERTY, 0); \
} \
if (filter.animateOut > 0) { \
    PROPERTY ## End = filter.getDouble(#PROPERTY, filter.duration - 1); \
}

#define LOAD_PRESET_STATIC(PROPERTY) PROPERTY ## Value = filter.getDouble(#PROPERTY);

#define LOAD_PRESET_COMBOBOX(PROPERTY) PROPERTY ## Value = filter.get(#PROPERTY);

#define LOAD_PRESET_TEXTFIELD(PROPERTY) PROPERTY ## Value = filter.get(#PROPERTY);

#define SET_CONTROLS(PROPERTY) PROPERTY ## Slider.value = filter.getDouble(#PROPERTY, position)

#define SET_CONTROLS_STATIC(PROPERTY) PROPERTY ## Slider.value = filter.getDouble(#PROPERTY)

#define SET_CONTROLS_COMBOBOX(PROPERTY) PROPERTY ## ComboBox.currentIndex = filter.get(#PROPERTY)

#define SET_CONTROLS_CHECKBOX(PROPERTY) PROPERTY ## CheckBox.checked = filter.get(#PROPERTY) == '1';

#define SET_CONTROLS_TEXTFIELD(PROPERTY) PROPERTY ## TextField.text = filter.get(#PROPERTY)

