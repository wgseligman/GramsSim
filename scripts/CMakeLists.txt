# Install example Geant4 ROOT scripts and conversion code

file(GLOB root_scripts RELATIVE ${PROJECT_SOURCE_DIR}/scripts CONFIGURE_DEPENDS 
	  *.C 
	  *.py 
	  *.cc 
	  *.txt 
	  *.md 
	  *.root
	  *.hepmc3
	  )

foreach(_script ${root_scripts})
  configure_file(
    ${_script}
    ${_script}
    COPYONLY
    )
endforeach()

# Compile the .cc files:

# Get a list of all the files in this directory that end in ".cc"
file(GLOB script_progs RELATIVE ${PROJECT_SOURCE_DIR}/scripts CONFIGURE_DEPENDS 
	  *.cc
	  )

# For each such file...
foreach(_filename ${script_progs})
   # Derive the executable program name by
   # stripping the ".cc" from the end.
   string(REGEX REPLACE "\\.cc" "" _prog "${_filename}")

   # Tell CMake to compile <whatever.cc> to make
   # the executable <whatever>, using the libraries
   # from HepMC3, ROOT, and FITS.
   add_executable(${_prog} ${_filename})
   target_link_libraries(${_prog} ${HEPMC3_LIBRARIES} )
   target_link_libraries(${_prog} ${ROOT_LIBRARIES} )
   target_link_libraries(${_prog} ${FITS_LDFLAGS} )

   # Put the compiled binary into a "bin" directory within
   # the destination directory. 
   set_target_properties( ${_prog} 
      PROPERTIES RUNTIME_OUTPUT_DIRECTORY 
      "${CMAKE_BINARY_DIR}/bin" 
      )
endforeach()
