cmake_minimum_required(VERSION 3.10)

project(spi_slave)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

option(USE_WIRINGOP "Use WiringOp instead of WiringPi" OFF)

if(USE_WIRINGOP)
    find_package(WiringOp REQUIRED)
    set(LIBS ${WiringOp_LIBRARIES})
else()
    find_package(WiringPi REQUIRED)
    set(LIBS ${WiringPi_LIBRARIES})
endif()

add_library(${PROJECT_NAME} INTERFACE)
target_include_directories(${PROJECT_NAME} INTERFACE ${WiringPi_INCLUDE_DIR} ${WiringOp_INCLUDE_DIR})
target_include_directories(${PROJECT_NAME} INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(${PROJECT_NAME} INTERFACE ${WiringPi_LIBRARIES} ${WiringOp_LIBRARIES})

add_executable(echo_client examples/echo_client.cpp)
target_link_libraries(echo_client ${PROJECT_NAME})

add_executable(attiny85 examples/attiny85.cpp)
target_link_libraries(attiny85 ${PROJECT_NAME})
