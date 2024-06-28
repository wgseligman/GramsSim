
# Create ROOT dictionaries for this project's data objects.
# 29-May-2024 WGS

# Locate all the header files (ending in .h) within the include
# directory. Note that 'LinkDef.hh' will therefore not be included in
# this list.

file(GLOB headerList ${CMAKE_CURRENT_SOURCE_DIR}/include/*.h)

# The source code to compile
file(GLOB DataObjSrc ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cc )

# To avoid typing:

set(LinkDef ${CMAKE_CURRENT_SOURCE_DIR}/include/LinkDef.hh)

# Define the name for the data object library. This will have to be
# specified in every CMakeLists.txt file for an executable that
# accesses the data objects.

set(LibName Dictionary)

# Create a ROOT dictionary using the headers and the LinkDef file.
# This ROOT CMake macro is incredibly picky. The order of these
# statements matters (first the macro, then add_library, then the rest).

# We want both the .so file and the .pcm to be in
# ${PROJECT_BINARY_DIR}, that is, the main build directory as seen by
# the user.

ROOT_GENERATE_DICTIONARY ( 
  ${CMAKE_CURRENT_BINARY_DIR}/Dictionary 
  ${headerList} 
  LINKDEF ${LinkDef} 
  OPTIONS -s ${PROJECT_BINARY_DIR}/Dictionary
)

# The following line is not needed on Linux, but it allows for the
# linking of libraries in Mac OS X Darwin. 
link_libraries (${ROOT_LIBRARIES})

# Add any source files, including the dictionary .cxx file, to the
# library dependencies.

add_library (${LibName} SHARED 
  ${CMAKE_CURRENT_BINARY_DIR}/Dictionary.cxx 
  ${DataObjSrc}
  )
#message(STATUS "GramsDataObj create library DataObj")
#message (STATUS "ROOT_LIBRARIES=${ROOT_LIBRARIES}")

# Make sure the compiler can find include files for our library when
# other libraries or executables link to it.

target_include_directories (${LibName} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

# Put the shared library and the .pcm into the main directory, to make
# it easier for the users.

set_target_properties( ${LibName}
  PROPERTIES LIBRARY_OUTPUT_DIRECTORY
  "${CMAKE_BINARY_DIR}"
  )
