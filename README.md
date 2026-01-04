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
  English |
  <a href="./README_CN.md">中文</a> |
</p>

# npycpp - C++ Library for Reading and Writing NumPy File Formats

## Project Overview

`npycpp` is a high-performance `C++` library specifically designed for reading and writing NumPy's `.npy` and `.npz` file formats. The library provides seamless data exchange capabilities with the `Python NumPy` ecosystem, supporting multiple data types and arbitrary-dimensional array operations, while integrating mainstream numerical computing libraries such as `OpenCV` and `Eigen`.

## Core Features

### 🚀 High-Performance Data Exchange

- **Zero-Copy Operations**: Supports zero-copy data conversion with `OpenCV` and `Eigen` libraries
- **Lazy Loading**: On-demand loading during large file processing, significantly reducing memory usage
- **Efficient Serialization**: Optimized binary format read/write performance

### 📊 Format Compatibility

- **Complete Format Support**: Full support for `.npy` (single array) and `.npz` (compressed archive) formats
- **Type System**: Supports all NumPy basic data types (float, double, int, char, etc.)
- **Multi-Dimensional Arrays**: Supports storage and retrieval of tensor data with arbitrary dimensions

### 🔧 Framework Integration

- **`OpenCV` Integration**: Native support for `cv::Mat` data type conversion (supports zero-copy)
- **`Eigen` Integration**: Seamless integration with `Eigen::Matrix` matrix computation library (supports zero-copy)
- **Standard Library Compatibility**: Perfectly supports `STL` container types, using pointers to create `STL` containers (but will generate copies)

## Project Structure

```bash
npycpp
├── CMakeLists.txt                         # Project build configuration file
├── Config.cmake.in                        # CMake package configuration template
├── examples                               # Example code directory
│   ├── CMakeLists.txt                     # Example build configuration
│   ├── NpycppExamples.cc                  # C++ usage examples
│   ├── res                                # Example resource directory
│   └── scripts                            # Script files directory
│       └── NpypyExamples.py               # Python interaction example script
├── include                                # Header files directory
│   └── npycpp                             # Library header files
│       ├── NpData.hpp                     # Data container definition
│       ├── NpzReader.hpp                  # NPZ reader definition
│       └── NpzWriter.hpp                  # NPZ writer definition
├── README.md                              # Project documentation
├── src                                    # Source files directory
│   ├── NpData.cc                          # Data container implementation
│   ├── NpzReader.cc                       # NPZ reader implementation
│   └── NpzWriter.cc                       # NPZ writer implementation
└── test                                   # Test code directory
    ├── CMakeLists.txt                     # Test build configuration
    ├── NpDataTest.cc                      # Data container tests
    ├── NpzReaderTest.cc                   # NPZ reader tests
    └── NpzWriterTest.cc                   # NPZ writer tests
```

## Quick Start

### System Requirements

- **Compiler**: Compiler supporting `C++20` standard
- **Build Tool**: `CMake 3.10+`
- **Dependencies**: `OpenCV`, `Eigen3`, `Zlib`, `spdlog`

### Dependency Installation

```bash
sudo apt update
sudo apt install libopencv-dev 
sudo apt install libeigen3-dev 
sudo apt install zlib1g-dev 
sudo apt install libspdlog-dev
```

### Build and Install

```bash
# Clone and build project
git clone <repository-url>
mkdir build && cd build
cmake -DBUILD_TESTING=ON -DBUILD_EXAMPLES=ON -DCMAKE_BUILD_TYPE=Release ..
make -j$(nproc)

# Run test suite
ctest --verbose

# Install to system
sudo make install
```

### Project Integration

Integrate `npycpp` in your `CMake` project:

```cmake
find_package(npycpp REQUIRED)
target_link_libraries(<your_target> npycpp::npycpp)
```

### Basic Usage Examples

#### Basic Data Read/Write

```cpp
#include <npycpp/NpzWriter.hpp>
#include <npycpp/NpzReader.hpp>

// Write data
std::vector<float> data = {1.0f, 2.0f, 3.0f, 4.0f};
std::vector<size_t> shape = {2, 2};

// Before using npz files, the writer must be closed to ensure file format integrity
// Method 1: Use RAII to ensure file is properly closed
{
    npy::NpzWriter writer("output.npz", npy::NpzWriter::Mode::W);
    writer.AddNpyData("array0", data.data(), shape);
}

// Method 2: Directly call Close() function
npy::NpzWriter writer("output.npz", npy::NpzWriter::Mode::A);
writer.AddNpyData("array1", data.data(), shape);
writer.Close();

// Read data and use
npy::NpzReader reader("output.npz");
npy::NpData npd = reader["array0"];

const float* ptr = npd.Ptr<float>();
```

#### Framework Interoperability

```cpp
// OpenCV Integration
cv::Mat npd_cv = npd.CVMat<float>();              // Copy construction
cv::Mat npd_cv_view = npd.CVMatZC<float>();       // Zero-copy view
// Eigen Integration  
auto npd_eigen = npd.EigenMatrix<float>();        // Copy construction
auto npd_eigen_map = npd.EigenMatrixZC<float>();  // Zero-copy mapping
```

## Testing and Validation

The project includes comprehensive unit tests:

- **[NpDataTest](test/NpDataTest.cc)**: Tests data container functionality
- **[NpzReaderTest](test/NpzReaderTest.cc)**: Tests read functionality
- **[NpzWriterTest](test/NpzWriterTest.cc)**: Tests write functionality

Run tests:

```bash
ctest --verbose
```

## Example Files

The project's [example folder](examples) contains a [`Python` example script](examples/scripts/NpypyExamples.py) and a [`C++` example source file](examples/NpycppExamples.cc), demonstrating data exchange between `C++` and `Python`.

```bash
./bin/npycpp_example
```

## License

This project follows the [MIT License](LICENSE) - see the [LICENSE](LICENSE) file for details.

## References

[1] [cnpy](https://github.com/rogersce/cnpy) - A C++ library for reading and writing NumPy's .npy and .npz files. Available: https://github.com/rogersce/cnpy

---

*This project is created and maintained by ssl, dedicated to providing efficient and easy-to-use C++/Python data exchange solutions.*