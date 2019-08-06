cmake_minimum_required(VERSION 3.1 FATAL_ERROR)
include(GenerateExportHeader)
include(GNUInstallDirs)

project(RaspiFanshim VERSION 1.0.0.0 LANGUAGES C CXX)

# =============================================================================================== #
# Overridable options                                                                             #
# =============================================================================================== #

# Build configuration
option(RASPI_FANSHIM_BUILD_SHARED_LIB
    "Build shared library"
    OFF)
option(RASPI_FANSHIM_BUILD_EXAMPLES
    "Build examples"
    OFF)

# =============================================================================================== #
# Dependencies                                                                                    #
# =============================================================================================== #

if (NOT EXISTS "${CMAKE_CURRENT_LIST_DIR}/dependencies/raspi-apa102/CMakeLists.txt")
    message(
        FATAL_ERROR
        "Can't find raspi-apa102 submodule. Please make sure to clone the repo recursively.\n"
        "You can fix this by running\n"
        "    git submodule update --init\n"
        "or by cloning using\n"
        "    git clone --recursive <url>"
    )
endif ()

add_subdirectory("${CMAKE_CURRENT_LIST_DIR}/dependencies/raspi-apa102" EXCLUDE_FROM_ALL)

# =============================================================================================== #
# Library configuration                                                                           #
# =============================================================================================== #

if (RASPI_FANSHIM_BUILD_SHARED_LIB)
    add_library("RaspiFanshim" SHARED)
else ()
    add_library("RaspiFanshim" STATIC)
endif ()

set_target_properties("RaspiFanshim" PROPERTIES LINKER_LANGUAGE C)
target_link_libraries("RaspiFanshim" "RaspiAPA102")
target_include_directories("RaspiFanshim"
    PUBLIC "include" ${PROJECT_BINARY_DIR}
    PRIVATE "src")
generate_export_header("RaspiFanshim" BASE_NAME "RASPI_FANSHIM" EXPORT_FILE_NAME 
    "RaspiFanshimExportConfig.h")

target_sources("RaspiFanshim"
    PRIVATE
        "${CMAKE_CURRENT_LIST_DIR}/include/RaspiFanshim/RaspiFanshim.h"
        "src/RaspiFanshim.c")

target_compile_definitions("RaspiFanshim" PRIVATE "_GNU_SOURCE")
find_library(wiringPi_LIB wiringPi REQUIRED)
target_link_libraries("RaspiFanshim" ${wiringPi_LIB})

# TODO: Install CMake config.
install(TARGETS "RaspiFanshim"
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
install(FILES
    "${PROJECT_BINARY_DIR}/RaspiFanshimExportConfig.h"
    DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}")
install(DIRECTORY "include/" DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

# =============================================================================================== #
# Examples                                                                                        #
# =============================================================================================== #

if (RASPI_FANSHIM_BUILD_EXAMPLES)
    add_executable("FanshimService" "examples/FanshimService.c")
    target_link_libraries("FanshimService" "RaspiFanshim")
endif ()

# =============================================================================================== #
