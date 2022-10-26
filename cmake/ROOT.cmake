set(ROOTSYS "" CACHE PATH "Which ROOT installation to use.")

if (NOT ROOTSYS STREQUAL "")
  message(STATUS "Using user-specified ROOT: ${ROOTSYS}")
elseif (DEFINED ENV{ROOTSYS})
  message(STATUS "Use ROOTSYS from environment: $ENV{ROOTSYS}")
  set (ROOTSYS "$ENV{ROOTSYS}" CACHE PATH "Which ROOT installation to use." FORCE)
endif()

# find ROOT of at least version 6
find_package(ROOT 6.0 REQUIRED)

# now remove any duplicates we have to keep things tidy
removeDuplicateSubstring("${CMAKE_CXX_FLAGS}" $CMAKE_CXX_FLAGS)
 
# ROOT doesn't implement the version and subversion number in CMAKE as it should, so
# the above find package doesn't match the version required. Need to decode version ourselves
if (ROOT_VERSION VERSION_LESS "6.00")
  message(FATAL_ERROR "ROOT Version 6 required. Version ${ROOT_MAJOR_VERSION} found")
endif()

# add ROOT include directory
include_directories(SYSTEM ${ROOT_INCLUDE_DIR})

option(ROOT_DOUBLE_OUTPUT "Double precision root output" OFF)
if(ROOT_DOUBLE_OUTPUT)
  add_definitions("-D__ROOTDOUBLE__")
  set(PREPROCESSOR_DEFS "-D__ROOTBUILD__;-D__ROOTDOUBLE__")
else()
  set(PREPROCESSOR_DEFS "-D__ROOTBUILD__")
endif()

# Make Dictionaries
file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/root)
file(GLOB linkHeaders ${CMAKE_CURRENT_SOURCE_DIR}/include/*LinkDef.hh)
# for loop over link definitions
foreach(header ${linkHeaders})
  # remove LinkDef.hh
  string(FIND ${header} "LinkDef.hh" pos REVERSE)
  string(FIND ${header} "/" dir REVERSE)
  MATH(EXPR beginpos "${dir}+1")
  MATH(EXPR length "${pos}-${beginpos}")
  string(SUBSTRING ${header} ${beginpos} ${length} className)
  #message(STATUS "${ROOTCINT_EXECUTABLE} -f ${CMAKE_CURRENT_BINARY_DIR}/root/${className}Dict.cc -c ${CMAKE_CURRENT_SOURCE_DIR}/include/${className}.hh ${header}")

  # add preprocessor flag __ROOTBUILD__ as used in BDSIM.
  # root6 .pcm file needs to be at library level
  # for discussion see e.g. this forum topic: https://root.cern.ch/phpBB3/viewtopic.php?t=17654
  add_custom_command(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/root/${className}Dict.cc
    ${CMAKE_CURRENT_BINARY_DIR}/root/${className}Dict_rdict.pcm
    COMMAND ${ROOTCINT_EXECUTABLE}
    ARGS -f ${CMAKE_CURRENT_BINARY_DIR}/root/${className}Dict.cc -noIncludePaths -inlineInputHeader
    ${PREPROCESSOR_DEFS}  -I${CMAKE_CURRENT_SOURCE_DIR} -I${CMAKE_CURRENT_SOURCE_DIR}/include -I${CMAKE_CURRENT_SOURCE_DIR}/parser/
    ${CMAKE_CURRENT_SOURCE_DIR}/include/${className}.hh ${header}
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/include/${className}.hh ${header}
    IMPLICIT_DEPENDS CXX ${CMAKE_CURRENT_SOURCE_DIR}/include/${className}.hh ${header}
    COMMENT "Generate ROOT Dictionary for ${className}"
    )
  add_custom_command(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/${className}Dict_rdict.pcm
    COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_BINARY_DIR}/root/${className}Dict_rdict.pcm
    ${CMAKE_CURRENT_BINARY_DIR}
    DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/root/${className}Dict.cc
    )
  # add arbitrary dependence to make it run at compile time (since DEPENDS at custom_command seems not to work)
  set_property(
    SOURCE ${CMAKE_CURRENT_BINARY_DIR}/root/${className}Dict.cc
    APPEND PROPERTY OBJECT_DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/${className}Dict_rdict.pcm
    )
  # keep list of ROOT pcms
  set(root_pcms ${root_pcms} ${CMAKE_CURRENT_BINARY_DIR}/${className}Dict_rdict.pcm)

  # keep list of ROOT dictionaries
  set(root_files ${root_files} ${CMAKE_CURRENT_SOURCE_DIR}/src/${className}.cc)
  set(root_dicts ${root_dicts} ${CMAKE_CURRENT_BINARY_DIR}/root/${className}Dict.cc)
endforeach()

# remove C++ standard flags from root library linking flags
string(REPLACE "-stdlib=libc++"  "" ROOT_LIBRARIES ${ROOT_LIBRARIES})

# 30-Dec-2021 WGS: Include the geometry libraries, so that ROOT
# programs can manage a detector geometry.
# 06-Jan-2022 WGS: Also include the EG library, so ROOT has access 
# to is particle database.
string(CONCAT ROOT_LIBRARIES ${ROOT_LIBRARIES} " -lGeom -lEG")
#message(STATUS "ROOT_LIBRARIES = ${ROOT_LIBRARIES}")

# fix the version number from root
# nice regex from CRMC pacakge in their search for ROOT also
STRING (REGEX REPLACE "[ \t\r\n]+" "" ROOT_VERSION "${ROOT_VERSION}")
STRING (REGEX REPLACE "/" "." ROOT_VERSION "${ROOT_VERSION}")
