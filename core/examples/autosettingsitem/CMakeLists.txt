cmake_minimum_required ( VERSION 2.6 FATAL_ERROR )
project ( AUTOSETTINGSITEM )

include ( "${CMAKE_CURRENT_SOURCE_DIR}/../../cmake/QutimPlugin.cmake"  )

ADD_QUTIMPLUGIN (autosettingsitem ${CMAKE_CURRENT_SOURCE_DIR} )
