
find_path(HUNSPELL_INCLUDE_DIR hunspell.hxx
  /usr/local/include/hunspell
  /usr/local/include
  /usr/include/hunspell
  /usr/include
  )

set(HUNSPELL_NAMES ${HUNSPELL_NAMES} hunspell hunspell-1.2 hunspell-1.3)
find_library(HUNSPELL_LIBRARY
  NAMES ${HUNSPELL_NAMES}
  PATHS /usr/lib /usr/local/lib
  )

if(HUNSPELL_LIBRARY AND HUNSPELL_INCLUDE_DIR)
  message(STATUS "Found Hunspell: ${HUNSPELL_LIBRARY}")
else(HUNSPELL_LIBRARY AND HUNSPELL_INCLUDE_DIR)
  message( "HUNSPELL not found. Can't build hunspeller" )
  return()
endif(HUNSPELL_LIBRARY AND HUNSPELL_INCLUDE_DIR)

include_directories( ${HUNSPELL_INCLUDE_DIR} )

qutim_add_plugin( hunspeller
	DISPLAY_NAME "HunSpell checker"
	DESCRIPTION "Check spelling by HunSpell library"
	LINK_LIBRARIES ${HUNSPELL_LIBRARY}
)
