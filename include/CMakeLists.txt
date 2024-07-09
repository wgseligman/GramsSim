# Install include files from utility sub-directories.

# 09-Jul-2024 WGS: To make it easier to run the example scripts, copy
# any header files they might need. Note that the only headers they'll
# need are those for the utilities and for the data objects. 

file(COPY
  ${PROJECT_SOURCE_DIR}/util/include/
  ${PROJECT_SOURCE_DIR}/GramsDataObj/include/
  DESTINATION ${CMAKE_BINARY_DIR}/include
  FILES_MATCHING PATTERN "*.h"
  )
      
