FIND_LIBRARY(
	WINTHINGS_APILAYER_LIBRARY wapicwrapper
	$ENV{LIB}
)

if(WINTHINGS_APILAYER_LIBRARY)
	message( STATUS "Found WinThings API layer library: ${WINTHINGS_APILAYER_LIBRARY}")
else()
	message( STATUS "Could NOT find WinThings API layer library.")
endif()
