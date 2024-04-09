cmake_minimum_required(VERSION 3.12)
project(qtdaomultidatabasetest)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_AUTOMOC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Core Sql Test REQUIRED)

include(${CMAKE_CURRENT_SOURCE_DIR}/sqlite/v1/entity.cmake)
include(${CMAKE_CURRENT_SOURCE_DIR}/sqlite/v2/entity.cmake)
include(${CMAKE_CURRENT_SOURCE_DIR}/mysql/entity.cmake)
include(${CMAKE_CURRENT_SOURCE_DIR}/sqlserver/entity.cmake)

add_executable(${PROJECT_NAME}
    main.cpp
    multidbtest.h
    multidbtest.cpp
    ${ENTITY_FILE_LIST}
  )

target_link_libraries(${PROJECT_NAME}
    Qt${QT_VERSION_MAJOR}::Core
    Qt${QT_VERSION_MAJOR}::Sql
    Qt${QT_VERSION_MAJOR}::Test
    qtdao::lib)