#osdab lib
PROJECT (osdabzip)

SET (OSDAB_SOURCES
	zip.cpp
	unzip.cpp
)

ADD_LIBRARY (osdabzip STATIC ${OSDAB_SOURCES})

TARGET_LINK_LIBRARIES (osdabzip
        ${QT_LIBRARIES}
        ${OSDABZIP_LIBS}
        )

INSTALL (TARGETS osdabzip DESTINATION lib/qutim/plugman )