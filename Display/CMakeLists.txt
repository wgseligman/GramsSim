
# Create ROOT dictionaries for this project's displays.
# 19-Aug-2024 WGS

set(PROG simd)

# For ROOT's widget system (signals and slots) to work, the header
# files for the display programs have to be compiled into a ROOT
# dictionary.

include_directories(include 
                    ${PROJECT_SOURCE_DIR}/util/include
                    ${PROJECT_SOURCE_DIR}/GramsDataObj/include
                    ${ROOT_INCLUDE_DIR}
                    ${XercesC_INCLUDE_DIR})

# The header file.
set(HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/include/SimulationDisplay.h)

# The source code to compile
set(SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/SimulationDisplay.cc )

# Create a ROOT dictionary using the headers and the LinkDef file.

ROOT_GENERATE_DICTIONARY(D_${PROG}
        ${HEADERS}
        LINKDEF ${LINKDEFH}
	OPTIONS
	   -s ${PROJECT_BINARY_DIR}/D_${PROG}
)

if ( MACOSX )
   # The following line is not needed on Linux, but it allows for the
   # linking of libraries in Mac OS X Darwin. 
   link_libraries (${ROOT_LIBRARIES})
endif()

# Add any source files, including the dictionary .cxx file, to the
# library dependencies.
add_library(SO_${PROG} ${SOURCES} D_${PROG}.cxx)
target_link_libraries(SO_${PROG} PUBLIC ${ROOT_LIBRARIES})

add_executable(${PROG} simd.cc ${SOURCES})
#set_property(TARGET ${PROG} PROPERTY ENABLE_EXPORTS 1)
target_link_libraries(${PROG} SO_${PROG})

# Include any internal libraries, such as this project's utilities and
# its data objects.
target_link_libraries(${PROG} Utilities )
target_link_libraries(${PROG} Dictionary )
if (NOT MACOSX)
   # The following line is needed to make sure Dictionary is linked with the executable.
   target_link_options(${PROG} PRIVATE "LINKER:-no-as-needed")
endif()

target_link_libraries(${PROG} ${ROOT_LIBRARIES} )
target_link_libraries(${PROG} ${XercesC_LIBRARY} )

message (STATUS "ROOT_LIBRARIES=${ROOT_LIBRARIES}")

# Put the compiled binary into the main GramsSim build directory.
# Without this statement, the binary would be placed in
# <build-directory>/Display/simd.

set_target_properties( ${PROG} 
   PROPERTIES RUNTIME_OUTPUT_DIRECTORY 
   "${CMAKE_BINARY_DIR}" 
   )
#install(FILES ${PROJECT_BINARY_DIR}/Display/libD_${PROG}_rdict.pcm
#        DESTINATION "${CMAKE_BINARY_DIR}")

#message (STATUS "CMAKE_BINARY_DIR=${CMAKE_BINARY_DIR}")
#message (STATUS "PROJECT_BINARY_DIR=${PROJECT_BINARY_DIR}")
