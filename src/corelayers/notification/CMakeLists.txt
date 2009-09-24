project (KineticNotification)

SET(HEADERS
	kineticnotification.h 
    notificationsmanager.h 
    notificationwidget.h 
)

SET(SOURCES
	kineticnotification.cpp
    notificationsmanager.cpp
    notificationwidget.cpp
)


QT4_WRAP_CPP (MOC_SRCS ${HEADERS})

ADD_LIBRARY (KineticNotification ${SOURCES} ${MOC_SRCS})

TARGET_LINK_LIBRARIES (KineticNotification
        ${QT_LIBRARIES}
		${QUTIM_LIBS}
		${QUTIM_LIBRARY}
		)