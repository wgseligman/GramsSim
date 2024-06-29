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

# Make lists of programs that have any special dependencies.
list(APPEND HepMC3_Programs
  "hepmc-convert.cc"
  "hepmc-grams-example.cc"
  "hepmc-ntuple.cc"
)

list(APPEND FITS_Programs
  "healpix-maps.cc"
)

# For each such file...
foreach(_filename ${script_progs})
   # Check if we can compile this program with the available
   # libraries.
   if (NOT HepMC3_FOUND)
     # Is this file on our list of programs that need HepMC3?
     list (FIND HepMC3_Programs "${_filename}" _index)
     if ( _index GREATER -1 )
       # Yes it is, so don't compile it.
       continue()
     endif()
   endif()

   if (NOT FITS_FOUND)
     # Is this file on our list of programs that need cfitsio/healpix_cxx?
     list (FIND FITS_Programs "${_filename}" _index)
     if ( _index GREATER -1 )
       # Yes it is, so don't compile it.
       continue()
     endif()
   endif()

   # Derive the executable program name by
   # stripping the ".cc" from the end. 
   string(REGEX REPLACE "\\.cc" "" _prog "${_filename}")
   # Add an extension determined by GramsSim/CMakeLists.txt.
   string(APPEND _prog ${EXE})

   # Tell CMake to compile <whatever.cc> to make
   # the executable <whatever>, using the libraries
   # from HepMC3, ROOT, and FITS.
   add_executable(${_prog} ${_filename})
   target_link_libraries(${_prog} ${ROOT_LIBRARIES} )

   # Make sure that the Utilities and Dictionary libraries are
   # available to the compiled programs, even if the program
   # may not use them. 
   target_link_libraries(${_prog} ${CMAKE_PROJECT_NAME}Utilities )
   target_link_libraries(${_prog} Dictionary)
   # The following line is needed to make sure Dictionary is linked with the executable.
   if (NOT MACOSX)
      # The following line is needed to make sure Dictionary is linked with the executable.
      target_link_options(${_prog} PRIVATE "LINKER:-no-as-needed")
   endif()

   if (HepMC3_FOUND)
     target_link_libraries(${_prog} ${HEPMC3_LIBRARIES} )
   endif()
   if (FITS_FOUND)
     target_link_libraries(${_prog} ${FITS_LDFLAGS} )
   endif()

   # Put the compiled binary into a "bin" directory within
   # the destination directory. 
   set_target_properties( ${_prog} 
      PROPERTIES RUNTIME_OUTPUT_DIRECTORY 
      "${CMAKE_BINARY_DIR}/bin" 
      )
endforeach()
