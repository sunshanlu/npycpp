/**
 * @file AbsWriter.hpp
 * @brief AbsWriter抽象类的头文件，提供写入NumPy文件的基本功能。
 * @author ssl
 * @date 2026/1/5
 * @copyright Copyright (c) 2026 ssl. All rights reserved.
 */

#pragma once

#include <cstdint>
#include <filesystem>
#include <fstream>
#include <string>
#include <typeinfo>
#include <vector>

#include <spdlog/spdlog.h>

namespace npy {

/**
 * @enum WriteMode
 * @brief 写入模式的枚举。
 */
enum class WriteMode {
  W, ///< 写入模式（创建新文件）。
  A  ///< 追加模式（追加到现有文件）。
};

/**
 * @class AbsWriter
 * @brief NumPy文件写入器的抽象基类。
 *
 * 此类提供写入.npy和.npz文件的通用功能，
 * 包括创建头部和管理文件流。
 */
class AbsWriter {
public:
  /**
   * @brief 使用文件路径和模式初始化写入器的构造函数。
   * @param file_path 要写入的文件路径。
   * @param mode 写入模式（默认为WriteMode::W）。
   */
  AbsWriter(const std::string &file_path, WriteMode mode = WriteMode::W) {
    if (file_path.empty()) {
      SPDLOG_WARN("npz_path is empty");
      path_invalid_ = true;
      return;
    }

    namespace fs = std::filesystem;

    try {
      const fs::path npz_path_abs = fs::absolute(file_path);
      const fs::path dir_path = npz_path_abs.parent_path();

      if (!fs::exists(dir_path)) {
        fs::create_directories(dir_path);

        if (mode == WriteMode::A) {
          SPDLOG_WARN("there is no npz_path, mode is changed to W");
          mode = WriteMode::W;
        }
      }

      switch (mode) {
      case WriteMode::W:
        file_.open(npz_path_abs, std::ios::out | std::ios::binary);
        break;
      case WriteMode::A:
        file_.open(npz_path_abs, std::ios::out | std::ios::in | std::ios::binary);
        break;
      }
    } catch (const fs::filesystem_error &) {
      path_invalid_ = true;
      SPDLOG_WARN("input file is invalid, please check it");
      return;
    }
  }

  /**
   * @brief 虚析构函数。
   */
  virtual ~AbsWriter() = default;

  /**
   * @brief 关闭写入器并完成文件。
   * @return const char指针（成功时为nullptr，否则为错误消息）。
   */
  virtual const char *Close() = 0;

protected:
  /**
   * @brief 将值转换为字节字符串。
   * @tparam T 数据类型。
   * @param rhs 要转换的值。
   * @return 字节的字符串表示。
   */
  template <typename T>
  static std::string CastToString(const T &rhs);

  /**
   * @brief 将类型映射到其NumPy类型字符。
   * @param t 类型信息。
   * @return NumPy类型字符。
   */
  static char MapType(const std::type_info &t);

  /**
   * @brief 测试大端字节顺序。
   * @return 字节顺序字符。
   */
  static char BigEndianTest();

  /**
   * @brief 为给定形状创建NPY头部。
   * @tparam T 数据类型。
   * @param shape 数据数组的形状。
   * @return NPY头部字符串。
   */
  template <typename T>
  static std::string CreateNpyHeader(const std::vector<int64_t> &shape);

  std::fstream file_;
  bool path_invalid_ = false;
  bool is_closed_ = false;
};

/**
 * @brief 转换为字符串的模板实现。
 * @tparam T 数据类型。
 * @param rhs 要转换的值。
 * @return 字节的字符串表示。
 */
template <typename T>
std::string AbsWriter::CastToString(const T &rhs) {
  std::string res;
  res.reserve(sizeof(T)); // 预分配空间以提高性能

  auto *bytes = reinterpret_cast<const unsigned char *>(&rhs);
  for (std::size_t byte = 0; byte < sizeof(T); byte++)
    res += static_cast<char>(bytes[byte]);

  return res;
}

/**
 * @brief 创建NPY头部的模板实现。
 * @tparam T 数据类型。
 * @param shape 数据数组的形状。
 * @return NPY头部字符串。
 */
template <typename T>
std::string AbsWriter::CreateNpyHeader(const std::vector<int64_t> &shape) {
  std::string dict;

  dict += "{'descr': '";
  dict += BigEndianTest();
  dict += MapType(typeid(T));
  dict += std::to_string(sizeof(T));
  dict += "', 'fortran_order': False, 'shape': (";
  dict += std::to_string(shape[0]);

  for (size_t i = 1; i < shape.size(); i++) {
    dict += ", ";
    dict += std::to_string(shape[i]);
  }

  if (shape.size() == 1)
    dict += ",";
  dict += "), }";
  const int remainder = 16 - (10 + dict.size()) % 16;
  dict.insert(dict.end(), remainder, ' ');
  dict.back() = '\n';

  std::string header;
  header += static_cast<char>(0x93);
  header += "NUMPY";
  header += static_cast<char>(0x01);
  header += static_cast<char>(0x00);
  header += CastToString(static_cast<uint16_t>(dict.size()));
  header.insert(header.end(), dict.begin(), dict.end());

  return header;
}
} // namespace npy