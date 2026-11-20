cmake_minimum_required(VERSION 3.20)
project(escan LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(FetchContent)

find_package(yaml-cpp QUIET)

if(NOT TARGET yaml-cpp::yaml-cpp AND NOT TARGET yaml-cpp)
    set(YAML_CPP_BUILD_TESTS OFF CACHE BOOL "" FORCE)
    set(YAML_CPP_BUILD_TOOLS OFF CACHE BOOL "" FORCE)
    set(YAML_CPP_BUILD_CONTRIB OFF CACHE BOOL "" FORCE)
    set(YAML_CPP_INSTALL OFF CACHE BOOL "" FORCE)

    FetchContent_Declare(
        yaml-cpp
        URL https://github.com/jbeder/yaml-cpp/archive/refs/tags/yaml-cpp-0.9.0.tar.gz
        DOWNLOAD_EXTRACT_TIMESTAMP TRUE
    )

    FetchContent_MakeAvailable(yaml-cpp)
endif()

if(TARGET yaml-cpp::yaml-cpp)
    set(ESCAN_YAML_TARGET yaml-cpp::yaml-cpp)
else()
    set(ESCAN_YAML_TARGET yaml-cpp)
endif()

set(FTXUI_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(FTXUI_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(FTXUI_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(FTXUI_ENABLE_INSTALL OFF CACHE BOOL "" FORCE)

FetchContent_Declare(
    ftxui
    URL https://github.com/ArthurSonzogni/FTXUI/archive/refs/tags/v6.1.9.tar.gz
    DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)

FetchContent_MakeAvailable(ftxui)

add_executable(escan
    src/main.cpp
    src/cli.cpp
    src/ElfParse.cpp
    src/NameTables.cpp
    src/ParseSegments.cpp
    src/UI.cpp
)

target_include_directories(escan PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(escan PRIVATE ftxui::screen ftxui::dom ${ESCAN_YAML_TARGET})

if(WIN32)
    target_compile_definitions(escan PRIVATE NOMINMAX WIN32_LEAN_AND_MEAN)
    target_link_libraries(escan PRIVATE dbghelp)
endif()
