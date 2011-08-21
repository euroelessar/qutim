qutim_add_plugin(meegocontactlist
    EXTENSION
    EXTENSION_HEADER ${CMAKE_CURRENT_SOURCE_DIR}/src/contactlist.h
    EXTENSION_CLASS MeegoIntegration::ContactList
	DISPLAY_NAME "Meego contact list"
	DESCRIPTION "Shows list of contacts"
	LINK_LIBRARIES ${QT_QTDECLARATIVE_LIBRARY}
	INCLUDE_DIRS ${QT_QTDECLARATIVE_INCLUDE_DIR}
)
