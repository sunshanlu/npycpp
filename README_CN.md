![npycpp](./npycpp.png)

<p align="center">
  <img alt="GitHub Repo Stars" src="https://img.shields.io/github/stars/sunshanlu/npycpp">
  <img alt="GitHub Repo Forks" src="https://img.shields.io/github/forks/sunshanlu/npycpp">
</p>

<p align="center">
  <img alt="GitHub Release" src="https://img.shields.io/github/v/release/sunshanlu/npycpp">
  <img alt="GitHub Downloads (all assets, all releases)" src="https://img.shields.io/github/downloads/sunshanlu/npycpp/total">
  <img alt="GitHub Repo License" src="https://img.shields.io/github/license/sunshanlu/npycpp">
  <img alt="GitHub Repo Issues" src="https://img.shields.io/github/issues/sunshanlu/npycpp">
</p>

<p align="center">
  <a href="./README.md">English</a> |
  中文 |
</p>

# npycpp - C++ NumPy 文件格式读写库

## 项目概述

`npycpp` 是一个高性能的 `C++`库，专门用于读写 NumPy 的 `.npy` 和 `.npz` 文件格式。该库提供了与 `Python
NumPy` 生态系统的无缝数据交换能力，支持多种数据类型和任意维度数组操作，并集成了 `OpenCV` 和 `Eigen`
等主流数值计算库。

## 核心特性

### 🚀 高性能数据交换

- **零拷贝操作**: 支持与 `OpenCV` 和 `Eigen` 库的零拷贝数据转换
- **懒加载机制**: 大文件处理时仅按需加载，显著降低内存占用
- **高效序列化**: 优化的二进制格式读写性能

### 📊 格式兼容性

- **完整格式支持**: 全面支持 `.npy`（单个数组）和 `.npz`（压缩归档）格式
- **类型系统**: 支持所有 NumPy 基本数据类型（float、double、int、char 等）
- **多维数组**: 支持任意维度的张量数据存储和读取

### 🔧 框架集成

- **`OpenCV` 集成**: 原生支持 `cv::Mat`数据类型转换 (支持零拷贝)
- **`Eigen` 集成**: 无缝对接 `Eigen::Matrix`矩阵运算库（支持零拷贝）
- **标准库兼容**: 完美支持 `STL` 容器类型，使用指针创建 `STL` 容器即可（但会产生拷贝）

## 项目结构

```bash
npycpp
├── CMakeLists.txt                         # 项目构建配置文件
├── Config.cmake.in                        # CMake包配置模板
├── examples                               # 示例代码目录
│   ├── CMakeLists.txt                     # 示例构建配置
│   ├── NpycppExamples.cc                  # C++使用示例
│   ├── res                                # 示例资源目录
│   └── scripts                            # 脚本文件目录
│       └── NpypyExamples.py               # Python交互示例脚本
├── include                                # 头文件目录
│   └── npycpp                             # 库头文件
│       ├── NpData.hpp                     # 数据容器定义
│       ├── NpzReader.hpp                  # NPZ读取器定义
│       └── NpzWriter.hpp                  # NPZ写入器定义
├── README.md                              # 项目说明文档
├── src                                    # 源文件目录
│   ├── NpData.cc                          # 数据容器实现
│   ├── NpzReader.cc                       # NPZ读取器实现
│   └── NpzWriter.cc                       # NPZ写入器实现
└── test                                   # 测试代码目录
    ├── CMakeLists.txt                     # 测试构建配置
    ├── NpDataTest.cc                      # 数据容器测试
    ├── NpzReaderTest.cc                   # NPZ读取器测试
    └── NpzWriterTest.cc                   # NPZ写入器测试
```

## 快速开始

### 系统要求

- **编译器**: 支持 `C++20` 标准的编译器
- **构建工具**: `CMake 3.10+`
- **依赖库**: `OpenCV`、`Eigen3`、`Zlib`、`spdlog`

### 依赖安装

```bash
sudo apt update
sudo apt install libopencv-dev 
sudo apt install libeigen3-dev 
sudo apt install zlib1g-dev 
sudo apt install libspdlog-dev
```

### 编译安装

```bash
# 克隆并构建项目
git clone <repository-url>
mkdir build && cd build
cmake -DBUILD_TESTING=ON -DBUILD_EXAMPLES=ON -DCMAKE_BUILD_TYPE=Release ..
make -j$(nproc)

# 运行测试套件
ctest --verbose

# 安装到系统
sudo make install
```

### 项目集成

在 `CMake` 项目中集成 `npycpp`：

```cmake
find_package(npycpp REQUIRED)
target_link_libraries(<your_target> npycpp::npycpp)
```

### 基本使用示例

#### 基础数据读写

```c++
#include <npycpp/NpzWriter.hpp>
#include <npycpp/NpzReader.hpp>

// 写入数据
std::vector<float> data = {1.0f, 2.0f, 3.0f, 4.0f};
std::vector<size_t> shape = {2, 2};

// 在使用npz文件之前，必须关闭写入器，以保证文件格式的完整性
// 写入方式1：使用 RAII 确保文件正确关闭
{
    npy::NpzWriter writer("output.npz", npy::NpzWriter::Mode::W);
    writer.AddNpyData("array0", data.data(), shape);
}

// 写入方式2：直接调用Close()函数
npy::NpzWriter writer("output.npz", npy::NpzWriter::Mode::A);
writer.AddNpyData("array1", data.data(), shape);
writer.Close();

// 读取数据并使用
npy::NpzReader reader("output.npz");
npy::NpData npd = reader["array0"];

const float* ptr = npd.Ptr<float>();
```

#### 框架互操作

```c++
// OpenCV 集成
cv::Mat npd_cv = npd.CVMat<float>();              // 拷贝构造
cv::Mat npd_cv_view = npd.CVMatZC<float>();       // 零拷贝视图
// Eigen 集成  
auto npd_eigen = npd.EigenMatrix<float>();        // 拷贝构造
auto npd_eigen_map = npd.EigenMatrixZC<float>();  // 零拷贝映射
```

## 测试与验证

项目包含完整的单元测试：

- **[NpDataTest](test/NpDataTest.cc)**: 测试数据容器功能
- **[NpzReaderTest](test/NpzReaderTest.cc)**: 测试读取功能
- **[NpzWriterTest](test/NpzWriterTest.cc)**: 测试写入功能

运行测试：

```bash
ctest --verbose
```

## 示例文件

项目的[示例文件夹](examples)中包含 [`Python` 示例脚本](examples/scripts/NpypyExamples.py)和 [
`C++` 示例源文件](examples/NpycppExamples.cc)，演示了 `C++` 和 `Python` 之间的数据交换。

```bash
./bin/npycpp_example
```


## 许可证

本项目遵循 [MIT 许可证](LICENSE) - 详见 [LICENSE](LICENSE) 文件。

## 引用

[1] [cnpy](https://github.com/rogersce/cnpy) - A C++ library for reading and writing NumPy's .npy and .npz files. 
Available: https://github.com/rogersce/cnpy

---
*本项目由 ssl 创建，致力于为 C++/Python 提供高效、易用的数据交换解决方案。*