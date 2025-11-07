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

find_package(CURL REQUIRED)

if(CURL_FOUND)
    message(STATUS "Found CURL: ${CURL_VERSION}")
endif()

if(WIN32)
    set(WINHTTP_LIBRARIES "winhttp.lib")
endif()

# set(CMAKE_PCH_INSTANTIATE_TEMPLATES ON)

include_directories(third-party/nlohmann)

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

if(CURL_FOUND)
    target_link_libraries(justc_core PRIVATE CURL::libcurl)
elseif(WIN32)
    target_link_libraries(justc_core PRIVATE ${WINHTTP_LIBRARIES})
else()
    message(WARNING "CURL library not found - HTTP functionality will be limited")
endif()

target_link_libraries(justc PRIVATE justc_core)

install(TARGETS justc DESTINATION bin)

if(WIN32)
    add_custom_command(TARGET justc POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
                $<TARGET_FILE:justc> ${CMAKE_BINARY_DIR}
    )
endif()
