/**
 * @file AbsReader.h
 * @brief AbsReader抽象类的头文件，提供读取NumPy文件的基本功能。
 * @author ssl
 * @date 2026/1/4
 * @copyright Copyright (c) 2026 ssl. All rights reserved.
 */

#pragma once

#include <fstream>
#include <string>
#include <vector>

#include <unordered_map>

#include "npycpp/NpData.hpp"

namespace npy {

/**
 * @struct SeekAndLocalInfo
 * @brief 保存数据加载的seek偏移和本地信息的结构体。
 */
struct SeekAndLocalInfo {
  std::size_t seek_offset;    ///< 文件偏移。
  std::size_t word_size;      ///< 单个元素的大小。
  std::vector<int64_t> shape; ///< 元素形状。
  bool fortran_order;         ///< Fortran顺序标志。

  /**
   * @brief 计算偏移。
   * @return 计算出的偏移。
   */
  int64_t ComputeOffset() const;
};

/**
 * @class AbsReader
 * @brief NumPy文件读取器的抽象基类。
 *
 * 此类提供读取.npy和.npz文件的通用功能，包括解析头部和加载数据。
 */
class AbsReader {
public:
  /**
   * @brief 构造函数，使用文件路径初始化读取器。
   * @param npz_path 要读取的文件路径。
   */
  explicit AbsReader(const std::string &npz_path);

  /**
   * @brief 虚析构函数。
   */
  virtual ~AbsReader() = default;

protected:
  /**
   * @brief 解析NPY头部。
   * @param word_size 输出参数：字大小。
   * @param shape 输出参数：形状。
   * @param fortran_order 输出参数：Fortran顺序。
   * @return const char指针（成功时为nullptr，否则为错误消息）。
   */
  const char *ParseNpyHeader(size_t &word_size, std::vector<int64_t> &shape, bool &fortran_order);

  /**
   * @brief 基于seek和本地信息加载数据的内部方法。
   * @param sinfo seek和本地信息。
   * @return 包含加载数据的NpData对象。
   */
  NpData LoadInternal(const SeekAndLocalInfo &sinfo) {
    const auto [seek_offset, word_size, shape, fortran_order] = sinfo;
    file_.seekg(seek_offset);
    NpData data(shape, word_size, fortran_order);
    file_.read(data.Ptr<char>(), data.NumBytes());
    return data;
  }

  std::fstream file_;                                         ///< 用于读取的文件流。
  std::unordered_map<std::string, SeekAndLocalInfo> offsets_; ///< 数据条目的偏移映射。
};
} // namespace npy