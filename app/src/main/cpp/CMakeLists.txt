# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html

# Sets the minimum version of CMake required to build the native library.

cmake_minimum_required(VERSION 3.10.2)

# Declares and names the project.

project("demo")

option(ENABLE_BACKTRACER "Enable backtrace" ON)
option(ENABLE_JSON_LIB "Enable json parser" ON)

string(TOLOWER "${CMAKE_BUILD_TYPE}" cmake_build_type_lower)

if(cmake_build_type_lower STREQUAL "release")
    message(VERBOSE "begin build release")
else()
    message(VERBOSE "begin build debug")
    add_definitions(-DENABLE_LOGGING)
endif()

if(NOT ANDROID_STL)
    set(ANDROID_STL "c++_static")
endif()

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fvisibility=hidden -fvisibility-inlines-hidden")

get_filename_component(PROJECT_ROOT_DIR
        ${CMAKE_CURRENT_SOURCE_DIR}/../../../.. ABSOLUTE)

set (ROOT_DIR ${PROJECT_ROOT_DIR})


set(PROJECT_DEP_LIB "")
set(sources
        LibEntry.cpp
        )

add_library(
        demo
        SHARED
        ${sources}
)

target_include_directories(demo PRIVATE
        include
        ${ROOT_DIR}/include
        )

if(ENABLE_JSON_LIB)
    add_subdirectory(${ROOT_DIR}/src/lib_json jsoncpp-bin)
    LIST(APPEND PROJECT_DEP_LIB jsoncpp_lib)
    add_definitions("-DENABLE_JSON_LIB")
endif()

if(ENABLE_BACKTRACER)
    add_subdirectory(${ROOT_DIR}/src/libbacktrace libbacktrace)
    LIST(APPEND PROJECT_DEP_LIB backtrace_lib)
    add_definitions("-DENABLE_BACKTRACER")
endif()

set_target_properties(
        demo
        PROPERTIES
        CXX_STANDARD 17
        CXX_STANDARD_REQUIRED YES
        CXX_EXTENSIONS NO
        )

SET_TARGET_PROPERTIES(
        demo
        PROPERTIES LINK_FLAGS "-Wl,--exclude-libs,ALL")

# Searches for a specified prebuilt library and stores the path as a
# variable. Because CMake includes system libraries in the search path by
# default, you only need to specify the key of the public NDK library
# you want to add. CMake verifies that the library exists before
# completing its build.

find_library(
        log-lib
        log )
LIST(APPEND PROJECT_DEP_LIB "${log-lib}")

target_link_libraries(
        demo
        ${PROJECT_DEP_LIB}
)

# Specifies libraries CMake should link to your target library. You
# can link multiple libraries, such as libraries you define in this
# build script, prebuilt third-party libraries, or system libraries.
