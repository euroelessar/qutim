import QtQuick 2.0
import QtQuick.Controls 1.3
import org.qutim 0.4

TableView {
    id: listView
    headerVisible: false
    alternatingRowColors: false
    frameVisible: false

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

        MouseArea {
            enabled: contact
            anchors.fill: parent
            acceptedButtons: Qt.RightButton

            onClicked: {
                listView.forceActiveFocus();
                listView.currentRow = styleData.row;
                listView.selection.clear();
                listView.selection.select(styleData.row, styleData.row);
                console.log('onClicked', listView.currentRow, styleData.row);
                menu.controller = contact;
                menu.popup();
            }
        }
    }

    ControlledMenu {
        id: menu
    }
}
