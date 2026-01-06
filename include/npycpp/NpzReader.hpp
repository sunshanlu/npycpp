/**
 * @file NpzReader.hpp
 * @brief NpzReader类的头文件，提供将NumPy .npz文件读取到NpData对象的功能。
 * @author ssl
 * @date 2025/12/30
 * @copyright Copyright (c) 2025 ssl. All rights reserved.
 */

#pragma once

#include <spdlog/spdlog.h>
#include <string>

#include "npycpp/AbsReader.h"
#include "npycpp/NpData.hpp"

namespace npy {

/**
 * @class NpzReader
 * @brief 用于将NumPy .npz文件加载到NpData对象的读取器类。
 *
 * 此类继承自AbsReader，提供读取和解析.npz文件的功能，
 * 允许按变量名访问单个数组。
 */
class NpzReader : public AbsReader {
public:
  /**
   * @brief 使用文件路径初始化读取器的构造函数。
   * @param npz_path 要读取的.npz文件路径。
   */
  explicit NpzReader(const std::string &npz_path);

  /**
   * @brief 使用operator[]按变量名访问数据数组。
   * @param varname .npz文件中变量的名称。
   * @return 包含加载数据的NpData对象。
   */
  NpData operator[](const std::string &varname);

  /**
   * @brief 按变量名加载数据数组。
   * @param varname .npz文件中变量的名称。
   * @return 包含加载数据的NpData对象。
   */
  NpData Load(std::string varname);
};
} // namespace npy