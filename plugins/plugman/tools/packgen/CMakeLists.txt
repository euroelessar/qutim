PROJECT (packgen)

FIND_PACKAGE (Qt4 REQUIRED)

INCLUDE_DIRECTORIES (${QT_INCLUDES}
		${PLUGMAN_INCLUDE_DIR}
		${CMAKE_CURRENT_BINARY_DIR}
			.
		)


SET (PACKGEN_SOURCES
	main.cpp
    generatorwindow.cpp 
    pluglibinfo.cpp 
    choosecategorypage.cpp 
    choosepathpage.cpp 
    configpackagepage.cpp
)

SET (PACKGEN_HEADERS
	generatorwindow.h
    pluglibinfo.h
    choosecategorypage.h
    choosepathpage.h 
    configpackagepage.h
)

SET (FORMS
	choosecategorypage.ui 
    choosepathpage.ui 
    configpackagepage.ui
)

QT4_WRAP_CPP (MOC_SRCS ${PACKGEN_HEADERS})
QT4_WRAP_UI (UIS_H ${FORMS})


ADD_EXECUTABLE (packgen  ${PACKGEN_SOURCES} ${MOC_SRCS} ${UIS_H})


TARGET_LINK_LIBRARIES (packgen
		${QT_LIBRARIES}
		${QZIPREADER_LIBS}
		qzipreader
		plugman_static
		)

