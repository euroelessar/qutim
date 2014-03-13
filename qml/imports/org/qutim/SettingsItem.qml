import QtQuick 2.2
import org.qutim 0.4

Item {
    id: root
    
    property bool __loading: true
    
    function __save() {
        root.save();
        
        __modified = false;
    }
    
    function __load() {
        __loading = true;
        __modified = false;
        
        root.load();
        
        __loading = false;
    }
    
    function __cancel() {
        __loading = true;
        __modified = false;
        
        if (root.cancel)
            root.cancel();
        else
            root.load();
        
        __loading = false;
    }
    
    function modify() {
        if (!__loading)
            __modified = true;
    }
    
    property bool __modified: false
    
    Component.onCompleted: __modified = false
}
