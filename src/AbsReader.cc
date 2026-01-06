/**
 * Created by ssl on 2026/1/4.
 * Copyright (c) 2026 ssl. All rights reserved.
 */
#include <regex>
#include <numeric>

#include <spdlog/spdlog.h>

#include "npycpp/AbsReader.h"

namespace npy {
int64_t SeekAndLocalInfo::ComputeOffset() const {
	return std::accumulate(
		       shape.begin(),
		       shape.end(), 1,
		       std::multiplies<int64_t>()
	       ) * word_size;
}

AbsReader::AbsReader(const std::string &npz_path) : file_(
	npz_path, std::ios::binary | std::ios::in) {
	if (!file_)
		throw std::runtime_error(fmt::format("Cannot open file: {}", npz_path));
}

const char *AbsReader::ParseNpyHeader(
	size_t &word_size,
	std::vector<int64_t> &shape,
	bool &fortran_order
) {
	// 读取前11个字节
	char buffer[256];
	file_.read(buffer, 11);
	if (file_.gcount() != 11)
		return "ParseNpyHeader: failed to read initial bytes";

	// 读取剩余的 header 行
	std::string header_line;
	std::getline(file_, header_line);

	// 确保以换行符结尾
	if (header_line.empty() || header_line.back() != '\n')
		header_line += '\n';

	std::string header = header_line;

	// 解析 fortran_order
	size_t loc1 = header.find("fortran_order");
	if (loc1 == std::string::npos)
		return "ParseNpyHeader: failed to find header keyword: 'fortran_order'";

	loc1 += 16;
	fortran_order = (header.substr(loc1, 4) == "True");

	// 解析 shape
	loc1 = header.find("(");
	size_t loc2 = header.find(")");
	if (loc1 == std::string::npos || loc2 == std::string::npos)
		return "ParseNpyHeader: failed to find header keyword: '(' or ')'";

	std::regex num_regex("[0-9][0-9]*");
	std::smatch sm;
	shape.clear();

	std::string str_shape = header.substr(loc1 + 1, loc2 - loc1 - 1);
	while (std::regex_search(str_shape, sm, num_regex)) {
		shape.push_back(std::stoll(sm[0].str()));
		str_shape = sm.suffix().str();
	}

	// 解析字节序、字大小和数据类型
	loc1 = header.find("descr");
	if (loc1 == std::string::npos)
		return "ParseNpyHeader: failed to find header keyword: 'descr'";

	loc1 += 9;
	const bool little_endian = (header[loc1] == '<' || header[loc1] == '|' ? true : false);
	if (!little_endian)
		return "ParseNpyHeader: only little endian is supported";


	std::string str_ws = header.substr(loc1 + 2);
	loc2 = str_ws.find("'");
	word_size = std::stoull(str_ws.substr(0, loc2));

	return nullptr;
}
}
