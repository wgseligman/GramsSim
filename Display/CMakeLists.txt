
# Create ROOT dictionaries for this project's displays.
# 19-Aug-2024 WGS

# Some variables to ease typing.
set(PROG simd)

# Header files needed to compile the program. 
include_directories(include 
                    ${PROJECT_SOURCE_DIR}/util/include
                    ${PROJECT_SOURCE_DIR}/GramsDataObj/include
                    ${ROOT_INCLUDE_DIR}
                    ${XercesC_INCLUDE_DIR})

# The header file.
set(HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/include/SimulationDisplay.h)

# The source code to compile
set(SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/SimulationDisplay.cc )

# For ROOT's widget system (signals and slots) to work, the header
# files for the display programs have to be compiled into a ROOT
# dictionary.

ROOT_GENERATE_DICTIONARY(
        ${PROG}Dict
        ${HEADERS}
        LINKDEF ${LINKDEFH}
	OPTIONS
	   -s ${PROJECT_BINARY_DIR}/${PROG}Dict
)

if ( MACOSX )
   # The following line is not needed on Linux, but it allows for the
   # linking of libraries in Mac OS X Darwin. 
   link_libraries (${ROOT_LIBRARIES})
endif()

# Add any source files, including the dictionary .cxx file, to the
# library dependencies.
add_library(${PROG}Lib ${SOURCES} ${PROG}Dict.cxx)
target_link_libraries(${PROG}Lib PUBLIC ${ROOT_LIBRARIES})

# Define the executable and the dictionary library it requires.
add_executable(${PROG} simd.cc ${SOURCES})
target_link_libraries(${PROG} ${PROG}Lib)

# Include any other libraries, such as this project's utilities and
# its data objects.

target_link_libraries(${PROG} Utilities )
target_link_libraries(${PROG} Dictionary )
target_link_libraries(${PROG} ${ROOT_LIBRARIES} )
target_link_libraries(${PROG} ${XercesC_LIBRARY} )
if (NOT MACOSX)
   # The following line is needed to make sure Dictionary is linked with the executable.
   target_link_options(${PROG} PRIVATE "LINKER:-no-as-needed")
endif()

# Put the compiled binary into the main GramsSim build directory.
# Without this statement, the binary would be placed in
# <build-directory>/Display/simd.

set_target_properties( ${PROG} 
   PROPERTIES RUNTIME_OUTPUT_DIRECTORY 
   "${CMAKE_BINARY_DIR}" 
   )
