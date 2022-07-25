cmake_minimum_required(VERSION 2.8...3.22)
project(pathTracer)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(USD_LIBRARY_DIRECTORY $ENV{USD_INSTALL_ROOT}/lib)
set(USD_INCLUDE_DIRECTORY $ENV{USD_INSTALL_ROOT}/include)

find_library(USD_SDF sdf HINTS ${USD_LIBRARY_DIRECTORY})
find_library(USD_TF tf HINTS ${USD_LIBRARY_DIRECTORY})
find_library(USD_USD usd HINTS ${USD_LIBRARY_DIRECTORY})
find_library(USD_USDGEOM usdGeom HINTS ${USD_LIBRARY_DIRECTORY})
find_library(USD_VT vt HINTS ${USD_LIBRARY_DIRECTORY})
find_library(USD_LIB_TBB tbb HINTS ${USD_LIBRARY_DIRECTORY})

file(GLOB_RECURSE SRCS sources/*.cpp sources/*.h)

add_executable(pathTracer ${SRCS})

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
        ${USD_LIB_TBB}
)


# Unittest
include(FetchContent)
FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG release-1.12.1
)

FetchContent_MakeAvailable(googletest)

enable_testing()

file(GLOB SOURCES_TESTS tests/*.cpp)

add_executable(
        runTests
        ${SOURCES_TESTS}
)

target_include_directories(
        runTests
        SYSTEM
        PUBLIC
        ${USD_INCLUDE_DIRECTORY}
        $ENV{EIGEN3_INCLUDE_DIR}
)


file(GLOB_RECURSE SOURCES_TO_TEST sources/*.cpp sources/*.h)
file(GLOB SOURCE_MAIN sources/main.cpp)

list(REMOVE_ITEM SOURCES_TO_TEST ${SOURCE_MAIN})

add_library(libToTest ${SOURCES_TO_TEST})

target_include_directories(
        libToTest
        SYSTEM
        PUBLIC
        ${USD_INCLUDE_DIRECTORY}
        $ENV{EIGEN3_INCLUDE_DIR}
)

target_link_libraries(
        runTests
        libToTest
        GTest::gtest_main
)

include(GoogleTest)

gtest_discover_tests(runTests)