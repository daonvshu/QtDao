cmake_minimum_required(VERSION 3.2)
project(qtdaocustomtypetest)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_AUTOMOC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

find_package(Qt5 COMPONENTS Core Sql Test REQUIRED)

include(${CMAKE_CURRENT_SOURCE_DIR}/entity/entity.cmake)
add_executable(${PROJECT_NAME}
  main.cpp
  customtypetest.h
  customtypetest.cpp
  ${ENTITY_FILE_LIST}
  )

target_link_libraries(${PROJECT_NAME} Qt5::Core Qt5::Sql Qt5::Test qtdao::lib)