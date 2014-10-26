import QtQuick 2.0
import QtQuick.Controls 1.3

TableView {
    id: listView
    headerVisible: false
    alternatingRowColors: false

    SystemPalette {
        id: palette
        colorGroup: enabled ? SystemPalette.Active : SystemPalette.Disabled
    }

    TableViewColumn {
    }

    // It's still not possible to specify row's height externally, so hack it a bit
    rowDelegate: Component {
        Item {
            height: 26

            QtObject {
                id: context
                readonly property QtObject _styleData: styleData
                readonly property var _model: model
                readonly property var _modelData: modelData
            }

            Loader {
                height: parent.height
                width: parent.width
                sourceComponent: listView.__style.rowDelegate

                property alias styleData: context._styleData
                property alias model: context._model
                property alias modelData: context._modelData
            }
        }
    }

    itemDelegate: BuddyDelegate {
        width: parent.width

        titleColor: styleData.textColor
        subtitleColor: styleData.textColor

        property string iconSource: model.iconSource
        property string title: model.title
        property string subtitle: model.subtitle
        property QtObject contact: model.contact
    }
}
