# Create a library for this project's utility code.

# The source code to compile
file(GLOB UtilSrc src/*.cc
                    ${PROJECT_SOURCE_DIR}/util/src/*.cc
    )

# The library's name will be lib(project)Utilities and will
# be located in /util within the build directory. .
set(LibName ${CMAKE_PROJECT_NAME}Utilities)
add_library (${LibName} SHARED ${UtilSrc})

# Make sure the compiler can find include files for our library
# when other libraries or executables link to it.
target_include_directories (${CMAKE_PROJECT_NAME}Utilities PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

# These utilities depend on ROOT and Xerces-C (for XML).

target_link_libraries(${LibName} ${ROOT_LIBRARIES} )
target_link_libraries(${LibName} ${XercesC_LIBRARIES} )


