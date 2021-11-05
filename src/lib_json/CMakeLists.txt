cmake_minimum_required(VERSION 3.10.2)
project("jsoncpp")
OPTION(JSONCPP_LIB_BUILD_SHARED "Build jsoncpp_lib as a shared library." OFF)
IF(JSONCPP_LIB_BUILD_SHARED)
  SET(JSONCPP_LIB_TYPE SHARED)
  ADD_DEFINITIONS( -DJSON_DLL_BUILD )
ELSE(JSONCPP_LIB_BUILD_SHARED)
  SET(JSONCPP_LIB_TYPE STATIC)
ENDIF(JSONCPP_LIB_BUILD_SHARED)

if( CMAKE_COMPILER_IS_GNUCXX )
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror=strict-aliasing")
endif( CMAKE_COMPILER_IS_GNUCXX )

SET( JSONCPP_INCLUDE_DIR ../../include )

add_library(jsoncpp_lib
        ${JSONCPP_LIB_TYPE}
        json_tool.h
        json_reader.cpp
        json_batchallocator.h
        json_valueiterator.inl
        json_value.cpp
        json_writer.cpp
        version.h.in
        )

target_include_directories(jsoncpp_lib PRIVATE ${JSONCPP_INCLUDE_DIR})

add_definitions(-DJSON_USE_EXCEPTION=0)

target_compile_options(jsoncpp_lib
        PRIVATE
        -Wall
        "-Werror"
        -Wno-implicit-fallthrough
        -Ofast
        )

SET_TARGET_PROPERTIES( jsoncpp_lib PROPERTIES OUTPUT_NAME jsoncpp )
SET_TARGET_PROPERTIES( jsoncpp_lib PROPERTIES VERSION ${JSONCPP_VERSION} SOVERSION ${JSONCPP_VERSION_MAJOR} )

# Install instructions for this target
IF(JSONCPP_WITH_CMAKE_PACKAGE)
    TARGET_INCLUDE_DIRECTORIES( jsoncpp_lib
                                PUBLIC $<INSTALL_INTERFACE:${INCLUDE_INSTALL_DIR}>
                                       $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/${JSON_INCLUDE_DIR}>
                                )
    SET(INSTALL_EXPORT EXPORT jsoncpp)
ELSE(JSONCPP_WITH_CMAKE_PACKAGE)
    SET(INSTALL_EXPORT)
ENDIF(JSONCPP_WITH_CMAKE_PACKAGE)