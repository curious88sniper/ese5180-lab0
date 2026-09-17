cmake_minimum_required(VERSION 3.20.0)

find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(sum_unit_test)

if(EXISTS "${CMAKE_CURRENT_LIST_DIR}/../../lab0_app")
  set(LAB0_APP_DIR "${CMAKE_CURRENT_LIST_DIR}/../../lab0_app")
else()
  set(LAB0_APP_DIR "${CMAKE_CURRENT_LIST_DIR}/../../lab0-app")
endif()

target_include_directories(app PRIVATE ${LAB0_APP_DIR}/include)

target_sources(app PRIVATE
  src/main.c
  ${LAB0_APP_DIR}/src/sum_printk/sum_printk.c
)
