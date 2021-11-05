cmake_minimum_required(VERSION 3.10.2)
project("libbase")

SET(BASE_INCLUDE_DIR include)

SET(BASELIB_SOURCES
        file.cpp
        logging.cpp
        parsenetaddress.cpp
        stringprintf.cpp
        strings.cpp
        test_utils.cpp
        )

if(ANDROID_PLATFORM)
    LIST(APPEND BASELIB_SOURCES errors_unix.cpp)
endif()


add_library(base_lib OBJECT ${BASELIB_SOURCES})

set(BASELIB_C_OPTIONS
        -Wall
        "-Wextra"
        "-Werror"
        "-Wno-empty-body"
        "-Wno-self-assign")

if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    LIST(APPEND BASELIB_C_OPTIONS "-Wexit-time-destructors")
endif()

target_compile_options(base_lib
        PRIVATE
        ${BASELIB_C_OPTIONS}
        )

target_include_directories(base_lib PRIVATE ${BASE_INCLUDE_DIR})

#SET_TARGET_PROPERTIES(base_lib PROPERTIES OUTPUT_NAME base)