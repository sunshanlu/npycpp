/**
 * @file NpyWriter.hpp
 * @brief NpyWriter类的头文件，提供从数据写入NumPy .npy文件的功能。
 * @author ssl
 * @date 2026/1/4
 * @copyright Copyright (c) 2026 ssl. All rights reserved.
 */

#pragma once

#include <numeric>

#include "npycpp/AbsReader.h"
#include "npycpp/AbsWriter.hpp"

namespace npy {

/**
 * @class NpyWriter
 * @brief 用于将数据保存到NumPy .npy文件的写入器类。
 *
 * 此类继承自AbsWriter，提供将数据数组写入.npy文件的功能，
 * 具有指定的形状和类型。
 */
class NpyWriter final : public AbsWriter {
public:
    /**
     * @brief 使用文件路径初始化写入器的构造函数。
     * @param npz_path 要写入的.npy文件路径。
     */
    NpyWriter(const std::string &npz_path);

    /**
     * @brief 将提供的数据保存到.npy文件。
     * @tparam T 数据类型。
     * @param data 数据数组指针。
     * @param shape 数据数组的形状。
     * @return const char指针（成功时为nullptr，否则为错误消息）。
     */
    template<typename T>
    const char *SaveNpyData(const T *data, const std::vector<int64_t> &shape);

    /**
     * @brief 关闭写入器并完成文件。
     * @return const char指针（成功时为nullptr，否则为错误消息）。
     */
    const char *Close() override;
};

/**
 * @brief 保存数据的模板实现。
 * @tparam T 数据类型。
 * @param data 数据数组指针。
 * @param shape 数据数组的形状。
 * @return const char指针（成功时为nullptr，否则为错误消息）。
 */
template<typename T>
const char *NpyWriter::SaveNpyData(const T *data, const std::vector<int64_t> &shape) {
    int64_t elements = std::accumulate(shape.begin(), shape.end(), 1, std::multiplies<int64_t>());
    auto npy_header = CreateNpyHeader<T>(shape);

    file_.write(npy_header.data(), npy_header.size());
    file_.write(reinterpret_cast<const char *>(data), elements * sizeof(T));

    return nullptr;
}
} // namespace npy