# GramsDetSim

include_directories(include 
                    ${PROJECT_SOURCE_DIR}/util/include
                    ${ROOT_INCLUDE_DIR}
		    ${XercesC_INCLUDE_DIR})

file(GLOB DetSimSrc src/*.cc
                    ${PROJECT_SOURCE_DIR}/util/src/*.cc
    )
add_executable(gramsdetsim gramsdetsim.cc ${DetSimSrc})
target_link_libraries(gramsdetsim ${ROOT_LIBRARIES} )
target_link_libraries(gramsdetsim ${XercesC_LIBRARY} )

# Put the compiled binary into the main GramsSim build directory.
# Without this statement, the binary would be placed in
# <build-directory>/GramsDetSim/gramsdetsim.

set_target_properties( gramsdetsim 
   PROPERTIES RUNTIME_OUTPUT_DIRECTORY 
   "${CMAKE_BINARY_DIR}" 
   )

# Copy the README.md file for the convenience of the user.
configure_file(README.md README.md COPYONLY)
