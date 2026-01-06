/**
 * Created by ssl on 2026/1/4.
 * Copyright (c) 2026 ssl. All rights reserved.
 */
#include "npycpp/NpyWriter.hpp"

namespace npy {
NpyWriter::NpyWriter(const std::string &npz_path) : AbsWriter(npz_path, WriteMode::W) {
	if (path_invalid_)
		throw std::runtime_error("npz_path is invalid");
}

const char * NpyWriter::Close() { return nullptr; }
}
