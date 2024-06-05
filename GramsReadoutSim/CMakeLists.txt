# GramsReadoutSim

include_directories(include 
                    ${PROJECT_SOURCE_DIR}/util/include
                    ${ROOT_INCLUDE_DIR}
		    ${XercesC_INCLUDE_DIR})

file(GLOB DetReadoutSrc src/*.cc)

# Add an extension determined by GramsSim/CMakeLists.txt
set (PROG "gramsreadoutsim${EXE}")

add_executable(${PROG} gramsreadoutsim.cc ${DetReadoutSrc})

# Include any internal libraries, such as this project's utilities and
# its data objects.
target_link_libraries(${PROG} ${CMAKE_PROJECT_NAME}Utilities )
target_link_libraries(${PROG} ${CMAKE_PROJECT_NAME}DataObj )
# The following line is needed to make sure DataObj is linked with the executable.
target_link_options(${PROG} PRIVATE "LINKER:-no-as-needed")

target_link_libraries(${PROG} ${ROOT_LIBRARIES} )
target_link_libraries(${PROG} ${XercesC_LIBRARY} )

# Put the compiled binary into the main GramsSim build directory.
# Without this statement, the binary would be placed in
# <build-directory>/GramsReadoutSim/gramsreadoutsim.

set_target_properties( ${PROG} 
   PROPERTIES RUNTIME_OUTPUT_DIRECTORY 
   "${CMAKE_BINARY_DIR}" 
   )

# Copy a few files for the convenience of the user.
set(GramsReadoutSim_FILES README.md options.xml)

foreach(_file ${GramsReadoutSim_FILES})
  configure_file(
    ${_file}
    ${_file}
    COPYONLY
    )
endforeach()

