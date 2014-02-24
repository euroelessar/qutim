import QtQuick 2.1
import QtWinExtras 1.0
import "../default" as Default

Default.Controller {
    popupComponent: Default.PopupBase {
        id: window

        textColor: "black"
        textStyle: Text.Outline
        textStyleColor: "white"

        DwmFeatures {
            id: dwm

            topGlassMargin: -1
            leftGlassMargin: -1
            rightGlassMargin: -1
            bottomGlassMargin: -1
        }

        color: dwm.compositionEnabled ? "transparent" : dwm.realColorizationColor
    }
}
