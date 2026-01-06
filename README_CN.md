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
  中文
</p>


# npycpp - C++ NumPy 文件格式读写库

## 项目概述

`npycpp` 是一个高性能的 `C++` 库，专门用于读写 NumPy 的 `.npy` 和 `.npz` 文件格式。该库提供了与 `Python NumPy` 生态系统的无缝数据交换能力，支持多种数据类型和任意维度数组操作，并集成了 `OpenCV` 和 `Eigen` 等主流数值计算库。

## 核心特性

### 🚀 高性能数据交换

- **零拷贝操作**: 支持与 `OpenCV` 和 `Eigen` 库的零拷贝数据转换
- **懒加载机制**: 大文件处理时仅按需加载，显著降低内存占用
- **高效序列化**: 优化的二进制格式读写性能

### 📊 格式兼容性

- **完整格式支持**: 全面支持 `.npy`（单个数组）和 `.npz`（压缩归档）格式
- **类型系统**: 支持多种基本数据类型（`float`、`double`、`int`、`char` 等）
- **多维数组**: 支持任意维度的张量数据存储和读取

### 🔧 框架集成

- **`OpenCV` 集成**: 原生支持 `cv::Mat` 数据类型转换（支持零拷贝）
- **`Eigen` 集成**: 无缝对接 `Eigen::Matrix` 矩阵运算库（支持零拷贝）
- **标准库兼容**: 完美支持 `STL` 容器类型，使用指针创建 `STL` 容器即可（不支持零拷贝）

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

#### NpzReader/NpzWriter 示例

```c++
#include <cstdint>
#include <vector>

#include <npycpp/npycpp.hpp>

// Write data
std::vector<float> data_float = {1.0f, 2.0f, 3.0f, 4.0f};
std::vector<int> data_int = {1, 2, 3, 4};
std::vector<int64_t> shape_float = {2, 2};
std::vector<int64_t> shape_int = {2, 2, 1};

// Before using npz files, the writer must be closed to ensure file format integrity
// Method 1: Use RAII to ensure file is properly closed
{

  npy::NpzWriter writer("output.npz", npy::WriteMode::W);
  writer.AddNpyData("array_float", data_float.data(), shape_float);
}

// Method 2: Directly call Close() function
npy::NpzWriter writer("output.npz", npy::WriteMode::A);
writer.AddNpyData("array_int", data_int.data(), shape_int);
writer.Close();

// Read data and use
npy::NpzReader reader("output.npz");
npy::NpData npd = reader["array_float"];

// Access data
const float *ptr = npd.Ptr<float>();
```

#### NpyReader/NpyWriter 示例

```c++
#include <cstdint>
#include <vector>

#include <npycpp/npycpp.hpp>

// Writing NPY file using NpyWriter
std::vector<double> data = {1.0, 2.0, 3.0, 4.0, 5.0, 6.0};
std::vector<int64_t> shape = {2, 3};
npy::NpyWriter writer("matrix.npy");
writer.SaveNpyData(data.data(), shape);
writer.Close();

// Reading NPY file using NpyReader
npy::NpyReader reader("matrix.npy");
npy::NpData npd = reader.Load();

// Access data
const double *loaded_data = npd.Ptr<double>();
```

#### 数据容器示例

```c++
#include <cstdint>
#include <vector>

#include <npycpp/npycpp.hpp>
#include <fmt/format.h>

npy::NpData npd_data = npy::NpyReader("matrix.npy").Load();

// Print array shape
fmt::print("Array shape: [{}]\n", fmt::join(npd_data.Shape(), ", "));

// access data
const double *npd_data_ptr = npd_data.Ptr<double>();
for (int i = 0; i < npd_data.Elements(); ++i)
  fmt::print("{:>5.2f} ", npd_data_ptr[i]);

fmt::print("\n");

// OpenCV Integration
cv::Mat npd_cv = npd_data.CVMat<double>();        // Copy construction
cv::Mat npd_cv_view = npd_data.CVMatZC<double>(); // Zero-copy view

// Eigen Integration
auto npd_eigen = npd_data.EigenMatrix<double>();       // Copy construction
auto npd_eigen_map = npd_data.EigenMatrixZC<double>(); // Zero-copy mapping

// STL Integration (Copy construction)
std::vector<double> npd_stl(npd_data.Ptr<double>(), npd_data.Ptr<double>() + npd_data.Elements());

fmt::print("STL Wrapper: [{}]", fmt::join(npd_stl, ", "));
std::cout << "Eigen map Wrapper: \n" << npd_eigen_map << std::endl;
std::cout << "CV View Wrapper: \n" << npd_cv_view << std::endl;
```

## 测试与验证

项目包含完整的单元测试：

- **[NpDataTest](test/NpDataTest.cc)**: 测试数据容器功能
- **[NpzReaderTest](test/NpzReaderTest.cc)**: 测试 npz 文件读取功能
- **[NpzWriterTest](test/NpzWriterTest.cc)**: 测试 npz 文件写入功能
- **[NpyReaderTest](test/NpyReaderTest.cc)**: 测试 npy 文件读取功能
- **[NpyWriterTest](test/NpyWriterTest.cc)**: 测试 npy 文件写入功能

运行测试：

```bash
ctest --verbose
```

## 示例文件

项目的 [example文件夹](examples) 包含 `Python` 示例脚本和 `C++` 示例源文件，用于 npy 和 npz 文件的读写，演示 `C++` 和 `Python` 之间的数据交换。

```bash
./bin/npycpp_example # npy 文件示例
./bin/npzcpp_example # npz 文件示例
```

## 许可证

本项目遵循 [MIT 许可证](LICENSE) - 详见 [LICENSE](LICENSE) 文件。

## 引用

[1] [cnpy](https://github.com/rogersce/cnpy) - 用于读写 NumPy 的 .npy 和 .npz 文件的 C++ 库。可用地址: https://github.com/rogersce/cnpy

---

*本项目由 ssl 创建和维护，致力于为 C++/Python 提供高效、易用的数据交换解决方案。* 