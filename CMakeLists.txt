cmake_minimum_required(VERSION 3.10)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
project(Mutriz LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 14)

set(EXTERNAL_PROJECT_DEPS "")
# Dependencies on ExternalProjects
set(MUTRIZ_DEPS "")

# Libraries linked to matrix_client
set(MUTRIZ_LIBS "")


file(DOWNLOAD
        "https://github.com/nlohmann/json/releases/download/v3.1.2/json.hpp"
        ${PROJECT_SOURCE_DIR}/json.hpp
        EXPECTED_HASH SHA256=fbdfec4b4cf63b3b565d09f87e6c3c183bdd45c5be1864d3fcb338f6f02c1733)

file(DOWNLOAD
        "https://github.com/mpark/variant/releases/download/v1.3.0/variant.hpp"
        ${PROJECT_SOURCE_DIR}/variant.hpp
        EXPECTED_MD5 "be0ce322cdd408e1b347b9f1d59ea67a")

#
# OpenSSL
#
find_package(OpenSSL REQUIRED)
include_directories(${OPENSSL_INCLUDE_DIR})
set(MUTRIZ_LIBS ${MUTRIZ_LIBS} ${OPENSSL_LIBRARIES})

#
# mtxclient
#
find_library(MTXCLIENT_LIBRARY
        NAMES matrix_client
        PATHS ${MTXCLIENT_ROOT}
        ${MTXCLIENT_ROOT}/lib
        ${MTXCLIENT_ROOT}/lib/static)

if(NOT MTXCLIENT_LIBRARY)
    include(Mtxclient)
    set(EXTERNAL_PROJECT_DEPS ${EXTERNAL_PROJECT_DEPS} Mtxclient)
endif()

include_directories(SYSTEM ${MTXCLIENT_INCLUDE_DIR})
set(MUTRIZ_LIBS ${MUTRIZ_LIBS} ${MTXCLIENT_LIBRARY})

#
# Boost 1.66
#
# If we can't find an already installed version we will
# download it and build it from source.
#
set(Boost_USE_STATIC_LIBS OFF)
set(Boost_USE_MULTITHREADED ON)
find_package(Boost 1.66)
find_package(Boost 1.66 COMPONENTS system random thread iostreams)

if(NOT Boost_FOUND)
    include(Boost)
    set(MUTRIZ_DEPS ${MUTRIZ_DEPS} Boost)
endif()

include_directories(${Boost_INCLUDE_DIRS})
set(MUTRIZ_LIBS ${MUTRIZ_LIBS} ${Boost_LIBRARIES})

#
# matrix-structs
#
find_library(MATRIX_STRUCTS_LIBRARY
        NAMES matrix_structs
        PATHS ${MATRIX_STRUCTS_ROOT}
        ${MATRIX_STRUCTS_ROOT}/lib
        ${MATRIX_STRUCTS_ROOT}/lib/static)

if(NOT MATRIX_STRUCTS_LIBRARY)
    include(MatrixStructs)
endif()
set(MUTRIZ_DEPS ${MUTRIZ_DEPS} MatrixStructs)
include_directories(${MATRIX_STRUCTS_INCLUDE_DIRS})
set(MUTRIZ_LIBS ${MUTRIZ_LIBS} matrix_structs)

#
# libolm
#
include(Olm)
set(MTXCLIENT_DEPS ${MTXCLIENT_DEPS} Olm)
set(MTXCLIENT_LIBS ${MTXCLIENT_LIBS} olm)


add_executable(Mutriz main.cpp)

if(EXTERNAL_PROJECT_DEPS)
    add_dependencies(Mutriz ${EXTERNAL_PROJECT_DEPS})
endif()

add_dependencies(Mutriz ${MUTRIZ_DEPS})
target_link_libraries(Mutriz ${MUTRIZ_LIBS})
target_include_directories(Mutriz SYSTEM PRIVATE $ENV{INCLUDE})