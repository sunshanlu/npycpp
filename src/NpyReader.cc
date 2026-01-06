/**
 * Created by ssl on 2026/1/4.
 * Copyright (c) 2026 ssl. All rights reserved.
 */
#include <spdlog/spdlog.h>

#include "npycpp/NpyReader.hpp"

namespace npy {
NpyReader::NpyReader(const std::string &file_path) : AbsReader(file_path) {
	SeekAndLocalInfo sinfo;
	ParseNpyHeader(sinfo.word_size, sinfo.shape, sinfo.fortran_order);
	sinfo.seek_offset = file_.tellg();
	offsets_.insert({"", sinfo});
}

NpData NpyReader::Load() {
	if (offsets_.empty()) {
		SPDLOG_WARN("Error: file is empty");
		return NpData();
	}

	return LoadInternal(offsets_.begin()->second);
}
} // npy
