project (KineticNotification)

SET(HEADERS
	kineticpopup.h
    kineticpopupsmanager.h
    kineticpopupwidget.h
	kineticpopupbackend.h
)

SET(SOURCES
	kineticpopup.cpp
    kineticpopupsmanager.cpp
    kineticpopupwidget.cpp
	kineticpopupbackend.cpp
)


QT4_WRAP_CPP (MOC_SRCS ${HEADERS})

ADD_LIBRARY (KineticNotification ${SOURCES} ${MOC_SRCS})

TARGET_LINK_LIBRARIES (KineticNotification
        ${QT_LIBRARIES}
		${QUTIM_LIBS}
		${QUTIM_LIBRARY}
		)