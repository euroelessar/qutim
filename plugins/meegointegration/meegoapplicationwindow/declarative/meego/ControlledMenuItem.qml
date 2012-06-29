// import QtQuick 1.0 // to target S60 5th Edition or Maemo 5
import QtQuick 1.1
import com.nokia.meego 1.0
import com.nokia.extras 1.0
import org.qutim 0.3

MenuItem {
    id: menuItem
    property QtObject action
    text: action.text
    onClicked: action.trigger()
}
