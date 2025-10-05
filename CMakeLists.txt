cmake_minimum_required(VERSION 3.10)
project(JUSTC)

set(CMAKE_CXX_STANDARD 11)

find_package(PkgConfig REQUIRED)

if(UNIX AND NOT APPLE)
    pkg_check_modules(CURL REQUIRED libcurl)
elseif(APPLE)
    find_library(CURL_LIBRARIES curl)
    find_path(CURL_INCLUDE_DIRS curl/curl.h)
elseif(WIN32)
    # Для Windows используем WinHTTP (встроено в Windows)
    set(WINHTTP_LIBRARIES "winhttp.lib")
endif()

add_library(justc_core
    lexer.cpp
    parser.cpp
    keywords.cpp
    json_serializer.cpp
    interpreter.cpp
)

add_executable(justc main.cpp)

if(UNIX AND NOT APPLE)
    target_include_directories(justc PRIVATE ${CURL_INCLUDE_DIRS})
    target_link_libraries(justc justc_core ${CURL_LIBRARIES})
elseif(APPLE)
    target_include_directories(justc PRIVATE ${CURL_INCLUDE_DIRS})
    target_link_libraries(justc justc_core ${CURL_LIBRARIES})
elseif(WIN32)
    target_link_libraries(justc justc_core ${WINHTTP_LIBRARIES})
endif()
