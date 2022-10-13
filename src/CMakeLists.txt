cmake_minimum_required(VERSION 3.2)
project(qtdaolib)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_AUTOMOC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

find_package(Qt5 COMPONENTS Core Sql REQUIRED)

include_directories(${CMAKE_SOURCE_DIR}/include)

add_library(${PROJECT_NAME}
    builder/basequerybuilder.cpp

    condition/conditionconstraint.cpp
    condition/connectordata.cpp
    condition/entitycondition.cpp
    condition/functioncondition.cpp

    config/configbuilder.cpp
    config/configsqlite.cpp
    config/configmysql.cpp
    config/configsqlserver.cpp

    dbclients/abstractclient.cpp
    dbclients/mysqlclient.cpp
    dbclients/sqliteclient.cpp
    dbclients/sqlserverclient.cpp

    query/basequery.cpp
    query/sqlitelockcontrol.cpp
    query/insertimpl.cpp
    query/insertintoselectimpl.cpp
    query/updateimpl.cpp
    query/deleteimpl.cpp
    query/selectimpl.cpp
    query/joinimpl.cpp

    connectionpool.cpp
    qtdao.cpp
)

target_include_directories(${PROJECT_NAME} PUBLIC ../include)

add_library(qtdao::lib ALIAS ${PROJECT_NAME})

target_link_libraries(${PROJECT_NAME} Qt5::Core Qt5::Sql)