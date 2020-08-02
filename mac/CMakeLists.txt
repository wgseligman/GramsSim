# Install example Geant4 macro files
set(g4_macros 
		run.mac
		vis.mac
		debug-geom.mac
)

foreach(_g4macro ${g4_macros})
  configure_file(
    ${_g4macro}
    ${_g4macro}
    COPYONLY
    )
endforeach()
