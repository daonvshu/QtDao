cmake_minimum_required(VERSION 3.12)
project(qtdaocoretest)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_AUTOMOC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Core Sql Test REQUIRED)

file(GLOB OTHER_TEST other/*.h other/*.cpp)
file(GLOB QUERY_TEST query/*.h query/*.cpp)
file(GLOB ENTITY_SQLITE entity/sqliteentity/*.h entity/sqliteentity/*.cpp)
file(GLOB ENTITY_MYSQL entity/mysqlentity/*.h entity/mysqlentity/*.cpp)
file(GLOB ENTITY_SQLSERVER entity/sqlserverentity/*.h entity/sqlserverentity/*.cpp)

add_executable(${PROJECT_NAME}
    BaseTest.h
    BaseTest.cpp
    ../acommontestutil/databaseselector.h
    main.cpp

    ${OTHER_TEST}
    ${QUERY_TEST}
    ${ENTITY_SQLITE}
    ${ENTITY_MYSQL}
    ${ENTITY_SQLSERVER}
)

target_link_libraries(${PROJECT_NAME}
    Qt${QT_VERSION_MAJOR}::Core
    Qt${QT_VERSION_MAJOR}::Sql
    Qt${QT_VERSION_MAJOR}::Test

    qtdao
    common-test-util
)