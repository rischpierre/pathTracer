cmake_minimum_required(VERSION 2.8...3.22)
project(pathTracer)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(USD_LIBRARY_DIRECTORY $ENV{USD_INSTALL_ROOT}/lib)
set(USD_INCLUDE_DIRECTORY $ENV{USD_INSTALL_ROOT}/include)

find_library(USD_SDF sdf HINTS /opt/USD/lib)
find_library(USD_TF tf HINTS ${USD_LIBRARY_DIRECTORY})
find_library(USD_USD usd HINTS ${USD_LIBRARY_DIRECTORY})
find_library(USD_USDGEOM usdGeom HINTS ${USD_LIBRARY_DIRECTORY})
find_library(USD_VT vt HINTS ${USD_LIBRARY_DIRECTORY})

file(GLOB_RECURSE SRCS src/*.cpp)
file(GLOB_RECURSE HDRS src/*.h)

add_executable(pathTracer ${SRCS} ${HDRS})

target_include_directories(
        pathTracer
        SYSTEM
        PUBLIC
        ${USD_INCLUDE_DIRECTORY}
        $ENV{EIGEN3_INCLUDE_DIR}
        )

target_link_libraries(
        pathTracer
        ${USD_SDF}
        ${USD_TF}
        ${USD_USDGEOM}
        ${USD_USD}
        ${USD_VT}
)