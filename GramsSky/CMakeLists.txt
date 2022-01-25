# GramsSky

if (NOT HepMC3_FOUND)
  message(STATUS "HepMC3 not found; skipping GramsSky compilation.")
  return()
endif()

if (NOT FITS_FOUND)
  message(STATUS "FITSIO/HEALPix not found; map-based generators omitted.")
endif()

include_directories(SYSTEM
                    include
                    ${PROJECT_SOURCE_DIR}/util/include
)

file(GLOB SkySrc src/*.cc
                    ${PROJECT_SOURCE_DIR}/util/src/*.cc
    )

# Add an extension determined by GramsSim/CMakeLists.txt
set (PROG "gramssky${EXE}")

add_executable(${PROG} gramssky.cc ${SkySrc})
target_link_libraries(${PROG} ${ROOT_LIBRARIES} )
target_link_libraries(${PROG} ${XercesC_LIBRARIES} )
target_link_libraries(${PROG} ${HEPMC3_LIBRARIES} )
if (FITS_FOUND)
  target_link_libraries(${PROG} ${FITS_LDFLAGS} )
endif()

# Put the compiled binary into the main GramsSim build directory.
# Without this statement, the binary would be placed in
# <build-directory>/GramsSky/gramssky.

set_target_properties( ${PROG} 
   PROPERTIES RUNTIME_OUTPUT_DIRECTORY 
   "${CMAKE_BINARY_DIR}" 
   )

# Copy the README.md and diagram file for the convenience of the user.
configure_file(README.md README.md COPYONLY)
configure_file(SkyDiagram.jpg SkyDiagram.jpg COPYONLY)
