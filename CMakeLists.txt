cmake_minimum_required(VERSION 3.10)
project(JUSTC)

set(CMAKE_CXX_STANDARD 11)

if(UNIX AND NOT APPLE)
    set(CMAKE_INSTALL_PREFIX "/usr/local" CACHE PATH "Installation prefix")
endif()

find_package(CURL)

if(CURL_FOUND)
    message(STATUS "Found CURL library")
else()
    find_package(PkgConfig)
    if(PKG_CONFIG_FOUND)
        if(UNIX AND NOT APPLE)
            pkg_check_modules(CURL libcurl)
        elseif(APPLE)
            find_library(CURL_LIBRARIES curl)
            find_path(CURL_INCLUDE_DIRS curl/curl.h)
        endif()
    endif()
endif()

if(WIN32)
    set(WINHTTP_LIBRARIES "winhttp.lib")
endif()

add_library(justc_core
    core/lexer.cpp
    core/parser.cpp
    core/keywords.cpp
    core/json_serializer.cpp
    core/fetch.cpp
    core/version.cpp
)

add_executable(justc core/main.cpp)

target_include_directories(justc_core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/core)

if(CURL_FOUND)
    target_include_directories(justc_core PRIVATE ${CURL_INCLUDE_DIRS})
    target_link_libraries(justc_core ${CURL_LIBRARIES})
elseif(UNIX AND NOT APPLE AND CURL_INCLUDE_DIRS)
    target_include_directories(justc_core PRIVATE ${CURL_INCLUDE_DIRS})
    target_link_libraries(justc_core ${CURL_LIBRARIES})
elseif(APPLE AND CURL_LIBRARIES)
    target_include_directories(justc_core PRIVATE ${CURL_INCLUDE_DIRS})
    target_link_libraries(justc_core ${CURL_LIBRARIES})
elseif(WIN32)
    target_link_libraries(justc_core ${WINHTTP_LIBRARIES})
else()
    message(WARNING "CURL library not found - HTTP functionality will be limited")
endif()

target_link_libraries(justc justc_core)

install(TARGETS justc DESTINATION bin)
if(WIN32)
    add_custom_command(TARGET justc POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:justc> ${CMAKE_BINARY_DIR}
    )
endif()