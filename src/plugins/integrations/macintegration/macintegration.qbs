import qbs.base 1.0

Project {
    name: "OsX specific"

    references: [
        "src/maccontactlistwidget/maccontactlistwidget.qbs",
        "src/macdock/macdock.qbs",
        "src/macgrowl/macgrowl.qbs",
        "src/macidle/macidle.qbs",
        "src/macspellchecker/macspellchecker.qbs"
    ]
}
