project(googleTests)

add_subdirectory(lib/googletest)
include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})

file(GLOB TESTS_SOURCES "*.cpp" )

add_executable(runTests ${TESTS_SOURCES})
add_compile_options(-Wall -Wextra -pedantic -Werror)


# todo use project path instead
file(GLOB_RECURSE SRCS "{PROJECT_SOURCE_DIR}/../src/*.cpp" "{PROJECT_SOURCE_DIR}/../src/*.h")


list(FILTER SRCS EXCLUDE REGEX "src/main.cpp")

set(USD_LIBRARY_DIRECTORY $ENV{USD_INSTALL_ROOT}/lib)
set(USD_INCLUDE_DIRECTORY $ENV{USD_INSTALL_ROOT}/include)

find_library(USD_SDF sdf HINTS ${USD_LIBRARY_DIRECTORY})
find_library(USD_TF tf HINTS ${USD_LIBRARY_DIRECTORY})
find_library(USD_USD usd HINTS ${USD_LIBRARY_DIRECTORY})
find_library(USD_USDGEOM usdGeom HINTS ${USD_LIBRARY_DIRECTORY})
find_library(USD_VT vt HINTS ${USD_LIBRARY_DIRECTORY})

target_include_directories(
        runTests
        PUBLIC
        ${SRCS}
        ${USD_INCLUDE_DIRECTORY}
        $ENV{EIGEN3_INCLUDE_DIR}
)

target_link_libraries(
        runTests
        ${USD_SDF}
        ${USD_TF}
        ${USD_USDGEOM}
        ${USD_USD}
        ${USD_VT}
)

target_link_libraries(runTests gtest gtest_main)
