# 11-Jan-2021 WGS: Taken from BDSim.

# taken from
# https://stackoverflow.com/questions/39758585/duplicate-compile-flag-in-cmake-cxx-flags
function(removeDuplicateSubstring stringIn stringOut)
    separate_arguments(stringIn)
    list(REMOVE_DUPLICATES stringIn)
    string(REPLACE ";" " " stringIn "${stringIn}")
    set(${stringOut} "${stringIn}" PARENT_SCOPE)
endfunction()

function(removeCXXStandardFlags stringIn stringOut)
  # remove -std=c++11 or -std=gnu++11 etc including ++1a 1y 1z etc
  # match suffix of one letter of lower case a to z
  string(REGEX REPLACE "\\-std=(c|gnu)\\+\\+[0-9]+[a-z]?" "" _TMPV "${stringIn}")
  set(CMAKE_CXX_FLAGS "${_TMPV}" PARENT_SCOPE)
  if($ENV{VERBOSE})
    message(STATUS "CMAKE_CXX_FLAGS after  ${_TMPV}")
  endif()
  unset(_TMPV)
endfunction()
