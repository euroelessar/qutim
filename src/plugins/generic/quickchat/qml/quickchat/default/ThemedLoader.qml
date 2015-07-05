import QtQuick 2.0
import org.qutim 0.4

Loader {
    id: loader
    source: manager.path("qml/" + category, config.themeName) + "/main.qml"

    property string category

    ThemeManager {
        id: manager
    }

    Config {
        id: config
        group: "themes/" + loader.category

        property string themeName: "default"
    }
}
