IF (NOT SYMBIAN)
	FIND_PACKAGE( Qt4 COMPONENTS QtWebkit)
	qutim_add_plugin( kineticscroller
		EXTENSION
		EXTENSION_HEADER ${CMAKE_CURRENT_SOURCE_DIR}/kineticscroller.h
		EXTENSION_CLASS Core::KineticScroller
		SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}
		DISPLAY_NAME "Qt Kinetic scroller"
		DESCRIPTION "Kinetic scroller from upcoming Qt release"
		LINK_LIBRARIES ${QT_QTWEBKIT_LIBRARY}
		INCLUDE_DIRS ${QT_QTWEBKIT_INCLUDE_DIR}
	)
ELSE()
	qutim_add_plugin( kineticscroller
		EXTENSION
		EXTENSION_HEADER ${CMAKE_CURRENT_SOURCE_DIR}/kineticscroller.h
		EXTENSION_CLASS Core::KineticScroller
		SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}
		DISPLAY_NAME "Qt Kinetic scroller"
		DESCRIPTION "Kinetic scroller from upcoming Qt release"
		COMPILE_FLAGS "-DQTSCROLLER_NO_WEBKIT"
	)

ENDIF()
