cmake_minimum_required(VERSION 2.6)
project(dsf2flac)

# check OS before we start
if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    # On Linux a static executable is made, this maximises portability.
    set(static 0)

    # link to librt (possibly not required but need to check with compressed dsf before removing).
    set (link_rt 0)

elseif(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    # On OSX statically linked executables are not allowed so instead a dynamic one is made.
    set(static 0)
    # don't link to librt
    set (link_rt 0)

else()
    # Error if another system is detected.
    message( FATAL_ERROR "Sorry, your operating system is not supported yet.\nIt is probably not too hard to add support but you will need to add some details to CMakeLists.txt" )
endif()

# compiler options
set(CMAKE_CXX_FLAGS "-O3 -Wall")

# linker options
if ( static )
    set(CMAKE_EXE_LINKER_FLAGS "-static")
    set(Boost_USE_STATIC_LIBS   ON)
else()
    set(CMAKE_EXE_LINKER_FLAGS "")
endif()

# set location of cmake modules for finding required libs
set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmakemodules)

# find the required libraries
find_package(Boost REQUIRED COMPONENTS system timer filesystem chrono) 
find_package(Ogg REQUIRED)
find_package(Flac REQUIRED)
find_package(Id3 REQUIRED)
find_package(Z REQUIRED)
if ( link_rt )
    find_package(Rt REQUIRED)
endif()

# set the include directories
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src/libdstdec)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src/wave)
include_directories(${Boost_INCLUDE_DIRS})
include_directories(${Ogg_INCLUDE_DIRS})
include_directories(${Flac_INCLUDE_DIRS})
include_directories(${Id3_INCLUDE_DIRS})
include_directories(${Z_INCLUDE_DIRS})

# source file locations
set( DSF2FLAC_SOURCE_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/tagConversion.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/dop_packer.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/dsd_sample_reader.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/dsf_file_reader.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/filters.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/fstream_plus.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/dsd_decimator.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/dsdiff_file_reader.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/cmdline.c
)
set( LIBDSTDEC_SOURCE_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/libdstdec/ccp_calc.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/libdstdec/unpack_dst.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/libdstdec/dst_ac.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/libdstdec/dst_data.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/libdstdec/dst_fram.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/libdstdec/dst_init.c
)

# define the executable that is to be created.
add_executable(dsf2flac
    ${DSF2FLAC_SOURCE_FILES}
    ${LIBDSTDEC_SOURCE_FILES}
)

# set the libs for linking
target_link_libraries(dsf2flac
    ${Flac_LIBRARIES}
    ${Boost_LIBRARIES}
    ${Ogg_LIBRARIES}
    ${Id3_LIBRARIES}
    ${Z_LIBRARIES}
)
if ( link_rt )
    target_link_libraries(dsf2flac ${Rt_LIBRARIES})
endif()


