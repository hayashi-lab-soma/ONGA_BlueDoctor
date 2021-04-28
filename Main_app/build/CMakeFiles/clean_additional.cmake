# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "Release")
  file(REMOVE_RECURSE
  "CMakeFiles\\Main_app_autogen.dir\\AutogenUsed.txt"
  "CMakeFiles\\Main_app_autogen.dir\\ParseCache.txt"
  "Main_app_autogen"
  )
endif()
