import "../UreenPlugin.qbs" as UreenPlugin

UreenPlugin {
    sourcePath: ''
    type: ['installed_content', 'dynamiclibrary']

    Depends { name: "libqutim-simplecontactlist" }
    Depends { name: "qt.xml" }
    Depends { name: "qutimscope" }
    Depends { name: "artwork" }

    cpp.includePaths: [
        '.',
        'settings',
    ]

    Group {
        fileTags: [ "artwork" ]
        qutimscope.basePath: ''
        prefix: "olddelegate/"
        files: '*'
        recursive: true
    }
}
