import qbs.base 1.0

Project {
    name: "Protocols"

//    SubProject {
//        filePath: "vkontakte/vreen/vreen.qbs"

//        Properties {
//            // location of Vreen/Base/... with qml, qmldir and .so file
//            vreen_qml_path: {
//                if (qbs.targetOS.contains('osx'))
//                    return "bin/qutim.app/Contents/";
//                else if (qbs.targetOS.contains('windows'))
//                    return "bin/"
//                else
//                    return "lib/qutim/";
//            }
//            vreen_lib_path: qutim_lib_path
//            vreen_libexec_path: qutim_libexec_path
//            name: "vreen-imported"
//        }
//    }

    SubProject {
        filePath: "jabber/jreen/jreen.qbs"

        Properties {
            name: "jreen-imported"
        }
    }

    references: [
        "jabber/jabber.qbs",
        "oscar/oscar.qbs",
        "irc/irc.qbs",
//        "vkontakte/vkontakte.qbs",
    ]
}
