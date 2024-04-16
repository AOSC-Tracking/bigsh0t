#define UPDATE_SIMPLE_KEYFRAMES(PROPERTY) $\
            if (filter.keyframeCount(#PROPERTY) <= 0)$\
                PROPERTY ## Start = PROPERTY ## Middle = PROPERTY ## End = filter.getDouble(#PROPERTY); $\

#define SETCONTROLS(PROPERTY) $\
        PROPERTY ## Slider.value = filter.getDouble(#PROPERTY, position); $\
        PROPERTY ## KeyframesButton.checked = filter.animateIn <= 0 && filter.animateOut <= 0 && filter.keyframeCount(#PROPERTY) > 0; $\

#define ONCOMPLETED(PROPERTY, DEFAULT_VALUE) $\
        if (filter.isNew) { $\
            filter.set(#PROPERTY + "Start", DEFAULT_VALUE); $\
        } else { $\
            PROPERTY##Middle = filter.getDouble(#PROPERTY, filter.animateIn); $\
            if (filter.animateIn > 0) $\
                PROPERTY##Start = filter.getDouble(#PROPERTY, 0); $\
            if (filter.animateOut > 0) $\
                PROPERTY##End = filter.getDouble(#PROPERTY, filter.duration - 1); $\
        } $\

#define ONPRESETSELECTED(PROPERTY) $\
                PROPERTY##Middle = filter.getDouble(#PROPERTY, filter.animateIn); $\
                if (filter.animateIn > 0) $\
                    PROPERTY##Start = filter.getDouble(#PROPERTY, 0); $\
                if (filter.animateOut > 0) $\
                    PROPERTY##End = filter.getDouble(#PROPERTY, filter.duration - 1); $
