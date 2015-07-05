// import QtQuick 1.0 // to target S60 5th Edition or Maemo 5
import QtQuick 1.1
import com.nokia.meego 1.0
import com.nokia.extras 1.0
import org.qutim 0.3
import ".."

SettingsItemPage {
    id: root
    impl: QtObject {
        function save() {
            config.setValue("highlighter/enableAutoHighlights", nickSwitch.checked);
            var regexps = [];
            for (var i = 0; i < listModel.count; ++i) {
                var regexp = listModel.get(i);
                var data = {};
                data.pattern = regexp.pattern;
                data.patternSyntax = regexp.patternSyntax;
                regexps.push({ "regexp": regexpService.toRegExp(data) });
            }
            config.setValue("highlighter/regexps", regexps);
        }
        function load() {
            listModel.clear();
            nickSwitch.checked = config.value("highlighter/enableAutoHighlights", true);
            var regexps = config.value("highlighter/regexps");
            if (regexps === undefined)
                return;
            for (var i = 0; i < regexps.length; ++i) {
                var regexp = regexpService.fromRegExp(regexps[i].regexp);
                listModel.append(regexp);
            }
        }
    }
    Config {
        id: config
    }
    RegExpService {
        id: regexpService
    }
    function syntaxName(syntax) {
        switch (syntax) {
        case RegExpService.RegExp:
        default:
            return qsTr("Perl-like");
        case RegExpService.RegExp2:
            return qsTr("Improved perl-like");
        case RegExpService.Wildcard:
            return qsTr("Shell-like");
        case RegExpService.WildcardUnix:
            return qsTr("Shell-like Unix");
        case RegExpService.FixedString:
            return qsTr("Fixed string");
        case RegExpService.W3CXmlSchema11:
            return qsTr("W3C XML Schema");
        }
    }

    Column {
        anchors.fill: parent
        spacing: 10
    }
    SwitchRow {
        id: nickSwitch
        anchors.top: parent.top
        text: qsTr("Enable auto highlights wher your nick was mentioned.")
    }
    ListView {
        anchors.bottom: parent.bottom
        anchors.top: nickSwitch.bottom
        width: parent.width
        model: ListModel {
            id: listModel
        }
        delegate: ItemDelegate {
            title: model.pattern
            subtitle: root.syntaxName(model.patternSyntax)
            ToolIcon {
                anchors { verticalCenter: parent.verticalCenter; right: parent.right }
                platformIconId: "toolbar-close"
                onClicked: listModel.remove(index)
            }
        }
    }
    QueryDialog {
        id: dialog
        titleText: qsTr("Add regular expression")
        acceptButtonText: qsTr("Add")
        rejectButtonText: qsTr("Cancel")
        content: Column {
            spacing: 10
            ComboBox {
                id: comboBox
                width: dialog.width
                title: qsTr("Pattern syntax")
                platformStyle: ComboBoxStyle { inverted: true }
                model: ListModel {
                    id: syntaxesModel
                }
            }
            TextField {
                id: textField
                width: dialog.width
                placeholderText: qsTr("Regular expression")
            }
        }
        onAccepted: listModel.append({ "pattern": textField.text, "patternSyntax": comboBox.currentItem.value })
    }
    
    tools: SettingsToolBarLayout {
        id: toolBarLayout
        page: root

        ToolIcon {
            visible: true
            platformIconId: "toolbar-add"
            onClicked: {
                comboBox.currentIndex = 0;
                dialog.open();
            }
        }
    }
    Component.onCompleted: {
        var syntaxes = [
                    RegExpService.RegExp,
                    RegExpService.RegExp2,
                    RegExpService.Wildcard,
                    RegExpService.WildcardUnix,
                    RegExpService.FixedString,
                    RegExpService.W3CXmlSchema11
                ];
        for (var i = 0; i < syntaxes.length; ++i) {
            var syntax = syntaxes[i];
            syntaxesModel.append({
                                     "name": root.syntaxName(syntax),
                                     "value": syntax
                                 });
        }
    }
}
