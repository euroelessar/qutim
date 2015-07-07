import qbs.base 1.0

Project {
    name: "Plugins"

    references: [
        "generic/generic.qbs",
        "protocols/protocols.qbs",
        "integrations/integrations.qbs"
    ]
}
