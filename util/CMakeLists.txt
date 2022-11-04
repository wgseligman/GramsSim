# Create a library for this project's utility code.

# The source code to compile
file(GLOB UtilSrc src/*.cc
                    ${PROJECT_SOURCE_DIR}/util/src/*.cc
    )

# The library's name will be lib(project)Utilities and will
# be located in /util within the build directory. .
add_library (${CMAKE_PROJECT_NAME}Utilities SHARED ${UtilSrc})

# Make sure the compiler can find include files for our library
# when other libraries or executables link to it.
target_include_directories (${CMAKE_PROJECT_NAME}Utilities PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)


