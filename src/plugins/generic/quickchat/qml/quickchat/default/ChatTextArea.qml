import QtQuick 2.0
import QtQuick.Controls 1.3
import org.qutim 0.4
import org.qutim.quickchat 0.4

TextArea {
    id: chatInput
    wrapMode: TextEdit.Wrap
    textFormat: TextEdit.PlainText
    implicitHeight: _autoResize ? contentHeight + textMargin * 2 : 0
    backgroundVisible: false

    Rectangle {
        anchors.fill: parent
        color: "#40ffffff"
        z: -2
    }

    Config {
        id: config
        path: "appearance"
        group: "chat/behavior/widget"

        property alias sendKey: chatInput._sendKey
        property alias autoResize: chatInput._autoResize
    }

    CompletionModel {
        id: completion
    }

    property alias completionModel: completion.model
    property QtObject session

    property int _sendKey: 1
    property bool _autoResize: false

    property int _tabsCount: 0
    property string _prefix
    property string _currentSuggestion
    property int _currentLength
    property int _cursorPosition: -1

    property int _entersCount: 0
    property int _enterPosition: 0

    function appendText(text) {
        chatInput.insert(chatInput.cursorPosition, text + ' ');
        chatInput.forceActiveFocus();
    }

    function appendNick(nick) {
        if (chatInput.cursorPosition === 0)
            chatInput.insert(0, nick + ': ');
        else
            chatInput.insert(chatInput.cursorPosition, nick + ' ');
        chatInput.forceActiveFocus();
    }

    function _send() {
        session.send(text);
        text = '';
    }

    function _processEnter(event) {
        var SendEnter = 0;
        var SendCtrlEnter = 1;
        var SendDoubleEnter = 2;

        if (event.key === Qt.Key_Enter || event.key === Qt.Key_Return) {
            if (event.modifiers === Qt.ControlModifier)  {
                if (_sendKey == SendCtrlEnter) {
                    _send();
                    event.accepted = true;
                } else if (_sendKey === SendEnter || _sendKey === SendDoubleEnter) {
                    chatInput.insert(chatInput.cursorPosition, '\n');
                    event.accepted = true;
                }
            } else if (event.modifiers === Qt.NoModifier
                       || event.modifiers === Qt.KeypadModifier) {
                if (_sendKey == SendEnter) {
                    _send();
                    event.accepted = true;
                } else if (_sendKey == SendDoubleEnter) {
                    _entersCount++;
                    if (_entersCount > 1) {
                        chatInput.remove(_enterPosition, _enterPosition + 1);
                        _entersCount = 0;
                        _send();
                        event.accepted = true;
                    } else {
                        _enterPosition = chatInput.cursorPosition;
                    }
                }
            }
        } else {
            _entersCount = 0;
        }
    }

    function _findPrefix() {
        var text = chatInput.getText(0, chatInput.cursorPosition);
        var prefix = /[^\s]*$/m.exec(text);
        return prefix === null ? '' : prefix[0];
    }

    function _processTab(event) {
        if (event.key === Qt.Key_Tab && completion.valid) {
            ++_tabsCount;
            event.accepted = true;

            var suggestions;
            var currentIndex;

            if (_tabsCount === 1 || _tabsCount === 2) {
                _prefix = _findPrefix();
                suggestions = completion.suggestions(_prefix);

                if (_tabsCount === 1 && suggestions.length !== 1) {
                    return;
                }

                _cursorPosition = chatInput.cursorPosition - _prefix.length;
                _currentLength = _prefix.length;
                _currentSuggestion = _prefix;
                currentIndex = 0;
            } else if (_tabsCount > 2) {
                suggestions = completion.suggestions(_prefix);
                currentIndex = suggestions.indexOf(_currentSuggestion);
                currentIndex = (currentIndex + 1) % suggestions.length;
            }

            chatInput.remove(_cursorPosition, _cursorPosition + _currentLength);

            _currentSuggestion = suggestions[currentIndex];
            var newNick = _currentSuggestion;
            if (_cursorPosition === 0)
                newNick += ': ';
            _currentLength = newNick.length;

            chatInput.insert(_cursorPosition, newNick);

            if (suggestions.length !== 1)
                return;
        }

        _tabsCount = 0;
        _prefix = '';
    }

    Keys.onPressed: {
        _processEnter(event);
        _processTab(event);
    }
}
