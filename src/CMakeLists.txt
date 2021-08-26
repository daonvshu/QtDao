cmake_minimum_required(VERSION 3.2)
project(qtdaolib)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_AUTOMOC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

find_package(Qt5 COMPONENTS Core Sql REQUIRED)

add_library(${PROJECT_NAME}
    builder/BaseQueryBuilder.cpp

    condition/ConditionConstraint.cpp
    condition/ConnectorData.cpp
    condition/EntityCondition.cpp
    condition/FunctionCondition.cpp

    dbclients/AbstractClient.cpp
    dbclients/MysqlClient.cpp
    dbclients/SqliteClient.cpp
    dbclients/SqlServerClient.cpp

    query/BaseQuery.cpp
    query/SqliteLockControl.cpp

    ConnectionPool.cpp
    DbLoader.cpp
    qtdao.cpp
)

target_include_directories(${PROJECT_NAME} PUBLIC ../include)

add_library(qtdao::lib ALIAS ${PROJECT_NAME})

target_link_libraries(${PROJECT_NAME} Qt5::Core Qt5::Sql)