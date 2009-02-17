#osdab lib
PROJECT (osdabzip)


SET (OSDAB_HEADERS
	unzip.h
	unzip_p.h
	zipentry_p.h
	zip.h
)

SET (OSDAB_SOURCES
	zip.cpp
	unzip.cpp
)


QT4_WRAP_CPP (MOC_SRCS ${OSDAB_HEADERS})

ADD_LIBRARY (osdabzip STATIC ${OSDAB_SOURCES} ${MOC_SRCS})

TARGET_LINK_LIBRARIES (osdabzip
        ${QT_LIBRARIES}
        ${OSDABZIP_LIBS}
        )
