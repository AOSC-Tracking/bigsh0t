import QtQuick 2.0
import org.shotcut.qml 1.0

Metadata {
    type: Metadata.Filter
    name: qsTr("Equirectangular to Rectilinear")
    mlt_service: "frei0r.eq_to_rect"
    objectName: "eq_to_rect"
    qml: "ui.qml"
    keyframes {
        allowAnimateIn: true
        allowAnimateOut: true
        simpleProperties: ['yaw', "pitch", "roll", "fov"]
        parameters: [
            Parameter {
                name: qsTr('Yaw')
                property: 'yaw'
                isSimple: true
                isCurve: true
                minimum: -360
                maximum: 360
            },
            Parameter {
                name: qsTr('Pitch')
                property: 'pitch'
                isSimple: true
                isCurve: true
                minimum: -180
                maximum: 180
            },
            Parameter {
                name: qsTr('Roll')
                property: 'roll'
                isSimple: true
                isCurve: true
                minimum: -180
                maximum: 180
            },
            Parameter {
                name: qsTr('FOV')
                property: 'fov'
                isSimple: true
                isCurve: true
                minimum: 0
                maximum: 180
            }
        ]
    }
}
