import qbs.base 1.0

Project {
    name: "Protocols"

    SubProject {
        filePath: "vkontakte/vreen/vreen.qbs"

        Properties {
            vreen_qml_path: qutim_qml_path
            name: "vreen-imported"
        }
    }

    references: [
        "jabber/jreen/jreen.qbs",
        "jabber/jabber.qbs",
        "oscar/oscar.qbs",
        "irc/irc.qbs",
        "vkontakte/vkontakte.qbs"
    ]
}
