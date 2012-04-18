option(QUTIM_CREATE_BUNDLE "Create application bundle on make install" OFF)

if(QUTIM_CREATE_BUNDLE)
    include(InstallRequiredSystemLibraries)
    include(QtBundleUtils)
    define_bundle_paths(qutim)
    set(APPS ${BUNDLE_PATH})
    list(APPEND DIRS
        ${QT_LIBRARY_DIR}
    )
    if(WIN32)
        get_filename_component(_dir ${QT_QMAKE_EXECUTABLE} PATH)
        list(APPEND DIRS ${_dir})
    endif()

    list(APPEND QT_PLUGINS
        bearer/qgenericbearer
        bearer/qnativewifibearer
        codecs/qcncodecs
        codecs/qjpcodecs
        codecs/qjpcodecs
        codecs/qkrcodecs
        codecs/qtwcodecs
        imageformats/qsvg
        imageformats/qgif
        imageformats/qjpeg
        imageformats/qico
        imageformats/qmng
        imageformats/qtga
        imageformats/qtiff
        iconengines/qsvgicon
        crypto/qca-ossl
        crypto/qca-sasl
        crypto/qca-gnupg
    ) 

    deploy_qml_modules(${QML_MODULES})
    deploy_qt_plugins(${QT_PLUGINS})

    INSTALL(CODE "
        list(APPEND DIRS \"${CMAKE_INSTALL_PREFIX}/${PLUGINSDIR}\"
            \"${CMAKE_INSTALL_PREFIX}/${LIBDIR}\")
        file(GLOB_RECURSE QTPLUGINS
          \"\${CMAKE_INSTALL_PREFIX}/*${CMAKE_SHARED_LIBRARY_SUFFIX}\")
        include(BundleUtilities)
        fixup_bundle(\"${APPS}\" \"\${QTPLUGINS}\" \"${DIRS}\")
        " COMPONENT Runtime)
endif()
 
