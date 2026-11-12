cmake_minimum_required(VERSION 3.20.0)

set(MODULE module1 CACHE STRING "Course module to build (app/<MODULE>/src/main.c)")

# Optional module-specific Kconfig fragment, merged on top of app/prj.conf
if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${MODULE}/prj.conf)
    set(EXTRA_CONF_FILE ${CMAKE_CURRENT_SOURCE_DIR}/${MODULE}/prj.conf)
endif()

find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(${MODULE})

if(NOT EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${MODULE}/src/main.c)
    message(FATAL_ERROR "Unknown module '${MODULE}': app/${MODULE}/src/main.c not found")
endif()

file(GLOB MODULE_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/${MODULE}/src/*.c)
target_sources(app PRIVATE ${MODULE_SOURCES})
