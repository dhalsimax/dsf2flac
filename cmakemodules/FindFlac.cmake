
find_path( Flac_INCLUDE_DIRS FLAC/metadata.h)
find_library( Flac_LIBRARY NAMES FLAC )
find_library( FlacXX_LIBRARY NAMES FLAC++ )


if (Flac_INCLUDE_DIRS AND Flac_LIBRARY AND FlacXX_LIBRARY)
   set(Flac_FOUND TRUE)
endif (Flac_INCLUDE_DIRS AND Flac_LIBRARY AND FlacXX_LIBRARY)

if (Flac_FOUND)
    message(STATUS "Found Flac: ${Flac_LIBRARY}")
    message(STATUS "Found Flac++: ${FlacXX_LIBRARY}")
    set(Flac_LIBRARIES ${Flac_LIBRARY} ${FlacXX_LIBRARY})
else (Flac_FOUND)
    message(FATAL_ERROR "Could NOT find Flac libraries")
endif (Flac_FOUND)

