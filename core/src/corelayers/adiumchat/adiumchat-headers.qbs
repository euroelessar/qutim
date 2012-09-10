Product {
    type: ["installed_content"]
    name: "adiumchat-headers"

    Group {
        qbs.installDir: "include/qutim/adiumchat"
        fileTags: [ "install", "hpp" ]
        files: [
            "chatlayer/chatforms/abstractchatform.h",
            "chatlayer/chatforms/abstractchatwidget.h",
            "chatlayer/chatedit.h",
            "chatlayer/chatlayer_global.h",
            "chatlayer/chatlayerimpl.h",
            "chatlayer/chatlayerplugin.h",
            "chatlayer/chatsessionimpl.h",
            "chatlayer/chatsessionmodel.h",
            "chatlayer/chatviewfactory.h",
            "chatlayer/conferencecontactsview.h",
            "chatlayer/conferencetabcompletion.h",
            "chatlayer/sessionlistwidget.h"
        ]
    }
}
