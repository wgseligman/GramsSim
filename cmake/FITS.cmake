# 12-Jan-2022

# Set up variables for the cfitsio and healpix libraries.  They're
# mainly needed for GramsSky, which needs to read in sky maps.
find_package(PkgConfig)
pkg_check_modules(FITS QUIET cfitsio healpix_cxx)

if (FITS_FOUND)
  message(STATUS "Using cfitsio/healpix_cxx")
  include_directories(SYSTEM ${FITS_INCLUDE_DIRS})
  # Set a preprocessor flag that the code can use to test if we've got
  # healpix_cxx.
  add_definitions(-DHEALPIX_INSTALLED)
else()
  message(STATUS "cfitsio/healpix_cxx not found")
endif()
