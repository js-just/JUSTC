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

set(CMAKE_CXX_STANDARD 11)
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
FetchContent_Declare(cpr GIT_REPOSITORY https://github.com/libcpr/cpr.git
                         GIT_TAG 3591f20e82338d0a21733045f59accd2d476d84f) # CPR 1.9.8
FetchContent_MakeAvailable(cpr)

find_package(cpr REQUIRED)

if(cpr_FOUND)
    message(STATUS "Found CPR: ${cpr_VERSION}")
endif()

# set(CMAKE_PCH_INSTANTIATE_TEMPLATES ON)

include_directories(third-party)

add_library(justc_core
    core/lexer.cpp
    core/parser.cpp
    core/keywords.cpp
    core/from.json.cpp
    core/to.json.cpp
    core/fetch.cpp
    core/to.xml.cpp
    core/to.yaml.cpp
    core/utility.cpp
)

add_executable(justc core/main.cpp)

target_include_directories(justc_core PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/core>
)

if(cpr_FOUND)
    target_link_libraries(justc_core PRIVATE cpr::cpr)
else()
    message(WARNING "CPR library not found")
endif()

target_link_libraries(justc PRIVATE justc_core)

install(TARGETS justc DESTINATION bin)

if(WIN32)
    add_custom_command(TARGET justc POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
                $<TARGET_FILE:justc> ${CMAKE_BINARY_DIR}
    )
endif()
