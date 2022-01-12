# GramsSky

include_directories(SYSTEM
                    include
                    ${PROJECT_SOURCE_DIR}/util/include
#                    ${ROOT_INCLUDE_DIR}
#		    ${XercesC_INCLUDE_DIR}
#		    ${FITS_INCLUDE_DIRS}
)

file(GLOB SkySrc src/*.cc
                    ${PROJECT_SOURCE_DIR}/util/src/*.cc
    )

add_executable(gramssky gramssky.cc ${SkySrc})
target_link_libraries(gramssky ${ROOT_LIBRARIES} )
target_link_libraries(gramssky ${XercesC_LIBRARY} )
target_link_libraries(gramssky ${HEPMC3_LIBRARIES} )
target_link_libraries(gramssky ${FITS_LDFLAGS} )

# Put the compiled binary into the main GramsSim build directory.
# Without this statement, the binary would be placed in
# <build-directory>/GramsSky/gramssky.

set_target_properties( gramssky 
   PROPERTIES RUNTIME_OUTPUT_DIRECTORY 
   "${CMAKE_BINARY_DIR}" 
   )

# Copy the README.md and diagram file for the convenience of the user.
configure_file(README.md README.md COPYONLY)
configure_file(SkyDiagram.jpg SkyDiagram.jpg COPYONLY)
