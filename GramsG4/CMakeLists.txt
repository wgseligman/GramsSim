# GramsG4

#----------------------------------------------------------------------------
# Locate sources and headers for this project
#
include_directories(include 
                    g4util/include
                    ${PROJECT_SOURCE_DIR}/util/include
                    ${Geant4_INCLUDE_DIR}
                    ${ROOT_INCLUDE_DIR}
)

file(GLOB sources src/*.cc
                  g4util/src/*.cc
                  ${PROJECT_SOURCE_DIR}/util/src/*.cc
    )

#----------------------------------------------------------------------------
# Add the executables, and link them to the libraries

# GramsG4
add_executable(gramsg4 gramsg4.cc ${sources})
target_link_libraries(gramsg4 ${HEPMC3_LIBRARIES} )
target_link_libraries(gramsg4 ${ROOT_LIBRARIES} )
target_link_libraries(gramsg4 ${Geant4_LIBRARIES} )
#message(STATUS "Geant4_LIBRARIES = ${Geant4_LIBRARIES}")
#message(STATUS "ROOT_LIBRARIES = ${ROOT_LIBRARIES}")
#message(STATUS "HEPMC3_LIBRARIES = ${HEPMC3_LIBRARIES}")


# Put the compiled binary into the main GramsSim build directory.
# Without this statement, the binary would be placed in
# <build-directory>/GramsG4/gramsg4.

set_target_properties( gramsg4 
   PROPERTIES RUNTIME_OUTPUT_DIRECTORY 
   "${CMAKE_BINARY_DIR}" 
   )

# Copy the README.md file for the convenience of the user.
configure_file(README.md README.md COPYONLY)
