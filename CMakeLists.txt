#
# (c) 2019 Copyright, Real-Time Innovations, Inc. All rights reserved.
#
#  RTI grants Licensee a license to use, modify, compile, and create derivative
#  works of the Software.  Licensee has the right to distribute object form
#  only for use with RTI products.  The Software is provided "as is", with no
#  warranty of any type, including any warranty for fitness for any purpose.
#  RTI is under no obligation to maintain or support the Software.  RTI shall
#  not be liable for any incidental or consequential damages arising out of the
#  use or inability to use the software.


cmake_minimum_required(VERSION 3.7.2)
project("rtixmloutpututility")

if(NOT CONNEXTDDS_DIR)
    set(msg "Please provide the top level folder location for your Connext DDS installation. "
            "You need v6.0.0 and above for using this utility. Use the following option "
            "-DCONNEXTDDS_DIR=<your installation path>.")
    message(FATAL_ERROR ${msg})
endif()

# Find RTI Connext dependencies
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CONNEXTDDS_DIR}/resource/cmake")
find_package(
    RTIConnextDDS "6.0.0"
    REQUIRED
    COMPONENTS
        core
)

if(CMAKE_SYSTEM_NAME MATCHES "Linux" AND
    CMAKE_C_COMPILER_VERSION VERSION_GREATER "4.6.0")
    set(CONNEXTDDS_EXTERNAL_LIBS
        -Wl,--no-as-needed  ${CONNEXTDDS_EXTERNAL_LIBS})
endif()

set(RTIXMLOUTPUTUTILITY_C_SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/src/RTIXMLOutputUtility.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/CommandLineArgumentParser.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/XMLHelper.c")

if(BUILD_SHARED_LIBS)
    set(rtixml2_library_name "rtixml2")
else()
    set(rtixml2_library_name "rtixml2z")
endif()

if(CMAKE_BUILD_TYPE)
    string(TOUPPER "${CMAKE_BUILD_TYPE}" build_mode)
else()
    set(build_mode "DEBUG")
endif()

if(build_mode STREQUAL "DEBUG")
    set(rtixml2_library_name "${rtixml2_library_name}d")
endif()

message("-- rtixml2_library_name: ${rtixml2_library_name}")

find_library(rtixml2_library
    NAMES ${rtixml2_library_name}
    PATHS "${CONNEXTDDS_DIR}/lib/${CONNEXTDDS_ARCH}/"
)

# Create the executable for rtixmloutpututility
add_executable(rtixmloutpututility
    ${RTIXMLOUTPUTUTILITY_C_SOURCES}
)

target_link_libraries(rtixmloutpututility
    RTIConnextDDS::c_api
    ${rtixml2_library}
    ${CONNEXTDDS_EXTERNAL_LIBS}
)

target_include_directories(rtixmloutpututility
    PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}/src")