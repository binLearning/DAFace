cmake_minimum_required(VERSION 2.8)

project(daface)

option(USE_OPENMP   "Set to ON to build use openmp"   ON)
option(USE_SSE      "Set to ON to build use SSE"      ON)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2")

# Use C++11
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
message(STATUS "C++11 support has been enabled by default.")

# Use SSE
if (USE_SSE)
    add_definitions(-DUSE_SSE)
    message(STATUS "Use SSE")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -msse4.1")
endif()

# Use OpenMP
if (USE_OPENMP)
    find_package(OpenMP QUIET)
    if (OPENMP_FOUND)
        message(STATUS "Use OpenMP")
        add_definitions(-DUSE_OPENMP)
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
        set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
    endif()
endif()

include_directories(
  /usr/local/include
  /root/daface/3rdparty/boost_1_63_0
  /root/daface/include
)

link_directories(
  /usr/local/lib
  /root/daface/libs/seeta
)

set(src_files
  src/util/boost_app_common.cpp
  src/util/boost_app_fs.cpp
  src/util/boost_app_parse.cpp
  src/daface_det.cpp
  src/daface_clean.cpp
  src/daface_draw.cpp
  src/daface_crop.cpp
  src/daface_norm.cpp
  src/main.cpp
)

add_executable(daface ${src_files})

target_link_libraries(daface
  libopencv_core.so
  libopencv_highgui.so
  libopencv_imgproc.so
  #libopencv_objdetect.so
  #libopencv_video.so
  libseeta_facedet_lib.a
  libseeta_fa_lib.a
)
