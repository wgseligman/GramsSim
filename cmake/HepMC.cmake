# 10-Jan-2021 WGS

# Quietly detect if we have HepMC library available - not required
find_package(HepMC3 3.2.0 REQUIRED)

if (HepMC3_FOUND)
  option( USE_HEPMC3 "Include HepMC3 library support." ON)
  option( USE_HEPMC3_ROOTIO "Use ROOT IO support of HEPMC3." ON)
  if (USE_HEPMC3)
    message(STATUS "Using HepMC3")
    include_directories(SYSTEM "${HEPMC3_INCLUDE_DIR}")
    # Set a preprocessor flag so that code can test if HepMC3 is available.
    add_definitions(-DHEPMC3_INSTALLED)
    
    # generally check if HepMC3 ROOTIO is available after the general package search
    # hepmc3 doesn't provide any great way of detecting components here
    if (DEFINED HEPMC3_ROOTIO_LIB)
      if (NOT HEPMC3_ROOTIO_LIB STREQUAL "HEPMC3_ROOTIO_LIB-NOTFOUND")
	add_definitions(-DHEPMC3_ROOTIO_INSTALLED)
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
else()
  option( USE_HEPMC3 "HepMC3 library not found - related functionality not compiled" OFF)
endif()
