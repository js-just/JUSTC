# MIT License
#
# Copyright (c) 2025 JustStudio. <https://juststudio.is-a.dev/>
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.

cmake_minimum_required(VERSION 3.10)
project(JUSTC)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(ProcessorCount)
ProcessorCount(NUM_PROCESSORS)
if(NUM_PROCESSORS EQUAL 0)
    set(NUM_PROCESSORS 1)
endif()

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2")
if(UNIX AND NOT APPLE)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pipe")
endif()

if(UNIX AND NOT APPLE)
    set(CMAKE_INSTALL_PREFIX "/usr/local" CACHE PATH "Installation prefix")
endif()

include(FetchContent)

find_package(cpr QUIET)

if(NOT cpr_FOUND)
    message(STATUS "CPR not found in system, building from source...")
    if(CMAKE_VERSION VERSION_GREATER_EQUAL 3.14)
        FetchContent_Declare(cpr
            GIT_REPOSITORY https://github.com/libcpr/cpr.git
            GIT_TAG 3591f20e82338d0a21733045f59accd2d476d84f # CPR 1.9.8
        )

        set(BUILD_CPR_TESTS OFF CACHE BOOL "Disable CPR tests")
        set(CPR_BUILD_TESTS OFF CACHE BOOL "Disable CPR tests")
        set(BUILD_SHARED_LIBS OFF CACHE BOOL "Build static libraries")
        set(CPR_BUILD_TESTS OFF CACHE BOOL "Disable CPR tests")

        FetchContent_MakeAvailable(cpr)
        set(CPR_BUILT_LOCALLY TRUE)
    else()
        message(WARNING "CPR auto-download requires CMake 3.14+. Please install CPR manually.")
        set(CPR_BUILT_LOCALLY FALSE)
    endif()
else()
    message(STATUS "Found system CPR: ${cpr_VERSION}")
endif()

message(STATUS "Installing QuickJS CMake...")
if(CMAKE_VERSION VERSION_GREATER_EQUAL 3.14)
    FetchContent_Declare(quickjscmake
        GIT_REPOSITORY https://github.com/robloach/quickjs-cmake.git
        GIT_TAG 2021.3.27
    )
    FetchContent_MakeAvailable(quickjscmake)
else()
    find_library(QUICKJS_LIB quickjs)
    find_path(QUICKJS_INCLUDE quickjs.h)
    if(QUICKJS_LIB AND QUICKJS_INCLUDE)
        add_library(quickjs INTERFACE)
        target_include_directories(quickjs INTERFACE ${QUICKJS_INCLUDE})
        target_link_libraries(quickjs INTERFACE ${QUICKJS_LIB})
        message(STATUS "Found system QuickJS")
    else()
        message(WARNING "QuickJS not found. Please install QuickJS development packages")
    endif()
endif()

message(STATUS "Installing Luau...")
if(CMAKE_VERSION VERSION_GREATER_EQUAL 3.14)
    FetchContent_Declare(luau
        GIT_REPOSITORY https://github.com/Roblox/luau.git
        GIT_TAG 0.699
    )

    set(LUAU_BUILD_CLI OFF CACHE BOOL "Disable CLI")
    set(LUAU_BUILD_TESTS OFF CACHE BOOL "Disable tests")
    set(LUAU_BUILD_WEB OFF CACHE BOOL "Disable web build")

    FetchContent_MakeAvailable(luau)

    if(luau_POPULATED)
        message(STATUS "Luau successfully configured")

        add_library(luau_lib STATIC
            ${luau_SOURCE_DIR}/Ast/src/Ast.cpp
            ${luau_SOURCE_DIR}/Ast/src/Confusables.cpp
            ${luau_SOURCE_DIR}/Ast/src/Lexer.cpp
            ${luau_SOURCE_DIR}/Ast/src/Location.cpp
            ${luau_SOURCE_DIR}/Ast/src/Parser.cpp
            ${luau_SOURCE_DIR}/Common/src/StringUtils.cpp
            ${luau_SOURCE_DIR}/Ast/src/TimeTrace.cpp
            ${luau_SOURCE_DIR}/Compiler/src/Builtins.cpp
            ${luau_SOURCE_DIR}/Compiler/src/BuiltinFolding.cpp
            ${luau_SOURCE_DIR}/Compiler/src/BytecodeBuilder.cpp
            ${luau_SOURCE_DIR}/Compiler/src/Compiler.cpp
            ${luau_SOURCE_DIR}/Compiler/src/ConstantFolding.cpp
            ${luau_SOURCE_DIR}/Compiler/src/CostModel.cpp
            ${luau_SOURCE_DIR}/Compiler/src/lcode.cpp
            ${luau_SOURCE_DIR}/Compiler/src/TableShape.cpp
            ${luau_SOURCE_DIR}/Compiler/src/ValueTracking.cpp
            ${luau_SOURCE_DIR}/VM/src/lapi.cpp
            ${luau_SOURCE_DIR}/VM/src/laux.cpp
            ${luau_SOURCE_DIR}/VM/src/lbaselib.cpp
            ${luau_SOURCE_DIR}/VM/src/lbitlib.cpp
            ${luau_SOURCE_DIR}/VM/src/lbuiltins.cpp
            ${luau_SOURCE_DIR}/VM/src/lcorolib.cpp
            ${luau_SOURCE_DIR}/VM/src/ldblib.cpp
            ${luau_SOURCE_DIR}/VM/src/ldebug.cpp
            ${luau_SOURCE_DIR}/VM/src/ldo.cpp
            ${luau_SOURCE_DIR}/VM/src/lfunc.cpp
            ${luau_SOURCE_DIR}/VM/src/lgc.cpp
            ${luau_SOURCE_DIR}/VM/src/linit.cpp
            ${luau_SOURCE_DIR}/VM/src/lmathlib.cpp
            ${luau_SOURCE_DIR}/VM/src/lmem.cpp
            ${luau_SOURCE_DIR}/VM/src/lobject.cpp
            ${luau_SOURCE_DIR}/VM/src/loslib.cpp
            ${luau_SOURCE_DIR}/VM/src/lperf.cpp
            ${luau_SOURCE_DIR}/VM/src/lstate.cpp
            ${luau_SOURCE_DIR}/VM/src/lstring.cpp
            ${luau_SOURCE_DIR}/VM/src/lstrlib.cpp
            ${luau_SOURCE_DIR}/VM/src/ltable.cpp
            ${luau_SOURCE_DIR}/VM/src/ltablib.cpp
            ${luau_SOURCE_DIR}/VM/src/ltm.cpp
            ${luau_SOURCE_DIR}/VM/src/ludata.cpp
            ${luau_SOURCE_DIR}/VM/src/lutf8lib.cpp
            ${luau_SOURCE_DIR}/VM/src/lvmexecute.cpp
            ${luau_SOURCE_DIR}/VM/src/lvmload.cpp
            ${luau_SOURCE_DIR}/VM/src/lvmutils.cpp
            ${luau_SOURCE_DIR}/Ast/src/Allocator.cpp
            ${luau_SOURCE_DIR}/Ast/src/Cst.cpp
            ${luau_SOURCE_DIR}/Ast/src/PrettyPrinter.cpp
            ${luau_SOURCE_DIR}/Compiler/src/Types.cpp
            ${luau_SOURCE_DIR}/VM/src/lbuffer.cpp
            ${luau_SOURCE_DIR}/VM/src/lbuflib.cpp
            ${luau_SOURCE_DIR}/VM/src/lgcdebug.cpp
            ${luau_SOURCE_DIR}/VM/src/lnumprint.cpp
            ${luau_SOURCE_DIR}/VM/src/lveclib.cpp
        )

        target_include_directories(luau_lib PUBLIC
            ${luau_SOURCE_DIR}/Ast/include
            ${luau_SOURCE_DIR}/Compiler/include
            ${luau_SOURCE_DIR}/VM/include
            ${luau_SOURCE_DIR}/Common/include
        )

        target_compile_definitions(luau_lib PRIVATE
            LUA_USE_LINUX
            LUAU_FASTCALL
        )

        if(MSVC)
            target_compile_options(luau_lib PRIVATE /W0)
        else()
            target_compile_options(luau_lib PRIVATE -w)
        endif()

        message(STATUS "Luau built from source")
    endif()
else()
    message(WARNING "Please use CMake 3.14+")
endif()

include_directories(third-party)

add_library(justc_core STATIC
    ${CMAKE_CURRENT_SOURCE_DIR}/core/lexer.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/core/parser.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/core/keywords.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/core/from.json.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/core/to.json.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/core/fetch.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/core/to.xml.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/core/to.yaml.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/core/utility.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/core/run.js.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/core/import.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/core/run.luau.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/core/built-in/http/http.cpp
)

add_executable(justc ${CMAKE_CURRENT_SOURCE_DIR}/core/main.cpp)

target_include_directories(justc_core PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/core>
)

if(CPR_BUILT_LOCALLY)
    target_link_libraries(justc_core PRIVATE cpr)
else()
    target_link_libraries(justc_core PRIVATE cpr::cpr)
endif()

target_link_libraries(justc_core PRIVATE quickjs luau_lib)
target_link_libraries(justc PRIVATE justc_core quickjs luau_lib)

target_include_directories(justc_core PUBLIC
    ${luau_SOURCE_DIR}/Ast/include
    ${luau_SOURCE_DIR}/Compiler/include
    ${luau_SOURCE_DIR}/VM/include
    ${luau_SOURCE_DIR}/Common/include
)

install(TARGETS justc DESTINATION bin)

if(WIN32)
    add_custom_command(TARGET justc POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
                $<TARGET_FILE:justc> ${CMAKE_BINARY_DIR}
    )
endif()
