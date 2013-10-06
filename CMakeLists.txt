cmake_minimum_required(VERSION 2.8.10)

if (DEFINED ENV{QTDIR})
    set(CMAKE_PREFIX_PATH $ENV{QTDIR} ${CMAKE_PREFIX_PATH})
else ()
    message (FATAL_ERROR "ERROR: Environment variable QTDIR is not set. Please locate your Qt folder MY_QT5_DIR.")
endif ()

project(qtuiotouch)

set(CMAKE_CXX_FLAGS                      "-Wall")
set(CMAKE_CXX_FLAGS_DEBUG          "-O0 -g")
set(CMAKE_CXX_FLAGS_MINSIZEREL  "-Os -DNDEBUG")

# Add visibility flags
include(CheckCXXCompilerFlag)
check_cxx_compiler_flag(-fvisibility=hidden HAS_VISIBILITY)
if(HAS_VISIBILITY)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fvisibility=hidden")
endif()

check_cxx_compiler_flag(-fvisibility-inlines-hidden HAS_VISIBILITY_INLINES_HIDDEN)
if(HAS_VISIBILITY_INLINES_HIDDEN)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fvisibility-inlines-hidden")
endif()

set(QTUIOTOUCH_INCLUDE_DIR "${PROJECT_SOURCE_DIR}/include")
include_directories("${PROJECT_BINARY_DIR}" "${QTUIOTOUCH_INCLUDE_DIR}")

# Qt 5
# Tell CMake to run moc when necessary:
set(CMAKE_AUTOMOC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
find_package(Qt5Gui REQUIRED)

find_package(Boost 1.51.0 COMPONENTS system)
include_directories(${Boost_INCLUDE_DIR})
include_directories(${Qt5Gui_PRIVATE_INCLUDE_DIRS})

set(QTUIOTOUCH_SOURCES
        src/dosc/decoder.cpp
        include/dosc/decoder.hpp
        include/dosc/bytes.hpp
        include/dosc/asio/udp_receiver.hpp
	src/tuio/tuio.cpp
	include/tuio/tuio.hpp
	src/qasioioservice.hpp
	src/qasioioservice.cpp
	src/qtuioplugin.hpp
	src/qtuioplugin.cpp
	src/qtuiotouch.hpp
	src/qtuiotouch.cpp
)

if(APPLE)
	add_library(qtuiotouch SHARED ${QTUIOTOUCH_SOURCES})
else ()
	add_library(qtuiotouch MODULE ${QTUIOTOUCH_SOURCES})
endif ()

qt5_use_modules(qtuiotouch Core)
target_link_libraries(qtuiotouch ${Boost_LIBRARIES} Qt5::Gui)
