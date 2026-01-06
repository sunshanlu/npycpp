/**
 * @file NpyReader.hpp
 * @brief NpyReader类的头文件，提供将NumPy .npy文件读取到NpData对象的功能。
 * @author ssl
 * @date 2026/1/4.
 * @copyright Copyright (c) 2026 ssl. All rights reserved.
 */

#pragma once

#include <fstream>

#include "npycpp/AbsReader.h"
#include "npycpp/NpData.hpp"

namespace npy {

/**
 * @class NpyReader
 * @brief 用于将NumPy .npy文件加载到NpData对象的读取器类。
 *
 * 此类继承自AbsReader，提供读取和解析.npy文件的功能，
 * 将其转换为NpData实例以进行进一步处理。
 */
class NpyReader final : public AbsReader {
public:
  /**
   * @brief 使用文件路径初始化读取器的构造函数。
   * @param file_path 要读取的.npy文件路径。
   */
  explicit NpyReader(const std::string &file_path);

  /**
   * @brief 从.npy文件加载数据到NpData对象。
   * @return 包含加载数据的NpData对象。
   */
  NpData Load();
};
} // namespace npy