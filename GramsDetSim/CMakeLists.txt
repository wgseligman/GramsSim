# GramsDetSim

include_directories(include 
                    ${PROJECT_SOURCE_DIR}/util/include
                    ${ROOT_INCLUDE_DIR}
		    ${XercesC_INCLUDE_DIR})

file(GLOB DetSimSrc src/*.cc)

# Add an extension determined by GramsSim/CMakeLists.txt
set (PROG "gramsdetsim${EXE}")

add_executable(${PROG} gramsdetsim.cc ${DetSimSrc})

# Include any internal libraries, such as this project's utilities and
# its data objects.
target_link_libraries(${PROG} ${CMAKE_PROJECT_NAME}Utilities )
target_link_libraries(${PROG} Dictionary )
if (NOT MACOSX)
   # The following line is needed to make sure Dictionary is linked with the executable.
   target_link_options(${PROG} PRIVATE "LINKER:-no-as-needed")
endif()

target_link_libraries(${PROG} ${ROOT_LIBRARIES} )
target_link_libraries(${PROG} ${XercesC_LIBRARY} )

# Put the compiled binary into the main GramsSim build directory.
# Without this statement, the binary would be placed in
# <build-directory>/GramsDetSim/gramsdetsim.

set_target_properties( ${PROG} 
   PROPERTIES RUNTIME_OUTPUT_DIRECTORY 
   "${CMAKE_BINARY_DIR}" 
   )
