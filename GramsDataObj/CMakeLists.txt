
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
set(Dictionary ${CMAKE_CURRENT_BINARY_DIR}/DataObjDict)

# Define the name for the data object library. This will have to be
# specified in every CMakeLists.txt file for an executable that
# accesses the data objects.

set(LibName ${CMAKE_PROJECT_NAME}DataObj)

# Create a ROOT dictionary using the headers and the LinkDef file.
# This ROOT CMake macro is incredibly picky. The order of these
# statements matters (first the macro, then add_library, then the rest).

ROOT_GENERATE_DICTIONARY ( ${Dictionary} ${headerList} LINKDEF ${LinkDef} )

# Add any source files, including the dictionary .cxx file, to the
# library dependencies.

add_library (${LibName} SHARED ${Dictionary}.cxx ${DataObjSrc})
#message(STATUS "GramsDataObj create library ${CMAKE_PROJECT_NAME}DataObj")
 
# Make sure the compiler can find include files for our library when
# other libraries or executables link to it.

target_include_directories (${LibName} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
