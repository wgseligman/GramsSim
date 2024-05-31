# GramsElecSim

include_directories(include 
                    ${PROJECT_SOURCE_DIR}/util/include
                    ${ROOT_INCLUDE_DIR}
		    ${XercesC_INCLUDE_DIR})

file(GLOB ElecSimSrc src/*.cc)

# Add an extension determined by GramsSim/CMakeLists.txt
set (PROG "gramselecsim${EXE}")

add_executable(${PROG} gramselecsim.cc ${ElecSimSrc})

# Include any internal libraries, such as this project's utilities and
# its data objects.
target_link_libraries(${PROG} ${CMAKE_PROJECT_NAME}Utilities )
target_link_libraries(${PROG} ${CMAKE_PROJECT_NAME}DataObj )

target_link_libraries(${PROG} ${ROOT_LIBRARIES} )
target_link_libraries(${PROG} ${XercesC_LIBRARY} )

# Put the compiled binary into the main GramsSim build directory.
# Without this statement, the binary would be placed in
# <build-directory>/GramsElecSim/gramselecsim.

set_target_properties( ${PROG} 
   PROPERTIES RUNTIME_OUTPUT_DIRECTORY 
   "${CMAKE_BINARY_DIR}" 
   )

# Copy a few files for the convenience of the user.
set(GramsElecSim_FILES README.md options.xml)

foreach(_file ${GramsElecSim_FILES})
  configure_file(
    ${_file}
    ${_file}
    COPYONLY
    )
endforeach()

