
find_path(Id3_INCLUDE_DIRS id3.h)
find_library(Id3_LIBRARIES NAMES id3)

if (Id3_INCLUDE_DIRS AND Id3_LIBRARIES)
   set(Id3_FOUND TRUE)
endif (Id3_INCLUDE_DIRS AND Id3_LIBRARIES)

if (Id3_FOUND)
    message(STATUS "Found Id3: ${Id3_LIBRARIES}")
else (Id3_FOUND)
    message(FATAL_ERROR "Could NOT find Id3 library")
endif (Id3_FOUND)

