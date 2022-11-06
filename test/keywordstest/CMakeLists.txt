cmake_minimum_required(VERSION 3.2)
project(qtdaokeywordtest)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_AUTOMOC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

find_package(Qt5 COMPONENTS Core Sql Test REQUIRED)

set(TEST_TYPE "sqlite" CACHE STRING "test type string")
message("test type: ${TEST_TYPE}")

string(TOUPPER ${TEST_TYPE} _TEST_TYPE)

include(${CMAKE_CURRENT_SOURCE_DIR}/${TEST_TYPE}/entity.cmake)
add_definitions(-DTEST_${_TEST_TYPE})

add_executable(${PROJECT_NAME}
  main.cpp
  ${ENTITY_FILE_LIST}
  keywordstest.h
  keywordstest.cpp
  )

target_link_libraries(${PROJECT_NAME} Qt5::Core Qt5::Sql Qt5::Test qtdao::lib)