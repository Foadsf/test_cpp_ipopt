cmake_minimum_required(VERSION 3.20)
project(test_ipopt LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Use conda-forge IPOPT (includes MUMPS)
# Set CONDA_PREFIX to the active conda environment
if(DEFINED ENV{CONDA_PREFIX})
    set(CONDA_PREFIX "$ENV{CONDA_PREFIX}")
else()
    message(FATAL_ERROR 
        "CONDA_PREFIX not set. Please activate the conda environment:\n"
        "  conda activate ipopt-dev"
    )
endif()

set(IPOPT_ROOT "${CONDA_PREFIX}/Library")

find_path(IPOPT_INCLUDE_DIR
    NAMES IpIpoptApplication.hpp
    PATHS "${IPOPT_ROOT}/include/coin-or"
    NO_DEFAULT_PATH
)

find_library(IPOPT_LIBRARY
    NAMES ipopt ipopt-3
    PATHS "${IPOPT_ROOT}/lib"
    NO_DEFAULT_PATH
)

if(NOT IPOPT_INCLUDE_DIR OR NOT IPOPT_LIBRARY)
    message(FATAL_ERROR 
        "IPOPT not found in conda environment.\n"
        "  CONDA_PREFIX: ${CONDA_PREFIX}\n"
        "  Run: conda install -c conda-forge ipopt"
    )
endif()

message(STATUS "Using conda IPOPT from: ${CONDA_PREFIX}")
message(STATUS "IPOPT include: ${IPOPT_INCLUDE_DIR}")
message(STATUS "IPOPT library: ${IPOPT_LIBRARY}")

add_executable(test_ipopt main.cpp)
target_include_directories(test_ipopt PRIVATE ${IPOPT_INCLUDE_DIR})
target_link_libraries(test_ipopt PRIVATE ${IPOPT_LIBRARY})

# Copy DLLs to output directory
file(GLOB IPOPT_DLLS "${IPOPT_ROOT}/bin/*ipopt*.dll" "${IPOPT_ROOT}/bin/*mumps*.dll")
add_custom_command(TARGET test_ipopt POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${IPOPT_DLLS}
        $<TARGET_FILE_DIR:test_ipopt>
    COMMENT "Copying IPOPT DLLs from conda"
)