include(ExternalProject)

#
# Download & install Boost from source.
#

set(THIRD_PARTY_ROOT ${CMAKE_SOURCE_DIR}/.third-party/mtxclient/.third-party)
set(BUNDLED_BOOST_ROOT ${THIRD_PARTY_ROOT}/boost_1_66_0)

ExternalProject_Add(
  Boost

  URL https://dl.bintray.com/boostorg/release/1.66.0/source/boost_1_66_0.zip
  URL_HASH SHA256=e1c55ebb00886c1a96528e4024be98a38b815115f62ecfe878fcf587ba715aad
  DOWNLOAD_DIR ${THIRD_PARTY_ROOT}/downloads
  DOWNLOAD_NO_PROGRESS 0

  BUILD_IN_SOURCE 1
  SOURCE_DIR ${BUNDLED_BOOST_ROOT}
  CONFIGURE_COMMAND ${BUNDLED_BOOST_ROOT}/bootstrap.sh
    --with-libraries=random,thread,system,iostreams
    --prefix=${BUNDLED_BOOST_ROOT}/build
  BUILD_COMMAND ${BUNDLED_BOOST_ROOT}/b2 -d0 --layout=system --abbreviate-paths toolset=msvc variant=release link=static threading=multi
  INSTALL_COMMAND ${BUNDLED_BOOST_ROOT}/b2 -d0 install
)

set(Boost_INCLUDE_DIRS ${BUNDLED_BOOST_ROOT}/build)
if(MSVC)
    set(Boost_LIBRARIES libboost_random libboost_system libboost_thread libboost_iostreams)
else()
    set(Boost_LIBRARIES boost_random boost_system boost_thread boost_iostreams)
endif()
include_directories(SYSTEM ${Boost_INCLUDE_DIRS})
link_directories(${Boost_INCLUDE_DIRS}/stage/lib)
