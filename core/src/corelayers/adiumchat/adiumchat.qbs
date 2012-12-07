import qbs.base 1.0

Project {
    moduleSearchPaths: [ "modules", "qbs/modules" ]

    references: [
        "plugin.qbs",
        "qutim-adiumchat.qbs",
        "chatfactories/textchat/textchat.qbs",
        "chatforms/stackedchatform/stackedchatform.qbs",
        "chatforms/tabbedchatform/tabbedchatform.qbs",
    ]
}
