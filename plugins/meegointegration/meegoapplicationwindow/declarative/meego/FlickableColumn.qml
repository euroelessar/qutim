// import QtQuick 1.0 // to target S60 5th Edition or Maemo 5
import QtQuick 1.1

Flickable {
    id: flickable
    contentWidth: column.width
    contentHeight: column.height
    
    default property alias content: column.children
    property alias spacing: column.spacing
    property alias add: column.add
    property alias move: column.move
    
    Column {
        id: column
        width: flickable.width
    }
}
