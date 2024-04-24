cmake_minimum_required(VERSION 3.12)
project(qtdaoforeignkeytest)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_AUTOMOC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Core Sql Test REQUIRED)

set(TEST_TYPE "sqlite" CACHE STRING "test type string")
message("foreign key test -> test type: ${TEST_TYPE}")

include(${CMAKE_CURRENT_SOURCE_DIR}/${TEST_TYPE}/entity.cmake)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/${TEST_TYPE})

add_compile_definitions(TEST_DB="${TEST_TYPE}")

add_executable(${PROJECT_NAME}
  main.cpp
  ${ENTITY_FILE_LIST}
  foreignkeytest.h
  foreignkeytest.cpp
  )

target_link_libraries(${PROJECT_NAME}
    Qt${QT_VERSION_MAJOR}::Core
    Qt${QT_VERSION_MAJOR}::Sql
    Qt${QT_VERSION_MAJOR}::Test
    qtdao)