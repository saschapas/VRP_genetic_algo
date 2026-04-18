# ------------------------------------------------
# Header
# ------------------------------------------------
cmake_minimum_required(VERSION 3.8)
project(ko++)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# ------------------------------------------------
# Options
# ------------------------------------------------
option(KO_ENABLE_LOGGER "enables the logger system" ON)

# ------------------------------------------------
# Dependencies
# ------------------------------------------------

# ---- FMTLIB ------------------------------------
add_subdirectory(deps/fmt-10.1.0)
get_target_property(fmt_INCLUDE_DIRECTORIES fmt::fmt INTERFACE_INCLUDE_DIRECTORIES)
include_directories(SYSTEM ${fmt_INCLUDE_DIRECTORIES})

# ------------------------------------------------
# Setup
# ------------------------------------------------
set(KO_COMPILE_OPTIONS -Wall -Wpedantic -Wextra)
set(KO_COMPILE_DEFINITIONS "")
set(KO_COMMON_LIBRARIES fmt::fmt)
set(CMAKE_BUILD_TYPE Debug)

if (NOT ${KO_ENABLE_LOGGING})
    message(STATUS "[KO] disabled logger")
    list(APPEND KO_COMPILE_DEFINITIONS "KO_DISABLE_LOGGER=1")
endif()

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release")
endif()

set(CMAKE_BUILD_TYPE "${CMAKE_BUILD_TYPE}" CACHE STRING
        "Choose the type of build, options are: None(CMAKE_CXX_FLAGS or CMAKE_C_FLAGS used) Debug Release RelWithDebInfo MinSizeRel Maintainer."
        FORCE)

function(ko_prepare_target target)
    message(STATUS "[KO] preparing target: ${target}")
    target_compile_options(${target} PUBLIC ${KO_COMPILE_OPTIONS})
    target_compile_definitions(${target} PUBLIC ${KO_COMPILE_DEFINITIONS})
    target_link_libraries(${target}  ${KO_COMMON_LIBRARIES})
endfunction()


# ------------------------------------------------
# Libraries
# ------------------------------------------------

# ---- KO-Plus -----------------------------------
add_library(ko++
        src/util/Logger.cpp
        src/vrp/Instance.cpp
        #src/vrp/population.cpp
        )
ko_prepare_target(ko++)
target_include_directories(ko++ PUBLIC src)

# ------------------------------------------------
# Executables
# ------------------------------------------------

# ---- DemoVRP -----------------------------------
# alle weiteren Source-Dateien (.cpp), die demoVRP benötigt, hier hinzufügen
add_executable(demoVRP 
    src/vrp/DemoVRP.cpp)
add_executable(testerVRP
    src/vrp/TesterVRP.cpp
    # Falls TesterVRP.cpp Code aus weiteren .cpp benötigt, hier auch hinzufügen
    # z.B. src/vrp/SomeClass.cpp
)
target_link_libraries(demoVRP ko++)
target_link_libraries(testerVRP PRIVATE ko++)