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

# remove C++ standard flags from root library linking flags
string(REPLACE "-stdlib=libc++"  "" ROOT_LIBRARIES ${ROOT_LIBRARIES})

# 30-Dec-2021 WGS: Include the geometry libraries, so that ROOT
# programs can manage a detector geometry.
# 06-Jan-2022 WGS: Also include the EG library, so ROOT has access 
# to the particle database.
string(CONCAT ROOT_LIBRARIES ${ROOT_LIBRARIES} " -lGeom -lEG")
#message(STATUS "ROOT_LIBRARIES = ${ROOT_LIBRARIES}")

# fix the version number from root
# nice regex from CRMC pacakge in their search for ROOT also
STRING (REGEX REPLACE "[ \t\r\n]+" "" ROOT_VERSION "${ROOT_VERSION}")
STRING (REGEX REPLACE "/" "." ROOT_VERSION "${ROOT_VERSION}")
