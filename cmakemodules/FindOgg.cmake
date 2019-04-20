
find_path(Ogg_INCLUDE_DIRS ogg/ogg.h)
find_library(Ogg_LIBRARIES NAMES ogg)

if (Ogg_INCLUDE_DIRS AND Ogg_LIBRARIES)
   set(Ogg_FOUND TRUE)
endif (Ogg_INCLUDE_DIRS AND Ogg_LIBRARIES)

if (Ogg_FOUND)
    message(STATUS "Found Ogg: ${Ogg_LIBRARIES}")
else (Ogg_FOUND)
    message(FATAL_ERROR "Could NOT find Ogg library")
endif (Ogg_FOUND)

