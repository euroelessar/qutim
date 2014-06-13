import QtQuick 2.2

ListView {
    id: root

    property Item view
    property alias rootIndex: rootModel.rootIndex

//    header: view ? view.componentByType(itemType) : undefined

//    VisualDataModel {
//        id: rootModel
//        model: root.view.model
////        delegate: ItemDelegate2 {
////            view: root.view
////            rootIndex: rootModel.modelIndex(index)
////        }
//    }

//    model: rootModel
}
