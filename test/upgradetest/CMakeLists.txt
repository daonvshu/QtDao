cmake_minimum_required(VERSION 3.12)
project(qtdaoupgradetest)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_AUTOMOC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Core Sql Test REQUIRED)

set(TEST_TYPE "sqlite" CACHE STRING "test type string")
set(TEST_VER "v1" CACHE STRING "test version")
message("upgrade test -> test type: ${TEST_TYPE}, version: ${TEST_VER}")

add_compile_definitions(TEST_DB="${TEST_TYPE}")

if (${TEST_VER} STREQUAL "v0")
  add_definitions(-DTEST_DB_CLEAR)
elseif (${TEST_VER} STREQUAL "v1")
  add_compile_definitions(TARGET_VER=1)
elseif (${TEST_VER} STREQUAL "v2")
  add_compile_definitions(TARGET_VER=2)
elseif (${TEST_VER} STREQUAL "v3")
  add_compile_definitions(TARGET_VER=3)
elseif (${TEST_VER} STREQUAL "v4")
  add_compile_definitions(TARGET_VER=4)
else ()
  message(FATAL_ERROR "upgrade test out of test version!")
endif ()

if (NOT ${TEST_VER} STREQUAL "v0")
  include(${CMAKE_CURRENT_SOURCE_DIR}/${TEST_TYPE}/${TEST_VER}/entity.cmake)
  include_directories(${CMAKE_CURRENT_SOURCE_DIR}/${TEST_TYPE}/${TEST_VER})
endif ()

add_executable(${PROJECT_NAME}
    main.cpp
    ${ENTITY_FILE_LIST}
    upgradetest.h
    upgradetest.cpp
    mydbupgrader.h
    mydbupgrader.cpp
    )

target_link_libraries(${PROJECT_NAME}
    Qt${QT_VERSION_MAJOR}::Core
    Qt${QT_VERSION_MAJOR}::Sql
    Qt${QT_VERSION_MAJOR}::Test
    qtdao)