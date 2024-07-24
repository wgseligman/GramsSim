# Create a library for this project's utility code.

# The source code to compile
file(GLOB UtilSrc src/*.cc
                    ${PROJECT_SOURCE_DIR}/util/src/*.cc
    )

# The library's name will be libUtilities.so and will
# be located within the build directory. .
set(LibName Utilities)
add_library (${LibName} SHARED ${UtilSrc})

# Make sure the compiler can find include files for our library
# when other libraries or executables link to it.
target_include_directories (Utilities PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

# These utilities depend on ROOT and Xerces-C (for XML).

target_link_libraries(${LibName} ${ROOT_LIBRARIES} )
target_link_libraries(${LibName} ${XercesC_LIBRARIES} )

# Copy the showoptions script to the bin/ directory
# of the project being built.
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/showoptions ${CMAKE_BINARY_DIR}/bin/showoptions COPYONLY)


# Put the dictionary shared library into the main directory, to make
# it easier for the users.

set_target_properties( ${LibName}
  PROPERTIES LIBRARY_OUTPUT_DIRECTORY
  "${CMAKE_BINARY_DIR}"
  )
