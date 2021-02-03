# Install example Geant4 ROOT scripts and conversion code

file(GLOB root_scripts RELATIVE ${PROJECT_SOURCE_DIR}/scripts CONFIGURE_DEPENDS 
	  *.C 
	  *.cc 
	  *.txt 
	  *.md 
	  *.root)

foreach(_script ${root_scripts})
  configure_file(
    ${_script}
    ${_script}
    COPYONLY
    )
endforeach()
