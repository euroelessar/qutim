project (JsonConfigBackend)

SET(HEADERS
	jsonconfigbackend.h
)

SET(SOURCES
	jsonconfigbackend.cpp
)


QT4_WRAP_CPP (MOC_SRCS ${HEADERS})

ADD_LIBRARY (JsonConfigBackend ${SOURCES} ${MOC_SRCS})

TARGET_LINK_LIBRARIES (JsonConfigBackend
        ${QT_LIBRARIES}
		${QUTIM_LIBS}
		${QUTIM_LIBRARY}
		)