---
sidebar_position: 2
---

# 安装

- 依赖库：QtDao库在`Qt5.12.3`、`Qt5.15.2`、`Qt6.5.3`等版本下进行了编译和运行测试，其他库可能需要你自己测试验证。
- 编译器：QtDao库使用了`MSVC`、`aarch64-linux-6.3.1`、`aarch64-linux-7.5.0`、`arm-linux-8.3`等工具链进行编译测试，不保证其他工具链能成功编译。

## 编译安装

进入到`src`目录，并创建`build`文件夹，进入到`build`文件夹中。

1. 如果使用`msvc`，可以使用如下命令：

```bash
# 执行cmake配置
cmake .. -DCMAKE_PREFIX_PATH=path/to/your/qt/library -DCMAKE_INSTALL_PREFIX=path/to/your/installdir/qtdao -G "Visual Studio 17 2022"
# 执行Debug编译和安装
cmake --build . --config Debug --target INSTALL
```

2. 如果使用`gcc`，可以使用如下命令：

```bash
# 配置为Debug
cmake .. -DCMAKE_BUILD_TYPE=Debug -DCMAKE_PREFIX_PATH=path/to/your/qt/library -DCMAKE_INSTALL_PREFIX=path/to/your/installdir/qtdao
# 执行编译和安装
make && make install
```

## 导入到工程中

可以到安装目录中直接找到头文件和库进行链接使用。如果使用`cmake`导入库，可以在`CMakeLists.txt`脚本中添加如下代码：

```bash
list(APPEND CMAKE_PREFIX_PATH path/to/your/installdir)

find_package(QtDao 2.3.0 REQUIRED)

target_link_libraries(${PROJECT_NAME} qtdao)
```