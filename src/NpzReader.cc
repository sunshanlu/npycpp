/**
 * Created by ssl on 2025/12/30.
 * Copyright (c) 2025 ssl. All rights reserved.
 */
#include <cstdint>
#include <numeric>

#include "npycpp/NpzReader.hpp"

namespace npy {
NpzReader::NpzReader(const std::string &npz_path)
	: AbsReader(npz_path) {
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

	return LoadInternal(offsets_[varname]);
}
} // npy
