Product {
    name: "jreen"
    
    property bool useSimpleSasl: true
    property string versionMajor: '1'
    property string versionMinor: '1'
    property string versionRelease: '0'
    property string version: versionMajor+'.'+versionMinor+'.'+versionRelease
    
    moduleSearchPaths: "../../../qbs/modules"

    destination: "lib"
    type: ["dynamiclibrary", "installed_content"]

    Depends { name: "cpp" }
    Depends { name: "Qt.core" }
    Depends { name: "Qt.network" }
    Depends { name: "qca"; required: true }
    Depends { name: "zlib"; required: true }
    Depends { name: "speex" }
    Depends { name: "windows.ws2_32" }
    Depends { name: "windows.advapi32" }

    //cpp.warningLevel: "all"
    cpp.includePaths: [
        ".",
        "3rdparty",
        "3rdparty/jdns",
        "3rdparty/simplesasl",
        "3rdparty/icesupport",
        "src",
        "src/experimental"
    ]
    cpp.defines: ["J_BUILD_LIBRARY"]
    cpp.positionIndependentCode: true

    Properties {
        condition: false //speex.found
        cpp.defines: "JREEN_HAVE_SPEEX=1"
    }
    Properties {
        condition: useSimpleSasl
        cpp.defines: "HAVE_SIMPLESASL"
    }
    Properties {
        condition: qbs.toolchain === "gcc" && (qbs.platform !== "windows")
        cpp.cxxFlags: "-fvisibility=hidden"
    }

    Group {
        prefix: "src/"
        files: [
            "*.h",
            "*.cpp"
        ]
    }

    Group {
        //jdns files
        prefix: "3rdparty/jdns/"
        files: [
            "*.h",
            "*.c",
            "*.cpp"
        ]
    }

    Group {
        //ice files
        prefix: "3rdparty/icesupport/"
        files: [
            "*.h",
            "*.cpp"
        ]
    }

    Group {
        //simple sasl files
        condition: useSimpleSasl
        prefix: "3rdparty/simplesasl/"
        files: [
            "*.h",
            "*.cpp"
        ]
    }

    Group {
        //experimental jingle support
        prefix: "src/experimental/"
        files: [
            "*.h",
            "*.cpp"
        ]
    }

    Group {
        //install headers
        qbs.installDir: "include/jreen"
        fileTags: ["install"]
        files: [
            "src/entitytime.h",
            "src/udpconnection.h",
            "src/receipt.h",
            "src/captcha.h",
            "src/directconnection.h",
            "src/messagesession.h",
            "src/attention.h",
            "src/pubsubmanager.h",
            "src/pgpsigned.h",
            "src/client.h",
            "src/abstractroster.h",
            "src/capabilities.h",
            "src/mucroom.h",
            "src/util.h",
            "src/jid.h",
            "src/jstrings.h",
            "src/activity.h",
            "src/buffereddatastream.h",
            "src/stanza.h",
            "src/nickname.h",
            "src/parser.h",
            "src/datastream.h",
            "src/iq.h",
            "src/disco.h",
            "src/vcard.h",
            "src/pgpencrypted.h",
            "src/stanzaextension.h",
            "src/softwareversion.h",
            "src/forwarded.h",
            "src/privatexml.h",
            "src/iqreply.h",
            "src/logger.h",
            "src/connectionbosh.h",
            "src/bookmarkstorage.h",
            "src/mood.h",
            "src/vcardupdate.h",
            "src/jreen.h",
            "src/tune.h",
            "src/connection.h",
            "src/privacymanager.h",
            "src/tcpconnection.h",
            "src/delayeddelivery.h",
            "src/bookmark.h",
            "src/dataform.h",
            "src/registrationmanager.h",
            "src/vcardmanager.h",
            "src/bitsofbinary.h",
            "src/metacontactstorage.h",
            "src/subscription.h",
            "src/error.h",
            "src/pubsubevent.h",
            "src/presence.h",
            "src/ping.h",
            "src/streamprocessor.h",
            "src/chatstate.h",
            "src/message.h",
        ]
    }
} 
