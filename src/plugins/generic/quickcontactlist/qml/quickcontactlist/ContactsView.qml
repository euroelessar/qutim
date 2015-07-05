import QtQuick 2.2
import org.qutim 0.4
import "constants.js" as Constants

ListView {
    id: root

    Service {
        id: contactModel
        name: "ContactModel"
    }

    property Component accountItem: Text {
        text: title + " (contact)"
    }
    property Component groupItem: Text {
        text: title + " (group)"
    }
    property Component contactItem: Text {
        text: title + " (contact)"
    }

    function componentByType(type) {
        if (type === Constants.ACCOUNT)
            return accountItem;
        if (type === Constants.GROUP)
            return groupItem;
        if (type === Constants.CONTACT)
            return contactItem;
        return undefined;
    }
    
    VisualDataModel {
        id: rootModel
        model: contactModel.object
        delegate: ListView {
            header: componentByType(itemType)
        }
//        ItemDelegate {
//            view: root
//            rootIndex: rootModel.modelIndex(index)
//        }
    }
    
    model: rootModel
}
