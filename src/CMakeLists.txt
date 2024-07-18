cmake_minimum_required(VERSION 3.12)

project(qtdao VERSION 2.2.3)

set(CMAKE_AUTOMOC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Core Sql REQUIRED)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../include)
include_directories(${CMAKE_CURRENT_BINARY_DIR}/include)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/dao.version.in ${CMAKE_CURRENT_BINARY_DIR}/include/dao.version.h @ONLY)

set(CMAKE_DEBUG_POSTFIX "d")

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
    config/configmanager.cpp

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
    configselector.cpp
)

target_link_libraries(${PROJECT_NAME}
    Qt${QT_VERSION_MAJOR}::Core
    Qt${QT_VERSION_MAJOR}::Sql
    )

target_include_directories(${PROJECT_NAME} PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../include>
    $<INSTALL_INTERFACE:include>
)

set_target_properties(${PROJECT_NAME} PROPERTIES
    VERSION ${PROJECT_VERSION}
    SOVERSION ${PROJECT_VERSION_MAJOR}
)

include(CMakePackageConfigHelpers)

install(TARGETS ${PROJECT_NAME}
    EXPORT QtDaoTargets
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
    INCLUDES DESTINATION include
)

install(DIRECTORY ${CMAKE_SOURCE_DIR}/include/
    DESTINATION include
    FILES_MATCHING PATTERN "*.h"
)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/include/dao.version.h DESTINATION include)

configure_package_config_file(${CMAKE_CURRENT_SOURCE_DIR}/QtDaoConfig.cmake.in
    "${CMAKE_CURRENT_BINARY_DIR}/QtDaoConfig.cmake"
    INSTALL_DESTINATION lib/cmake/QtDao
)

write_basic_package_version_file(
    "QtDaoConfigVersion.cmake"
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
)

install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/QtDaoConfig.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/QtDaoConfigVersion.cmake"
    DESTINATION lib/cmake/QtDao
)

install(EXPORT QtDaoTargets
    FILE QtDaoTargets.cmake
    DESTINATION lib/cmake/QtDao
)