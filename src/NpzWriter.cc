#include "npycpp/NpzWriter.hpp"

namespace npy {
NpzWriter::NpzWriter(const std::string &npz_path, Mode mode) {
	if (npz_path.empty()) {
		SPDLOG_WARN("npz_path is empty");
		path_invalid = false;
		return;
	}

	namespace fs = std::filesystem;

	try {
		const fs::path npz_path_abs = fs::absolute(npz_path);
		const fs::path dir_path = npz_path_abs.parent_path();
		if (!fs::exists(dir_path)) {
			fs::create_directories(dir_path);

			if (mode == Mode::A) {
				SPDLOG_WARN("there is no npz_path, mode is changed to W");
				mode = Mode::W;
			}
		}

		switch (mode) {
			case Mode::W:
				file_.open(npz_path_abs, std::ios::out | std::ios::binary);
				break;
			case Mode::A:
				file_.open(npz_path_abs, std::ios::out | std::ios::in | std::ios::binary);
				SparseNpzFooter();
				break;
		}
	} catch (const fs::filesystem_error &) {
		path_invalid = false;
		SPDLOG_WARN("input file is invalid, please check it");
		return;
	}
}

const char *NpzWriter::Close() {
	if (!path_invalid) {
		is_closed_ = true;
		return "input npz path is invalid";
	}

	const std::string footer = BuildFooter();

	file_.write(global_header_.data(), global_header_.size());
	file_.write(footer.data(), footer.size());
	file_.close();
	is_closed_ = true;
	return nullptr;
}

std::string NpzWriter::BuildLocalHeader(const std::string &fname, const uint32_t &crc, const int &nbytes) {
	std::string local_header;
	local_header += "PK";                                              //< first part of sig
	local_header += CastToString(static_cast<uint16_t>(0x0403));       //< second part of sig
	local_header += CastToString(static_cast<uint16_t>(20));           //< version needed to extract 压缩版本
	local_header += CastToString(static_cast<uint16_t>(0));            //< general purpose bit flag
	local_header += CastToString(static_cast<uint16_t>(0));            //< compress method 压缩方法
	local_header += CastToString(static_cast<uint16_t>(0));            //< file last mod time 文件最后修改时间
	local_header += CastToString(static_cast<uint16_t>(0));            //< file last mod date 文件最后修改日期
	local_header += CastToString(crc);                                 //< crc32数据密钥
	local_header += CastToString(static_cast<uint32_t>(nbytes));       //< compressed size 压缩大小
	local_header += CastToString(static_cast<uint32_t>(nbytes));       //< uncompressed size 解压缩大小
	local_header += CastToString(static_cast<uint16_t>(fname.size())); //< fname的名字长度
	local_header += CastToString(static_cast<uint16_t>(0));            //< extra field 额外的保存内容
	local_header += fname;

	return local_header;
}

void NpzWriter::AddInfoToGlobalHeader(const std::string &fname, const std::string &local_header) {
	global_header_ += "PK";
	global_header_ += CastToString(static_cast<uint16_t>(0x0201));
	global_header_ += CastToString(static_cast<uint16_t>(20));
	global_header_.insert(global_header_.end(), local_header.begin() + 4, local_header.begin() + 30);
	global_header_ += CastToString(static_cast<uint16_t>(0));
	global_header_ += CastToString(static_cast<uint16_t>(0));
	global_header_ += CastToString(static_cast<uint16_t>(0));
	global_header_ += CastToString(static_cast<uint32_t>(0));
	global_header_ += CastToString(static_cast<uint32_t>(global_header_offset_));
	global_header_ += fname;
}

std::string NpzWriter::BuildFooter() {
	std::string res;

	res += "PK";
	res += CastToString(static_cast<uint16_t>(0x0605));
	res += CastToString(static_cast<uint16_t>(0));
	res += CastToString(static_cast<uint16_t>(0));
	res += CastToString(static_cast<uint16_t>(records_nums_));
	res += CastToString(static_cast<uint16_t>(records_nums_));
	res += CastToString(static_cast<uint32_t>(global_header_.size()));
	res += CastToString(static_cast<uint32_t>(global_header_offset_));
	res += CastToString(static_cast<uint16_t>(0));

	return res;
}

char NpzWriter::BigEndianTest() {
	int x = 1;
	return (reinterpret_cast<char *>(&x)[0]) ? '<' : '>';
}

char NpzWriter::MapType(const std::type_info &t) {
	if (t == typeid(float)) return 'f';
	if (t == typeid(double)) return 'f';
	if (t == typeid(long double)) return 'f';

	if (t == typeid(int)) return 'i';
	if (t == typeid(char)) return 'i';
	if (t == typeid(short)) return 'i';
	if (t == typeid(long)) return 'i';
	if (t == typeid(long long)) return 'i';

	if (t == typeid(unsigned char)) return 'u';
	if (t == typeid(unsigned short)) return 'u';
	if (t == typeid(unsigned long)) return 'u';
	if (t == typeid(unsigned long long)) return 'u';
	if (t == typeid(unsigned int)) return 'u';

	if (t == typeid(bool)) return 'b';

	if (t == typeid(std::complex<float>)) return 'c';
	if (t == typeid(std::complex<double>)) return 'c';
	if (t == typeid(std::complex<long double>)) return 'c';

	else return '?';
}

const char *NpzWriter::SparseNpzFooter() {
	// 检查文件是否超过22个字节
	file_.seekg(0, std::ios::end);
	if (file_.tellg() < 22)
		return "File size is less than 22 bytes";

	std::string footer(22, '0');
	file_.seekg(-22, std::ios::end);
	file_.read(footer.data(), 22);

	const auto global_header_size = *reinterpret_cast<uint32_t *>(&footer[12]);
	records_nums_ = *reinterpret_cast<uint16_t *>(&footer[10]);
	global_header_offset_ = *reinterpret_cast<uint32_t *>(&footer[16]);

	assert(
		*reinterpret_cast<uint16_t *>(&footer[4]) == 0 &&
		*reinterpret_cast<uint16_t *>(&footer[6]) == 0 &&
		*reinterpret_cast<uint16_t *>(&footer[8]) == records_nums_ &&
		*reinterpret_cast<uint16_t *>(&footer[20]) == 0 &&
		"footer sparse got same error!"
	);

	global_header_.resize(global_header_size);
	file_.seekg(global_header_offset_);
	file_.read(global_header_.data(), global_header_size);
	file_.seekg(global_header_offset_);
	return nullptr;
}
} // npy
