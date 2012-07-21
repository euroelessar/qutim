add_subdirectory(lib)

qutim_add_plugin(simplecontactlist
	EXTENSION
        EXTENSION_HEADER ${CMAKE_CURRENT_SOURCE_DIR}/contactlist/simplecontactlist.h
	EXTENSION_CLASS Core::SimpleContactList::Module
	SOURCE_DIR contactlist
	DISPLAY_NAME "Simple contact list"
	DESCRIPTION "Default qutIM contact list implementation. Just simple"
	COMPILE_FLAGS "-DSIMPLECONTACTLIST_LIBRARY"
	LINK_LIBRARIES qutim-simplecontactlist
)

qutim_add_plugin(simplecontactdelegate
	SUBPLUGIN
	SOURCE_DIR simplecontactdelegate
	DISPLAY_NAME "Contact delegate"
	DESCRIPTION "Default contact delegate. Just simple"
	DEPENDS simplecontactlist
	LINK_LIBRARIES qutim-simplecontactlist
)

set(DEVEL_HEADERS
	lib/abstractcontactlist.h
	lib/abstractcontactmodel.h
	lib/simplecontactlistview.h
	lib/simplestatusdialog.h
	lib/simplecontactlist_global.h
	lib/servicechooser.h
	lib/simplecontactlistitem.h
	lib/lineedit.h
	lib/abstractcontactmodel_p.h
)
install(FILES ${DEVEL_HEADERS}
	DESTINATION include/qutim/simplecontactlist
	COMPONENT CoreDevel
)
qutim_generate_includes("qutim/simplecontactlist" ${DEVEL_HEADERS})


add_subdirectory(widgets)
add_subdirectory(models)
