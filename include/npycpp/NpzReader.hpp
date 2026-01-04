/**
 * Created by ssl on 2025/12/30.
 * Copyright (c) 2025 ssl. All rights reserved.
 */

#pragma once

#include <string>
#include <fstream>
#include <numeric>
#include <vector>
#include <regex>
#include <unordered_map>
#include <zlib.h>

#include <spdlog/spdlog.h>

#include "npycpp/NpData.hpp"

namespace npy {
class NpzReader {
public:
	struct SeekAndLocalInfo {
		std::size_t seek_offset;    //< 文件偏移
		std::size_t word_size;      //< 单个元素大小
		std::vector<int64_t> shape; //< 元素形状
		bool fortran_order;         //< Fortran 顺序

		int64_t ComputeOffset() const {
			return std::accumulate(shape.begin(), shape.end(), 1, std::multiplies<int64_t>()) * word_size;
		}
	};

	explicit NpzReader(const std::string &npz_path);

	NpData operator[](const std::string &varname);

	NpData Load(std::string varname);

private:
	const char *ParseNpyHeader(size_t &word_size, std::vector<int64_t> &shape, bool &fortran_order);

	std::fstream file_;
	std::unordered_map<std::string, SeekAndLocalInfo> offsets_;
};
} // npy
