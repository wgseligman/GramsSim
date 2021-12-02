# 10-Jan-2021 WGS

# This script was copied from the Beam Delivery Simulation
# (BDSIM). The difference between that build script and the following
# is that HepMC is mandatory rather than optional.

# 10-Jan-2021 WGS: except that I've OFF to ON because for this
# distribution HepMC3 is required.

# quietly detect if we have HepMC library available - not required

option( USE_HEPMC3 "Include HepMC3 library support." ON)
option( USE_HEPMC3_ROOTIO "Use ROOT IO support of HEPMC3." ON)
if (USE_HEPMC3)
  message(STATUS "Using HepMC3")
  find_package(HepMC3 REQUIRED COMPONENTS HepMC3 HepMC3fio HINTS HepMC3_DIR)
  message(STATUS "HEPMC3 Use File: ${HEPMC3_ROOT_DIR}/share/HepMC3/cmake/HepMC3Config.cmake")
  include_directories(SYSTEM "${HEPMC3_INCLUDE_DIR}")
  add_definitions(-DUSE_HEPMC3)

  # generally check if HepMC3 ROOTIO is available after the general package search
  # hepmc3 doesn't provide any great way of detecting components here
  if (DEFINED HEPMC3_ROOTIO_LIB)
    if (NOT HEPMC3_ROOTIO_LIB STREQUAL "HEPMC3_ROOTIO_LIB-NOTFOUND")
      add_definitions(-DUSE_HEPMC3_ROOTIO)
      set(USE_HEPMC3_ROOTIO ON)	
      message(STATUS "Using HEPMC3 ROOTIO library")
      #message(STATUS ${HEPMC3_ROOTIO_LIB})
    else ()
      set(USE_HEPMC3_ROOTIO OFF)
    endif()
  else ()
    set(USE_HEPMC3_ROOTIO OFF)
  endif()

  mark_as_advanced(HEPMC3_SEARCH_LIB)
  mark_as_advanced(HEPMC3_LIB)
  mark_as_advanced(HEPMC3_ROOTIO_LIB)
endif()
