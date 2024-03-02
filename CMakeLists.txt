cmake_minimum_required(VERSION 3.12)
project(qtdao)

add_subdirectory(src)

option(BUILD_TEST "enable test projects" OFF)

if (BUILD_TEST)
  message("config qtdao unit tests!")

  add_subdirectory(test/corelibtest)
  add_subdirectory(test/keywordstest)
  add_subdirectory(test/customtypetest)
  add_subdirectory(test/clienttest)
  add_subdirectory(test/upgradetest)
  add_subdirectory(test/foreignkeytest)
endif ()