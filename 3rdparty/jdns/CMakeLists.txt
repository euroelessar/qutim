# libjdns project for cmake
CMAKE_MINIMUM_REQUIRED( VERSION 2.6 )
PROJECT( libjdns )

# Set sources
SET( SRC
    "jdns.c"
    "jdns_mdnsd.c"
    "jdns_packet.c"
    "jdns_sys.c"
    "jdns_util.c"
    "qjdns_sock.cpp"
    "qjdns.cpp"
)

# MinGW special options
# Not sure if those applicable for linux
if(MINGW)
    ADD_DEFINITIONS( -DUNICODE -DQT_LARGEFILE_SUPPORT -DLIBQUTIM_LIBRARY -DQT_THREAD_SUPPORT )
    LIST( APPEND COMPILE_FLAGS "-mthreads" )
endif(MINGW)

# Require QT 4.6
SET( QT_MIN_VERSION "4.6.0" )

# Set QT modules
SET( QT_USE_QTNETWORK TRUE )
SET( QT_USE_QTGUI FALSE )

# Search for QT4
FIND_PACKAGE( Qt4 REQUIRED )

# Include QT4
INCLUDE( ${QT_USE_FILE} )

# Add include directories
INCLUDE_DIRECTORIES( ${CMAKE_CURRENT_BINARY_DIR} )

# Generate moc files
QT4_AUTOMOC( "qjdns.cpp" )

# This project will generate static library
ADD_LIBRARY( libjdns STATIC ${SRC} )

if( CMAKE_COMPILER_IS_GNUCXX )
	set_target_properties( libjdns PROPERTIES COMPILE_FLAGS "${COMPILE_FLAGS} -fPIC -DPIC" )
endif( CMAKE_COMPILER_IS_GNUCXX )

# Set tagret properties
SET_TARGET_PROPERTIES ( libjdns PROPERTIES PREFIX "")

# Link with QT
#TARGET_LINK_LIBRARIES( jdns ${QT_LIBRARIES} )
