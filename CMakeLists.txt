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
project(JUSTC LANGUAGES CXX)

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

if(CMAKE_VERSION VERSION_GREATER_EQUAL 3.14)
    include(FetchContent)
endif()

message(STATUS "Setting up Luau...")
if(CMAKE_VERSION VERSION_GREATER_EQUAL 3.14)
    FetchContent_Declare(luau
        GIT_REPOSITORY https://github.com/luau-lang/luau
        GIT_TAG 994b6416f1a2d16ac06c52b4e574bad5d8749053 # Luau 0.699
    )
    FetchContent_MakeAvailable(luau)
else()
    message(STATUS "Using manual Luau setup (CMake ${CMAKE_VERSION})")

    if(NOT TARGET Luau::VM)
        # Add Luau source files directly
        file(GLOB_RECURSE LUAU_SOURCES
            "luau/Ast/src/*.cpp"
            "luau/Compiler/src/*.cpp"
            "luau/VM/src/*.cpp"
        )

        if(LUAU_SOURCES)
            add_library(luau_static STATIC ${LUAU_SOURCES})
            target_include_directories(luau_static PUBLIC
                "luau/Ast/include"
                "luau/Compiler/include"
                "luau/VM/include"
            )
            target_compile_features(luau_static PUBLIC cxx_std_17)
            add_library(Luau::VM ALIAS luau_static)
            add_library(Luau::Compiler ALIAS luau_static)
            add_library(Luau::Ast ALIAS luau_static)
            message(STATUS "Using pre-downloaded Luau sources")
        else()
            message(WARNING "Luau sources not found. Please clone Luau manually:")
            message(WARNING "git clone https://github.com/luau-lang/luau.git")
            message(WARNING "git checkout 994b6416f1a2d16ac06c52b4e574bad5d8749053")
        endif()
    endif()
endif()

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

include_directories(third-party)

add_library(justc_core STATIC
    core/lexer.cpp
    core/parser.cpp
    core/keywords.cpp
    core/from.json.cpp
    core/to.json.cpp
    core/fetch.cpp
    core/to.xml.cpp
    core/to.yaml.cpp
    core/utility.cpp
    core/run.js.cpp
    core/import.cpp
    core/run.lua.cpp
)

add_executable(justc core/main.cpp)

target_include_directories(justc_core PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/core>
)

if(CPR_BUILT_LOCALLY)
    target_link_libraries(justc_core PRIVATE cpr)
else()
    target_link_libraries(justc_core PRIVATE cpr::cpr)
endif()

target_link_libraries(justc_core PRIVATE quickjs)

if(TARGET Luau::VM)
    target_link_libraries(justc_core PRIVATE Luau::Ast Luau::Compiler Luau::VM)
elseif(TARGET luau_static)
    target_link_libraries(justc_core PRIVATE luau_static)
else()
    message(WARNING "Luau not available - Lua scripting will be disabled")
    target_compile_definitions(justc_core PRIVATE -DNO_LUA_SUPPORT)
endif()

target_link_libraries(justc PRIVATE justc_core quickjs)

if(TARGET Luau::VM)
    target_link_libraries(justc PRIVATE Luau::Ast Luau::Compiler Luau::VM)
elseif(TARGET luau_static)
    target_link_libraries(justc PRIVATE luau_static)
endif()

install(TARGETS justc DESTINATION bin)

if(WIN32)
    add_custom_command(TARGET justc POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
                $<TARGET_FILE:justc> ${CMAKE_BINARY_DIR}
    )
endif()
