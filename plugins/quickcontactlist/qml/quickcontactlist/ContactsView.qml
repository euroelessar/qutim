import QtQuick 2.2
import org.qutim 0.4

ListView {    
    Service {
        id: contactModel
        name: "ContactModel"
    }
    
    VisualDataModel {
        id: rootModel
        model: contactModel.object
    }
    
    model: rootModel
    
    delegate: Text {
        text: title
    }
}
