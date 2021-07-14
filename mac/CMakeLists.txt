# Install example Geant4 macro files

file(GLOB g4_macros RELATIVE ${PROJECT_SOURCE_DIR}/mac CONFIGURE_DEPENDS 
	  *.mac
	  *.md
	  )

foreach(_g4macro ${g4_macros})
  configure_file(
    ${_g4macro}
    ${_g4macro}
    COPYONLY
    )
endforeach()
