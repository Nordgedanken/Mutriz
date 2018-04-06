 include(ExternalProject)

 #
 # Build matrix-structs.
 #

 set(THIRD_PARTY_ROOT ${CMAKE_SOURCE_DIR}/.third-party/mtxclient/.third-party)
 set(MATRIX_STRUCTS_ROOT ${THIRD_PARTY_ROOT}/matrix_structs)
 set(MATRIX_STRUCTS_INCLUDE_DIRS ${MATRIX_STRUCTS_ROOT}/include)
 set(MATRIX_STRUCTS_INCLUDE_DIRS ${MATRIX_STRUCTS_INCLUDE_DIRS} ${MATRIX_STRUCTS_ROOT}/include/mtx)
 set(MATRIX_STRUCTS_LIBRARY matrix_structs)

 link_directories(${MATRIX_STRUCTS_ROOT})

 set(WINDOWS_FLAGS "")

 if(MSVC)
     set(WINDOWS_FLAGS "-DCMAKE_GENERATOR_PLATFORM=x64")
 endif()

 ExternalProject_Add(
         MatrixStructs

         GIT_REPOSITORY https://github.com/mujx/matrix-structs
         GIT_TAG 93fcce2d83d728cc0db31b0b9eed196409de0df1

         BUILD_IN_SOURCE 1
         SOURCE_DIR ${MATRIX_STRUCTS_ROOT}
         CONFIGURE_COMMAND ${CMAKE_COMMAND}
         -DCMAKE_BUILD_TYPE=Release ${MATRIX_STRUCTS_ROOT}
         ${WINDOWS_FLAGS}
         BUILD_COMMAND ${CMAKE_COMMAND} --build ${MATRIX_STRUCTS_ROOT} --config Release
         INSTALL_COMMAND ""
 )
 link_directories(${MATRIX_STRUCTS_ROOT}/Release)