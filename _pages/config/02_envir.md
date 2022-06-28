---
title: 环境配置
category: Jekyll
layout: post
---

在配置环境前，经过上一个步骤编写的entity.xml数据表配置后，在VSCode中搜索插件`QtDao`，使用VSCode打开entity.xml，右键`'create entity for qtdao'`，在当前目录下生成c++模型类和配置相关文件。

Qt Creator / VS
-------------
使用前，使用`QtCreator`打开`qtdaolib/qtdaolib.pro`，选择目标Qt库构建套件，最后执行`构建`，构建出qtdao静态链接库，在`lib`目录中找到编译出的库。

#### 项目中导入库
- QtCreator

复制编译好的静态链接库和`include`文件夹到项目文件夹中，在项目pro文件中添加如下脚本导入库：
```
QT += core gui widgets sql

CONFIG += c++11

SOURCES += \
    main.cpp \
    widget.cpp

HEADERS += \
    widget.h \
    entity/sqliteconfig.h #添加数据库连接配置文件

FORMS += \
    widget.ui

INCLUDEPATH += $$PWD/include

CONFIG(debug, debug|release){
    LIBS += $$PWD/lib/x64/qtdao_d.lib
} else {
    LIBS += $$PWD/lib/x64/qtdao.lib
}

#添加entity文件列表，entity.pri由vscode-qtdao插件自动生成
include(entity/entity.pri)
```

- Visual Studio

在VS中使用Qt前，确保安装`Qt Visual Studio Tools`插件以提供Qt库配置的支持。同样的，复制编译好的静态链接库和`include`文件夹到项目文件夹中，在 `项目 -> 属性` 中配置链接库等，并手动将生成的c++模型类文件添加到`include`列表中，如下x64 debug环境配置：

|设置|值|
|:--:|:--:|
|`Qt Project Settings -> General -> Qt Modules`|勾选Sql模块|
|`C/C++ -> 常规 -> 附加包含目录`|`$(ProjectDir)include`|
|`链接器 -> 常规 -> 附加库目录`|`$(ProjectDir)lib\x64`|
|`链接器 -> 输入 -> 附加依赖项`|`qtdao_d.lib`|


CMake
-------------
这里推荐大家使用CMake编译Qt应用程序，使用CMake脚本配置Qt将不再受编辑器IDE限制（选择自己喜欢的IDE如：Visual Studio、CLion、QtCreator、VS Code等），使用CMake脚本配置Qt环境使链接到第三方库、切换Qt环境等更加方便。使用IDE提供的CMake支持还可以方便的切换编译器，如msvc/gcc/clang等构建工具，同时方便的在wsl或远程linux服务器上交叉编译linux应用程序。使用cmake配置时，不必单独编译静态链接库，如下脚本添加qtdao库：
```cmake
#添加qtdao库子项目
add_subdirectory(${CMAKE_SOURCE_DIR}/3rdparty/qtdao/src)

#添加entity文件列表，entity.cmake由vscode-qtdao插件自动生成
include(${CMAKE_CURRENT_SOURCE_DIR}/entity/entity.cmake)

#...
add_executable(${PROJECT_NAME} WIN32 
    #...

    #添加数据库连接配置文件
    entity/sqliteconfig.h
    #添加entity文件列表，此变量由entity.cmake提供
    ${ENTITY_FILE_LIST}
)

target_link_libraries(${PROJECT_NAME}
    #...

    #静态链接到qtdao库
    qtdao::lib
)
```