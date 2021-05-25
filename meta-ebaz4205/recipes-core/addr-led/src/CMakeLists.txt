cmake_minimum_required(VERSION 3.9)

project(addr-led VERSION 1.0.1 DESCRIPTION "Addressable led driver")

find_package(eclipse-paho-mqtt-c REQUIRED)

add_library(${CMAKE_PROJECT_NAME} SHARED
    led.c
)

set_target_properties(${CMAKE_PROJECT_NAME} PROPERTIES VERSION ${PROJECT_VERSION})
set_target_properties(${CMAKE_PROJECT_NAME} PROPERTIES PUBLIC_HEADER led.h)

target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE .)

add_executable(led-strip main.c)

target_link_libraries(led-strip PRIVATE ${CMAKE_PROJECT_NAME})
target_link_libraries(led-strip PRIVATE paho-mqtt3c)

install(TARGETS ${CMAKE_PROJECT_NAME} led-strip
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
