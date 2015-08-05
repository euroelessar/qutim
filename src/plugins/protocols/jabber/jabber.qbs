import "../Protocol.qbs" as Protocol

Protocol {
    property bool jingle: false

    Depends { name: "jreen" }

    cpp.includePaths: ["src"]
    cpp.defines: {
        var basicDefs = base
        if(jingle)
            basicDefs = basicDefs.concat("JABBER_HAVE_MULTIMEDIA")
        return basicDefs
    }

    Depends {
        condition: jingle
        name: "Qt.multimedia"
    }
}
