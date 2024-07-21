cmake_minimum_required(VERSION 3.12)
project(qtdao)

add_subdirectory(src)

option(QTDAO_BUILD_TEST "enable qtdao test projects" OFF)

if (QTDAO_BUILD_TEST)
  message("config qtdao unit tests!")

  add_subdirectory(test/acommontestutil)

  add_subdirectory(test/corelibtest)
  add_subdirectory(test/keywordstest)
  add_subdirectory(test/customtypetest)
  add_subdirectory(test/clienttest)
  add_subdirectory(test/upgradetest)
  add_subdirectory(test/foreignkeytest)
  add_subdirectory(test/multidatabasetest)
endif ()