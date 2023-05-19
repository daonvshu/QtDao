cmake_minimum_required(VERSION 3.2)
project(qtdaolib)

set(CMAKE_AUTOMOC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Core Sql REQUIRED)

include_directories(${CMAKE_SOURCE_DIR}/include)

add_library(${PROJECT_NAME}
    builder/frombuilder.cpp
    builder/unionbuilder.cpp

    condition/conditionconstraint.cpp
    condition/entitycondition.cpp
    condition/functioncondition.cpp

    condition/connector/connectable.cpp
    condition/connector/entityconnector.cpp
    condition/connector/groupconnector.cpp

    config/configbuilder.cpp
    config/configsqlite.cpp
    config/configmysql.cpp
    config/configsqlserver.cpp

    dbclients/abstractclient.cpp
    dbclients/mysqlclient.cpp
    dbclients/sqliteclient.cpp
    dbclients/sqlserverclient.cpp

    query/basequery.cpp
    query/insertimpl.cpp
    query/insertintoselectimpl.cpp
    query/updateimpl.cpp
    query/deleteimpl.cpp
    query/selectimpl.cpp
    query/joinimpl.cpp
    query/upsertimpl.cpp

    utils/logging.cpp
    utils/dbupgrader.cpp

    versionctl/versioncontrol.cpp

    connectionpool.cpp
    qtdao.cpp
)

target_include_directories(${PROJECT_NAME} PUBLIC ../include)

add_library(qtdao::lib ALIAS ${PROJECT_NAME})

target_link_libraries(${PROJECT_NAME}
    Qt${QT_VERSION_MAJOR}::Core
    Qt${QT_VERSION_MAJOR}::Sql
    )