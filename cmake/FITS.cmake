# 12-Jan-2022

# Set up variables for the cfitsio and healpix libraries.  They're
# mainly needed for GramsSky, which needs to read in sky maps.
pkg_check_modules (FITS cfitsio healpix_cxx)

include_directories(SYSTEM ${FITS_INCLUDE_DIRS})
