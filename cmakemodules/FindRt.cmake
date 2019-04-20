
find_library(Rt_LIBRARIES NAMES rt)

if (Rt_LIBRARIES)
   set(Rt_FOUND TRUE)
endif (Rt_LIBRARIES)

if (Rt_FOUND)
    message(STATUS "Found Rtlib: ${Rt_LIBRARIES}")
else (Rt_FOUND)
    message(FATAL_ERROR "Could NOT find Rt library")
endif (Rt_FOUND)

