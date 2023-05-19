cmake_minimum_required(VERSION 3.2)
project(qtdaocoretest)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_AUTOMOC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Core Sql Test REQUIRED)

file(GLOB CONDITION_TEST condition/*.h condition/*.cpp)
file(GLOB LOADER_TEST loader/*.h loader/*.cpp)
file(GLOB QUERY_TEST query/*.h query/*.cpp)
file(GLOB ENTITY_SQLITE sqliteentity/*.h sqliteentity/*.cpp)
file(GLOB ENTITY_MYSQL mysqlentity/*.h mysqlentity/*.cpp)
file(GLOB ENTITY_SQLSERVER sqlserverentity/*.h sqlserverentity/*.cpp)

add_executable(${PROJECT_NAME}
  BaseTest.h
  BaseTest.cpp
  ConnectionPoolTest.h
  ConnectionPoolTest.cpp
  EngineModelSelector.h
    main.cpp
    ${CONDITION_TEST}
    ${LOADER_TEST}
    ${QUERY_TEST}
    ${ENTITY_SQLITE}
    ${ENTITY_MYSQL}
    ${ENTITY_SQLSERVER}
)

target_link_libraries(${PROJECT_NAME}
    Qt${QT_VERSION_MAJOR}::Core
    Qt${QT_VERSION_MAJOR}::Sql
    Qt${QT_VERSION_MAJOR}::Test
    qtdao::lib)