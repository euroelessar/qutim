qutim_add_plugin(meegoapplicationwindow
    EXTENSION
    EXTENSION_HEADER ${CMAKE_CURRENT_SOURCE_DIR}/src/applicationwindow.h
    EXTENSION_CLASS MeegoIntegration::ApplicationWindow
	DISPLAY_NAME "Meego application window"
	DESCRIPTION "Main window of application"
	LINK_LIBRARIES ${QT_QTDECLARATIVE_LIBRARY}
	INCLUDE_DIRS ${QT_QTDECLARATIVE_INCLUDE_DIR}
)
