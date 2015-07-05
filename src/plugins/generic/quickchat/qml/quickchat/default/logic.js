
function addSession(session) {
    console.log('Session created!111', session.unit.title);

    var tab = tabs.addTab("", tabComponent);
    tabs.currentIndex = tabs.count - 1;
    tab.title = Qt.binding(function() {
        return session.unit.title;
    });
    tab.item.session = session;
    window.updateToolBar();
}

function removeSession(session) {
    console.log('Session destroyed!111', session.unit.title);

    for (var i = 0; i < tabs.count; ++i) {
        var tab = tabs.getTab(i);
        if (tab.item.session === session) {
            tabs.removeTab(i);
            break;
        }
    }
    window.updateToolBar();
}

function setActiveSession(session) {
    if (!session || !session.unit) {
        return;
    }

    console.log('Session activated!111', session.unit.title);

    for (var i = 0; i < tabs.count; ++i) {
        var tab = tabs.getTab(i);
        if (tab.item.session === session) {
            if (tabs.currentIndex !== i) {
                tabs.currentIndex = i;
                break;
            }
        }
    }
    window.updateToolBar();
}
