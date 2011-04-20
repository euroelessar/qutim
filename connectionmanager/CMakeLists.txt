if(${BEARERMANAGER})
	message(STATUS "ConnectionManager conflict with BearerManager. Skipped")
	return()
endif()

find_package(Qt4 COMPONENTS QtNetwork )
qutim_add_plugin( connectionmanager
	DISPLAY_NAME "ConnectionManager"
	DESCRIPTION ""
)
