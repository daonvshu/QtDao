project(common-test-util)

set(CMAKE_AUTOMOC ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Core Test REQUIRED)

add_compile_definitions(TEST_CONFIG_PATH="${CMAKE_CURRENT_SOURCE_DIR}/test-config.json")

if (WIN32)
    add_definitions(-DUNICODE)
    add_definitions(-D_UNICODE)
endif()

add_library(${PROJECT_NAME}
    multitestrunner.h

    utils/testutils.h
    utils/testutils.cpp
    utils/testconfigloader.h
    utils/testconfigloader.cpp
)

target_link_libraries(${PROJECT_NAME}
    Qt${QT_VERSION_MAJOR}::Core
    Qt${QT_VERSION_MAJOR}::Test
)

target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
