# 12-Jan-2022

# Set up variables for the cfitsio and healpix libraries.  They're
# mainly needed for GramsSky, which needs to read in sky maps.
pkg_check_modules (FITS cfitsio healpix_cxx)

#For debugging:
#message(STATUS "FITS_CFLAGS = ${FITS_CFLAGS}")
#message(STATUS "FITS_LDFLAGS = ${FITS_LDFLAGS}")