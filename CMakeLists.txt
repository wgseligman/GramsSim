#----------------------------------------------------------------------------
# Setup the project
cmake_minimum_required(VERSION 2.8 FATAL_ERROR)
project(GramsG4)

# Add source dir as a place for CMake modules
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake/Modules)

# Include any special macros
include(macros)

#----------------------------------------------------------------------------
# Find Geant4 package, activating all available Vis drivers by default
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
# Set up ROOT
include(cmake/ROOT.cmake)

#----------------------------------------------------------------------------
# Setup Geant4 include directories and compile definitions
#
include(${Geant4_USE_FILE})

# Set up HEPMC3
include(cmake/HepMC.cmake)

#----------------------------------------------------------------------------
# Locate sources and headers for this project
#
include_directories(${PROJECT_SOURCE_DIR}/include 
                    ${PROJECT_SOURCE_DIR}/util/include
                    ${PROJECT_SOURCE_DIR}/g4util/include
                    ${Geant4_INCLUDE_DIR}
                    ${ROOT_INCLUDE_DIR})
file(GLOB sources ${PROJECT_SOURCE_DIR}/src/*.cc
                  ${PROJECT_SOURCE_DIR}/util/src/*.cc
                  ${PROJECT_SOURCE_DIR}/g4util/src/*.cc)
file(GLOB headers ${PROJECT_SOURCE_DIR}/include/*.hh
                  ${PROJECT_SOURCE_DIR}/util/include/*.h
                  ${PROJECT_SOURCE_DIR}/g4util/include/*.h)

#----------------------------------------------------------------------------
# Add the executable, and link it to the Geant4 libraries
#
add_executable(gramsg4 gramsg4.cc ${sources} ${headers})
target_link_libraries(gramsg4 ${Geant4_LIBRARIES} )

#----------------------------------------------------------------------------
# Copy all scripts to the build directory, i.e. the directory in which we
# build GramsG4. This is so that we can run the executable directly because it
# relies on these scripts being in the current working directory.
#
set(GramsG4_SCRIPTS grams.gdml options.xml)

foreach(_script ${GramsG4_SCRIPTS})
  configure_file(
    ${PROJECT_SOURCE_DIR}/${_script}
    ${PROJECT_BINARY_DIR}/${_script}
    COPYONLY
    )
endforeach()

add_subdirectory(GDMLSchema)
add_subdirectory(mac)
add_subdirectory(scripts)

#----------------------------------------------------------------------------
# Add program to the project targets
# (this avoids the need of typing the program name after make)
#
add_custom_target(GramsG4 DEPENDS gramsg4)

#----------------------------------------------------------------------------
# Install the executable to 'bin' directory under CMAKE_INSTALL_PREFIX
#
install(TARGETS gramsg4 DESTINATION bin)

# Set the compiler to the c++17 standard.  If I was more sophisticated
# with CMake scripting, I'd set this to something like "set this to
# c++17 or higher if our packages support it".  
set(CMAKE_CXX_STANDARD 17)
