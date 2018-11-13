cmake_minimum_required (VERSION 2.8.11)
project (hidconsoleapp)

IF(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")


find_package(IOKit REQUIRED)
find_package(CoreFoundation REQUIRED)

include_directories(
    "../include"
    ${IOKit_INCLUDE_DIRS}
    ${CoreFoundation_INCLUDE_DIR}
)

add_definitions( "-Wall -g -c")

add_executable(
    hidconsoleapp
    consoleapp.cpp
    usb_hid_raw.cpp
    hid-macosx.c
)


target_link_libraries( hidconsoleapp ${IOKit_LIBRARIES} ${CoreFoundation_LIBRARIES} )


elseif (${CMAKE_SYSTEM_NAME} MATCHES "Linux")

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")

find_package(libusb-1.0 REQUIRED)

include_directories(
     ${LIBUSB_1_INCLUDE_DIRS}
     "../include"
)

add_definitions( "-Wall -std=gnu++11")

add_executable(
    hidconsoleapp
    consoleapp.cpp
    usb_hid_raw.cpp
    hid-libusb.c
)

target_link_libraries(
    hidconsoleapp
    ${LIBUSB_1_LIBRARIES}
    pthread
)

elseif (${CMAKE_SYSTEM_NAME} MATCHES "Windows")

include_directories(
    "../include"
    "${PROJECT_SOURCE_DIR}/getopt"
)

add_executable(
    hidconsoleapp
    getopt/getopt.c
    consoleapp.cpp
    hid.cpp
    usb_hid_raw.cpp
)

target_link_libraries(
    hidconsoleapp
    setupapi
)

endif()
