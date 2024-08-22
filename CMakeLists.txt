cmake_minimum_required(VERSION 3.28)

set(project_name MyLib)

project(MyLib)

set(CMAKE_CXX_STANDARD 26)

include ("$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake")

add_executable(${project_name} main.cpp
        src/modules/ResourceManager.ixx
        src/modules/com/yrm/libcpp/std.compat.ixx
        src/modules/com/yrm/libcpp/std.ixx
        src/modules/com/yrm/container/DLList.ixx)

#[[target_sources(${project_name}
        PUBLIC
        FILE_SET std_modules TYPE CXX_MODULES FILES
        libs/modules/std.compat.ixx
        libs/modules/std.ixx
)]]

file(GLOB_RECURSE MODULE_FILES CONFIGURE_DEPENDS ${CMAKE_SOURCE_DIR}/src/modules/*)

target_sources(${project_name}
        PUBLIC
        FILE_SET my_modules TYPE CXX_MODULES FILES ${MODULE_FILES}
)