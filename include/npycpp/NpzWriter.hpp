/**
 * @file NpzWriter.hpp
 * @brief NpzWriter类的头文件，提供从数据写入NumPy .npz文件的功能。
 * @author ssl
 * @date 2026/1/6
 * @copyright Copyright (c) 2026 ssl. All rights reserved.
 */

#pragma once

#include <filesystem>
#include <fstream>
#include <numeric>
#include <string>
#include <zlib.h>

#include <spdlog/spdlog.h>

#include "npycpp/AbsWriter.hpp"

namespace npy {

/**
 * @class NpzWriter
 * @brief 用于将数据保存到NumPy .npz文件的写入器类。
 *
 * 此类继承自AbsWriter，提供将多个数据数组写入.npz文件的功能，
 * 具有指定的名称、形状和类型。
 */
class NpzWriter final : public AbsWriter {
public:
  /**
   * @brief 使用文件路径和模式初始化写入器的构造函数。
   * @param npz_path 要写入的.npz文件路径。
   * @param mode 写入模式。
   */
  NpzWriter(const std::string &npz_path, WriteMode mode = WriteMode::W);

  /**
   * @brief 将数据数组添加到.npz文件。
   *
   * 此方法创建NPY头部、计算CRC32、构建本地头部、
   * 将信息添加到全局头部，并写入数据。
   *
   * @tparam T 数据类型。
   * @param name NPY数组的名称。
   * @param data 数据数组指针。
   * @param shape 数据数组的形状。
   * @return const char指针（成功时为nullptr，否则为错误消息）。
   */
  template <typename T>
  const char *AddNpyData(const std::string &name, const T *data, const std::vector<int64_t> &shape) {
    const std::string fname = name + ".npy";

    const std::string npy_header = CreateNpyHeader<T>(shape);
    const int64_t nelements = std::accumulate(shape.begin(), shape.end(), 1, std::multiplies<int64_t>());
    const int64_t nbytes = nelements * sizeof(T) + npy_header.size();

    uint32_t crc = crc32(0L, reinterpret_cast<const uint8_t *>(npy_header.data()), npy_header.size());
    crc = crc32(crc, reinterpret_cast<const uint8_t *>(data), nelements * sizeof(T));

    const std::string local_header = BuildLocalHeader(fname, crc, nbytes);
    AddInfoToGlobalHeader(fname, local_header);

    global_header_offset_ += (nbytes + local_header.size());
    records_nums_ += 1;

    file_.write(local_header.data(), local_header.size());
    file_.write(npy_header.data(), npy_header.size());
    file_.write(reinterpret_cast<const char *>(data), sizeof(T) * nelements);
    return nullptr;
  }

  /**
   * @brief 析构函数。
   */
  ~NpzWriter() override;

  /**
   * @brief 关闭写入器并完成文件。
   * @return const char指针（成功时为nullptr，否则为错误消息）。
   */
  const char *Close() override;

private:
  /**
   * @brief 为文件条目构建本地头部。
   * @param fname 文件名。
   * @param crc CRC32校验和。
   * @param nbytes 字节数。
   * @return 本地头部字符串。
   */
  std::string BuildLocalHeader(const std::string &fname, const uint32_t &crc, const int &nbytes);

  /**
   * @brief 将信息添加到全局头部。
   * @param fname 文件名。
   * @param local_header 本地头部字符串。
   */
  void AddInfoToGlobalHeader(const std::string &fname, const std::string &local_header);

  /**
   * @brief 构建NPZ文件的尾部。
   * @return 尾部字符串。
   */
  std::string BuildFooter();

  /**
   * @brief 处理稀疏NPZ尾部。
   * @return const char指针（成功时为nullptr，否则为错误消息）。
   */
  const char *SparseNpzFooter();

  std::string global_header_;
  uint32_t global_header_offset_ = 0;
  uint16_t records_nums_ = 0;
};
} // namespace npy