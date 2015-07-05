var popups = [];

var MARGIN = 20;
var DURATION = 600;

function relayoutItems() {
    var x = root.screenWidth;
    var y = root.screenHeight;

    for (var i = 0; i < popups.length; ++i) {
        var popup = popups[i];
        var popupHeight = popup.window.height;
        var popupWidth = popup.window.width;
        
        var toX = x - popupWidth - MARGIN;
        var toY = y - popupHeight - MARGIN;

        if (!popup.window.visible) {
            popup.window.x = toX;
            popup.window.y = toY + MARGIN / 2;
        }

        popup.move(toX, toY);

        y = toY;
    }
}

function addPopup(notify) {
    var popup = animationComponent.createObject();
    popup.window.addNotify(notify);

    popups.push(popup);
    relayoutItems();
    popup.show();
}

function removePopup(notify) {
    console.log('Notification removed: ', notify ? notify.title : undefined)

    for (var i = 0; i < popups.length; ++i) {
        var popup = popups[i];
        if (popup === notify) {
            popup.window.removeAll();
            
            popups.splice(i, 1);
            popup.hide();
            --i;
        } else if (popup.window.removeNotify(notify)) {
            popups.splice(i, 1);
            popup.hide();
            --i;
        }
    }

    relayoutItems();
}

function init(notifies) {
    var i;
    
    for (i = 0; i < notifies.length; ++i) {
        var notify = notifies[i];
        var popup = animationComponent.createObject();
        popup.window.addNotify(notify);

        popups.push(popup);
    }
    relayoutItems();
    
    for (i = 0; i < popups.length; ++i)
        popups[i].show();
}

function deinit() {
    for (var i = 0; i < popups.length; ++i) {
        var popup = popups[i];
        popup.window.notifies = [];
        popup.window.destroy();
        popup.destroy();
    }
    popups = [];
}
