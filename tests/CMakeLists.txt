project(Google_tests)

add_subdirectory(lib/googletest)
include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})

file(GLOB TESTS_SOURCES "*.cpp" )

add_executable(Google_Tests_run ${TESTS_SOURCES})
add_compile_options(-Wall -Wextra -pedantic -Werror)

file(GLOB_RECURSE SRCS ../src/*.cpp ../src/*.h)
list(FILTER SRCS EXCLUDE REGEX "src/main.cpp")

set(USD_LIBRARY_DIRECTORY $ENV{USD_INSTALL_ROOT}/lib)
set(USD_INCLUDE_DIRECTORY $ENV{USD_INSTALL_ROOT}/include)

find_library(USD_SDF sdf HINTS /opt/USD/lib)
find_library(USD_TF tf HINTS ${USD_LIBRARY_DIRECTORY})
find_library(USD_USD usd HINTS ${USD_LIBRARY_DIRECTORY})
find_library(USD_USDGEOM usdGeom HINTS ${USD_LIBRARY_DIRECTORY})
find_library(USD_VT vt HINTS ${USD_LIBRARY_DIRECTORY})

target_include_directories(
        Google_Tests_run
        PUBLIC
        ${SRCS}
        ${USD_INCLUDE_DIRECTORY}
        $ENV{EIGEN3_INCLUDE_DIR}
)

message(${USD_USD})
target_link_libraries(
        Google_Tests_run
        ${SRCS}
        ${USD_SDF}
        ${USD_TF}
        ${USD_USDGEOM}
        ${USD_USD}
        ${USD_VT}
)

target_link_libraries(Google_Tests_run gtest gtest_main)
