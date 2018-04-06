include(ExternalProject)

#
# Build mtxclient.
#

set(THIRD_PARTY_ROOT ${CMAKE_SOURCE_DIR}/.third-party)
set(MTXCLIENT_ROOT ${THIRD_PARTY_ROOT}/mtxclient)
set(MTXCLIENT_INCLUDE_DIR ${MTXCLIENT_ROOT}/src)
set(MTXCLIENT_LIBRARY matrix_client)

link_directories(${MTXCLIENT_ROOT})

set(WINDOWS_FLAGS "")

if(MSVC)
    set(WINDOWS_FLAGS "-DCMAKE_GENERATOR_PLATFORM=x64")
endif()

ExternalProject_Add(
        Mtxclient

        GIT_REPOSITORY https://github.com/mujx/mtxclient
        GIT_TAG 6ecda9ed78244b1d2aa71eb043fd51cb49a9c75d

        BUILD_IN_SOURCE 1
        SOURCE_DIR ${MTXCLIENT_ROOT}
        CONFIGURE_COMMAND ${CMAKE_COMMAND}
        -H.
        -DBUILD_LIB_TESTS=OFF
        -DBUILD_LIB_EXAMPLES=OFF
        -DCMAKE_BUILD_TYPE=Release ${MTXCLIENT_ROOT}
        ${WINDOWS_FLAGS}
        BUILD_COMMAND ${CMAKE_COMMAND} --build ${MTXCLIENT_ROOT} --config Release
        INSTALL_COMMAND ""
)

link_directories(${MTXCLIENT_ROOT}/Release)