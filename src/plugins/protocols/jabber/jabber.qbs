import "../Protocol.qbs" as Protocol

Protocol {
    Depends { name: "jreen" }

    cpp.includePaths: ["src"]
}
