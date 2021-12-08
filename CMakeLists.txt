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
  find_package(Geant4 REQUIRED gdml vis_all)
else()
  find_package(Geant4 REQUIRED gdml)
endif()

#----------------------------------------------------------------------------
# Setup Geant4 include directories and compile definitions
#
include(${Geant4_USE_FILE})


#----------------------------------------------------------------------------
# Set up ROOT
include(cmake/ROOT.cmake)

# Set up HEPMC3
include(cmake/HepMC.cmake)

# We're going to need the Xerces-C package for some programs.
find_package (XercesC REQUIRED)

# 11-Jan-2021 WGS: Again, copied from BDSim.
# rpath for libraries to work from installation correctly

# Include the Geant4 libraries.
if (USE_HEPMC3)
  set(CMAKE_INSTALL_RPATH "${ROOT_LIBRARY_DIR};${HEPMC3_ROOT_DIR}/lib;${Geant4_LIBRARY_DIR};${CMAKE_INSTALL_PREFIX}/lib")
  message(STATUS "rpath = ${CMAKE_INSTALL_RPATH}")
else()
  set(CMAKE_INSTALL_RPATH "${ROOT_LIBRARY_DIR};${Geant4_LIBRARY_DIR};${CMAKE_INSTALL_PREFIX}/lib")
  message(STATUS "rpath = ${CMAKE_INSTALL_RPATH}")
endif()

#----------------------------------------------------------------------------
# Copy all scripts to the build directory, i.e. the directory in which we
# build GramsG4. This is so that we can run the executable directly because it
# relies on these scripts being in the current working directory.
#
set(GramsG4_SCRIPTS grams.gdml options.xml README.md)

foreach(_script ${GramsG4_SCRIPTS})
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
