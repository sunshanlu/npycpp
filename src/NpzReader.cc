/**
 * Created by ssl on 2025/12/30.
 * Copyright (c) 2025 ssl. All rights reserved.
 */
#include <cstdint>
#include <numeric>

#include "npycpp/NpzReader.hpp"

namespace npy {
NpzReader::NpzReader(const std::string &npz_path)
	: file_(npz_path, std::ios::binary | std::ios::in) {
	if (!file_.is_open())
		throw std::runtime_error("Error: Unable to open file: " + npz_path);

	std::string local_header(30, '0');
	while (file_.read(local_header.data(), 30)) {
		if (local_header[2] != 0x03 || local_header[3] != 0x04)
			break;

		const uint16_t namelen = *reinterpret_cast<uint16_t *>(&local_header[26]);
		const uint16_t extralen = *reinterpret_cast<uint16_t *>(&local_header[28]);
		std::string varname(namelen, '0');
		std::string extra_file(extralen, '0');
		file_.read(varname.data(), namelen);
		file_.read(extra_file.data(), extralen);
		varname.erase(varname.end() - 4, varname.end());

		SeekAndLocalInfo sinfo;
		ParseNpyHeader(sinfo.word_size, sinfo.shape, sinfo.fortran_order);
		sinfo.seek_offset = file_.tellg();
		offsets_.insert(std::make_pair(varname, sinfo));

		file_.seekg(sinfo.ComputeOffset(), std::ios::cur);
	}
}

NpData NpzReader::operator[](const std::string &varname) {
	return Load(varname);
}

NpData NpzReader::Load(std::string varname) {
	if (varname.size() > 4 && std::string(varname.end() - 4, varname.end()) == ".npy")
		varname = varname.substr(0, varname.size() - 4);

	if (!offsets_.contains(varname)) {
		SPDLOG_WARN("Error: Variable {} not found in npz file", varname);
		return NpData();
	}

	const auto [seek_offset, word_size, shape, fortran_order] = offsets_[varname];
	file_.seekg(seek_offset);
	NpData data(shape, word_size, fortran_order);
	file_.read(data.Ptr<char>(), data.NumBytes());
	return data;
}

const char *NpzReader::ParseNpyHeader(
	size_t &word_size,
	std::vector<int64_t> &shape,
	bool &fortran_order
) {
	// 读取前11个字节
	char buffer[256];
	file_.read(buffer, 11);
	if (file_.gcount() != 11) {
		return "ParseNpyHeader: failed to read initial bytes";
	}

	// 读取剩余的 header 行
	std::string header_line;
	std::getline(file_, header_line);

	// 确保以换行符结尾
	if (header_line.empty() || header_line.back() != '\n') {
		header_line += '\n';
	}

	std::string header = header_line;

	// 解析 fortran_order
	size_t loc1 = header.find("fortran_order");
	if (loc1 == std::string::npos) {
		return "ParseNpyHeader: failed to find header keyword: 'fortran_order'";
	}
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
} // npy
