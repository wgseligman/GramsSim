
# Create ROOT dictionaries for this project's displays.
# 19-Aug-2024 WGS

# For ROOT's widget system (signals and slots) to work, the header
# files for the display programs have to be compiled into a ROOT
# dictionary.

include_directories(include 
                    ${PROJECT_SOURCE_DIR}/util/include
                    ${ROOT_INCLUDE_DIR}
                    ${XercesC_INCLUDE_DIR})

set(headerList ${CMAKE_CURRENT_SOURCE_DIR}/include/SimulationDisplay.h)

# The source code to compile
set(DisplaySrc ${CMAKE_CURRENT_SOURCE_DIR}/src/SimulationDisplay.cc )

# To avoid typing:

set(LinkDef ${CMAKE_CURRENT_SOURCE_DIR}/include/LinkDef.hh)
set(SimdDict DisplayDictionary)
set(LibName ${SimdDict})
set (PROG "simd")

add_executable(${PROG} simd.cc ${DisplaySrc})
set_property(TARGET ${name} PROPERTY ENABLE_EXPORTS 1)

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

# Create a ROOT dictionary using the headers and the LinkDef file.

# Mac OS X has special runtime linking issues, and requires
# additional options. It also requires that a .rootmap file
# be present at a program's execution time. 
if ( MACOSX )

   ROOT_GENERATE_DICTIONARY ( 
      ${SimdDict} 
      ${headerList} 
      LINKDEF ${LinkDef}
      MODULE ${PROG}
#      OPTIONS
#	-s ${SimdDict}
#	-rmf ${SimdDict}.rootmap
#	-rml ${SimdDict}
   )	 

   # The following line is not needed on Linux, but it allows for the
   # linking of libraries in Mac OS X Darwin. 
   link_libraries (${ROOT_LIBRARIES})

else()

   ROOT_GENERATE_DICTIONARY ( 
      ${SimdDict} 
      ${headerList} 
      LINKDEF ${LinkDef}
      MODULE ${PROG}
#      OPTIONS
#	-s ${SimdDict}
   )
   
endif()

# Add any source files, including the dictionary .cxx file, to the
# library dependencies.

#add_library (${LibName} SHARED 
#  ${CMAKE_CURRENT_BINARY_DIR}/${SimdDict}.cxx 
#  ${DisplaySrc}
#  )
message(STATUS "Display create library ${LibName}")
message (STATUS "ROOT_LIBRARIES=${ROOT_LIBRARIES}")

# Make sure the compiler can find include files for our library when
# other libraries or executables link to it.

#target_include_directories (${LibName} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

# Put the compiled binary into the main GramsSim build directory.
# Without this statement, the binary would be placed in
# <build-directory>/Display/simd.

set_target_properties( ${PROG} 
   PROPERTIES RUNTIME_OUTPUT_DIRECTORY 
   "${CMAKE_BINARY_DIR}" 
   )
