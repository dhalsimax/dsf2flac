
find_path(Z_INCLUDE_DIRS zlib.h)
find_library(Z_LIBRARIES NAMES z)

if (Z_INCLUDE_DIRS AND Z_LIBRARIES)
   set(Z_FOUND TRUE)
endif (Z_INCLUDE_DIRS AND Z_LIBRARIES)

if (Z_FOUND)
    message(STATUS "Found Zlib: ${Z_LIBRARIES}")
else (Z_FOUND)
    message(FATAL_ERROR "Could NOT find Zlib library")
endif (Z_FOUND)

