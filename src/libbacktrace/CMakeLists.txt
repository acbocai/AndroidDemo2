cmake_minimum_required(VERSION 3.10.2)
project("backtrace")

SET(BACKTRACE_INCLUDE_DIR include)
LIST(APPEND BACKTRACE_INCLUDE_DIR
        ../../include/backtrace
        ../libbase/include
        ../libunwind/include)

#    add_subdirectory(${ROOT_DIR}/src/lzma/C lzmac-obj)
#    list(APPEND sources $<TARGET_OBJECTS:lzmac>)
#
#    add_subdirectory(${ROOT_DIR}/src/libunwind libunwind)
#    LIST(APPEND sources $<TARGET_OBJECTS:unwind_lib>)

set(sources
        Backtrace.cpp
        BacktraceCurrent.cpp
        BacktraceMap.cpp
        BacktracePtrace.cpp
        thread_utils.c
        ThreadEntry.cpp
        UnwindCurrent.cpp
        UnwindMap.cpp
        UnwindPtrace.cpp
        CallStack.cpp)

add_subdirectory(../lzma/C lzmac-obj)
list(APPEND sources $<TARGET_OBJECTS:lzmac>)
add_subdirectory(../libbase libbase-obj)
list(APPEND sources $<TARGET_OBJECTS:base_lib>)
add_subdirectory(../libunwind libunwind-obj)
LIST(APPEND sources $<TARGET_OBJECTS:unwind_lib>)

add_library(backtrace_lib
        STATIC
        ${sources}
        )

set(BACKTRACE_C_OPTIONS
        -Wall
        "-Werror"
        "-Wno-return-type"
        "-Wno-empty-body"
        "-Wno-self-assign")

if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    LIST(APPEND BACKTRACE_C_OPTIONS "-Wno-inline-asm")
endif()

target_compile_options(backtrace_lib
        PRIVATE
        ${BACKTRACE_C_OPTIONS}
)


target_include_directories(backtrace_lib PRIVATE ${BACKTRACE_INCLUDE_DIR})

SET_TARGET_PROPERTIES(backtrace_lib PROPERTIES OUTPUT_NAME backtrace)