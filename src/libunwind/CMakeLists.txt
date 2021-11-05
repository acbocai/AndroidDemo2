cmake_minimum_required(VERSION 3.10.2)
project("unwind")
option(UNWIND_DEBUG "Enable debug" OFF)
OPTION(UNWIND_LIB_BUILD_SHARED "Build unwind_lib as a shared library." OFF)
enable_language(C ASM)
set(UNWIND_ARCH "")
if(${CMAKE_ANDROID_ARCH} STREQUAL "arm64")
    set(UNWIND_ARCH "aarch64")
else()
    set(UNWIND_ARCH ${CMAKE_ANDROID_ARCH})
endif()

SET(UNWIND_INCLUDE_DIR_COMMON include)
LIST(APPEND UNWIND_INCLUDE_DIR_COMMON
        src
        ../lzma/C
        ../libbase/include)


set(UNWIND_C_COMPILE_OPTIONS
        -Wall
        "-Werror"
        "-Wno-old-style-cast"
        "-Wno-header-guard"
        "-Wno-unused-parameter"
        "-Wno-uninitialized"
        "-Wno-absolute-value"
        "-Wno-inline-asm"
        "-Wno-unused-function"
        "-Wno-unused-variable"
        )

set(UNWIND_FLAGS_COMMON
        -DHAVE_CONFIG_H
        -D_GNU_SOURCE
        )

if(UNWIND_DEBUG)
    LIST(APPED UNWIND_FLAGS_COMMON
            -DDEBUG
            -U_FORTIFY_SOURCE
            )
else()
    LIST(APPEND UNWIND_FLAGS_COMMON
            -DNDEBUG
            )
endif()

LIST(APPEND UNWIND_INCLUDE_DIR_COMMON "include/tdep-${UNWIND_ARCH}")

set(UNWIND_SOURCES_COMMON
        src/mi/init.c
        src/mi/flush_cache.c
        src/mi/mempool.c
        src/mi/strerror.c
        src/mi/backtrace.c
        src/mi/dyn-cancel.c
        src/mi/dyn-info-list.c
        src/mi/dyn-register.c
        src/mi/map.c
        src/mi/Lmap.c
        src/mi/Ldyn-extract.c
        src/mi/Lfind_dynamic_proc_info.c
        src/mi/Lget_proc_info_by_ip.c
        src/mi/Lget_proc_name.c
        src/mi/Lput_dynamic_unwind_info.c
        src/mi/Ldestroy_addr_space.c
        src/mi/Lget_reg.c
        src/mi/Lset_reg.c
        src/mi/Lget_fpreg.c
        src/mi/Lset_fpreg.c
        src/mi/Lset_caching_policy.c
        src/mi/Gdyn-extract.c
        src/mi/Gdyn-remote.c
        src/mi/Gfind_dynamic_proc_info.c
        src/mi/Gget_accessors.c
        src/mi/Gget_proc_info_by_ip.c
        src/mi/Gget_proc_name.c
        src/mi/Gput_dynamic_unwind_info.c
        src/mi/Gdestroy_addr_space.c
        src/mi/Gget_reg.c
        src/mi/Gset_reg.c
        src/mi/Gget_fpreg.c
        src/mi/Gset_fpreg.c
        src/mi/Gset_caching_policy.c
        src/dwarf/Lexpr.c
        src/dwarf/Lfde.c
        src/dwarf/Lparser.c
        src/dwarf/Lpe.c
        src/dwarf/Lstep_dwarf.c
        src/dwarf/Lfind_proc_info-lsb.c
        src/dwarf/Lfind_unwind_table.c
        src/dwarf/Gexpr.c
        src/dwarf/Gfde.c
        src/dwarf/Gfind_proc_info-lsb.c
        src/dwarf/Gfind_unwind_table.c
        src/dwarf/Gparser.c
        src/dwarf/Gpe.c
        src/dwarf/Gstep_dwarf.c
        src/dwarf/global.c
        src/os-common.c
        src/os-linux.c
        src/Los-common.c
        src/ptrace/_UPT_accessors.c
        src/ptrace/_UPT_access_fpreg.c
        src/ptrace/_UPT_access_mem.c
        src/ptrace/_UPT_access_reg.c
        src/ptrace/_UPT_create.c
        src/ptrace/_UPT_destroy.c
        src/ptrace/_UPT_find_proc_info.c
        src/ptrace/_UPT_get_dyn_info_list_addr.c
        src/ptrace/_UPT_put_unwind_info.c
        src/ptrace/_UPT_get_proc_name.c
        src/ptrace/_UPT_reg_offset.c
        src/ptrace/_UPT_resume.c
        )

LIST(APPEND UNWIND_SOURCES_COMMON
        "src/${UNWIND_ARCH}/is_fpreg.c"
        "src/${UNWIND_ARCH}/regname.c"
        "src/${UNWIND_ARCH}/Gcreate_addr_space.c"
        "src/${UNWIND_ARCH}/Gget_proc_info.c"
        "src/${UNWIND_ARCH}/Gget_save_loc.c"
        "src/${UNWIND_ARCH}/Gglobal.c"
        "src/${UNWIND_ARCH}/Ginit.c"
        "src/${UNWIND_ARCH}/Ginit_local.c"
        "src/${UNWIND_ARCH}/Ginit_remote.c"
        "src/${UNWIND_ARCH}/Gregs.c"
        "src/${UNWIND_ARCH}/Gresume.c"
        "src/${UNWIND_ARCH}/Gstep.c"
        "src/${UNWIND_ARCH}/Lcreate_addr_space.c"
        "src/${UNWIND_ARCH}/Lget_proc_info.c"
        "src/${UNWIND_ARCH}/Lget_save_loc.c"
        "src/${UNWIND_ARCH}/Lglobal.c"
        "src/${UNWIND_ARCH}/Linit.c"
        "src/${UNWIND_ARCH}/Linit_local.c"
        "src/${UNWIND_ARCH}/Linit_remote.c"
        "src/${UNWIND_ARCH}/Lregs.c"
        "src/${UNWIND_ARCH}/Lresume.c"
        "src/${UNWIND_ARCH}/Lstep.c"
        )


if(${CMAKE_ANDROID_ARCH} STREQUAL "arm")
    LIST(APPEND UNWIND_SOURCES_COMMON
            src/arm/getcontext.S
            src/arm/Gis_signal_frame.c
            src/arm/Gex_tables.c
            src/arm/Lis_signal_frame.c
            src/arm/Lex_tables.c
            src/elf32.c
            )
elseif(${CMAKE_ANDROID_ARCH} STREQUAL "arm64")
    LIST(APPEND UNWIND_SOURCES_COMMON
            src/aarch64/Gis_signal_frame.c
            src/aarch64/Lis_signal_frame.c
            src/elf64.c
            )
elseif(${CMAKE_ANDROID_ARCH} STREQUAL "x86")
    LIST(APPEND UNWIND_SOURCES_COMMON
            src/x86/getcontext-linux.S
            src/x86/Gos-linux.c
            src/x86/Los-linux.c
            src/elf32.c
            )
elseif(${CMAKE_ANDROID_ARCH} STREQUAL "x86_64")
    LIST(APPEND UNWIND_SOURCES_COMMON
            src/x86_64/getcontext.S
            src/x86_64/Gstash_frame.c
            src/x86_64/Gtrace.c
            src/x86_64/Gos-linux.c
            src/x86_64/Lstash_frame.c
            src/x86_64/Ltrace.c
            src/x86_64/Los-linux.c
            src/x86_64/setcontext.S
            src/elf64.c
            )
elseif(${CMAKE_ANDROID_ARCH} STREQUAL "mips")
    LIST(APPEND UNWIND_SOURCES_COMMON
            src/mips/getcontext-android.S
            src/mips/Gis_signal_frame.c
            src/mips/Lis_signal_frame.c
            src/elf32.c
            )

elseif(${CMAKE_ANDROID_ARCH} STREQUAL "mips64")
    LIST(APPEND UNWIND_INCLUDE_DIR_COMMON "include/tdep-mips")
    LIST(APPEND UNWIND_SOURCES_COMMON
            src/mips/getcontext-android.S
            src/mips/Gis_signal_frame.c
            src/mips/Lis_signal_frame.c
            src/elf64.c
            )
else()
    message(FATAL_ERROR "Unsupport platform: ${CMAKE_ANDROID_ARCH}")
endif()


###11111
#set(UNWIND_SHARED_DEPS "")
#if(UNWIND_DEBUG)
#    find_library(
#            log-lib
#            log )
#    LIST(APPEND UNWIND_SHARED_DEPS ${log-lib})
#endif()

#set(UNWIND_STATIC_DEPS "lzmac")

add_library(unwind_lib
        OBJECT
        ${UNWIND_SOURCES_COMMON})

target_include_directories(unwind_lib PRIVATE ${UNWIND_INCLUDE_DIR_COMMON})
add_definitions(${UNWIND_FLAGS_COMMON})

#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${UNWIND_FLAGS_COMMON} -fno-sanitize=all")

add_definitions("-DANDROID_ARM_MODE=arm")
####22222
#SET_TARGET_PROPERTIES(
#        unwind_lib
#        PROPERTIES LINK_FLAGS "-fno-sanitize=all")
target_compile_options(unwind_lib PRIVATE ${UNWIND_C_COMPILE_OPTIONS})