# 12-Jan-2021 WGS: Any lines which reference HepMC3 were copied
# from the BDSim Project, by the University of London. 

#----------------------------------------------------------------------------
# Setup the project
cmake_minimum_required(VERSION 3.6 FATAL_ERROR)

# Note: Do NOT make the project name the same as any executable
# name, including case. When the project was GramsG4 and the 
# main executable gramsg4, the build process would work on Linux
# but not on Mac OS. 
project(GramsSimProject)

# The above advice extends to subdirectories as well. Put in a test to
# check if we're building on Mac OS X Darwin.
if (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    set(MACOSX TRUE)
endif()

# For a Mac OS compilation, we have to avoid the program names having
# the same names as the subidrectories; e.g., in Linux we can have
# directory GramsSky and executable gramssky, but in Mac OS we
# can't. So define an extension that will have a value for Mac OS
# executables; e.g., gramssky.exe.
set(EXE "")
if (MACOSX)
   set (EXE ".exe")
endif()

# Add this project's cmake/Module directory as a source of our own CMake modules
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake/Modules)

# Include any special macros
include(macros)

#----------------------------------------------------------------------------
# Find the Geant4 package, activating all available Vis drivers by default
# You can set WITH_GEANT4_VIS to OFF via the command line or ccmake/cmake-gui
# to build a batch mode only executable
#
option(WITH_GEANT4_VIS "Build example with Geant4 Vis drivers" ON)
if(WITH_GEANT4_VIS)
  find_package(Geant4 COMPONENTS gdml vis_all)
else()
  find_package(Geant4 COMPONENTS gdml)
endif()

#----------------------------------------------------------------------------
# Setup Geant4 include directories and compile definitions
#
if (Geant4_FOUND)
   include(${Geant4_USE_FILE})
endif()

#----------------------------------------------------------------------------
# Set up ROOT
include(cmake/ROOT.cmake)

# Set up HEPMC3
include(cmake/HepMC.cmake)

# We're going to need the Xerces-C package for some programs.
find_package (XercesC REQUIRED)

# Set up the astrophysics libraries
include(cmake/FITS.cmake)

# Set the rpath so that the executables can find the libraries
# even if the environment setups have not been run in the shell.  
set(CMAKE_INSTALL_RPATH "${ROOT_LIBRARY_DIR}")
if (FITS_FOUND)
  list(APPEND CMAKE_INSTALL_RPATH "${FITS_LIBRARY_DIRS}")
endif()
message(STATUS "rpath = ${CMAKE_INSTALL_RPATH}")

#For debugging:
# Display all CMAKE variables
#get_cmake_property(_variableNames VARIABLES)
#list (SORT _variableNames)
#foreach (_variableName ${_variableNames})
#    message(STATUS "${_variableName}=${${_variableName}}")
#endforeach()

#----------------------------------------------------------------------------
# Copy all configuration files to the build directory, i.e. the directory in which we
# build GramsSim. This is so that we can run the executables directly because they
# relies on these scripts being in the current working directory.
#
set(GramsSim_SCRIPTS grams.gdml options.xml xinclude.xml README.md)

foreach(_script ${GramsSim_SCRIPTS})
  configure_file(
    ${PROJECT_SOURCE_DIR}/${_script}
    ${PROJECT_BINARY_DIR}/${_script}
    COPYONLY
    )
endforeach()

# Copy/compile the sub-projects and work files.
add_subdirectory(GramsSky)
add_subdirectory(GramsG4)
add_subdirectory(GramsDetSim)
add_subdirectory(GDMLSchema)
add_subdirectory(mac)
add_subdirectory(scripts)
