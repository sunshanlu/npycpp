#pragma once

#include <string>
#include <complex>
#include <fstream>
#include <filesystem>
#include <numeric>
#include <zlib.h>

#include <spdlog/spdlog.h>

namespace npy {
class NpzWriter {
public:
	enum class Mode { W, A };

	/**
	 * 根据文件和模式创建一个 NpzWriter 对象，不同模式会有不同的初始化方式
	 *
	 * @param npz_path	输入的 npz 文件路径
	 * @param mode			输入的写入模式（创建模式和追加模式）
	 */
	explicit NpzWriter(const std::string &npz_path, Mode mode = Mode::W);

	/**
	 * 添加一个 npy 文件到 npz 文件中
	 *
	 * 1. 创建 npy_header @see CreateNpyHeader
	 * 2. crc32数据密钥
	 * 3. 创建local_header @see BuildLocalHeader
	 * 4. 添加信息到全局头部 @see AddInfoToGlobalHeader
	 * 5. 写入 local_header npy_header data
	 *
	 * @tparam T		待写入的模板数据类型
	 * @param name	写入的npy名称
	 * @param data	写入的数据指针
	 * @param shape	写入的数据形状
	 * @return			返回错误信息，无错误返回 nullptr
	 */
	template<typename T>
	const char *AddNpyData(const std::string &name, const T *data, const std::vector<size_t> &shape) {
		const std::string fname = name + ".npy";

		const std::string npy_header = CreateNpyHeader<T>(shape);
		const std::size_t nelements = std::accumulate(shape.begin(), shape.end(), 1
		                                              , std::multiplies<size_t>());
		const std::size_t nbytes = nelements * sizeof(T) + npy_header.size();

		uint32_t crc = crc32(0L, reinterpret_cast<const uint8_t *>(npy_header.data())
		                     , npy_header.size());
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

	~NpzWriter() {
		if (!is_closed_)
			Close();
	}

	const char *Close();

private:
	std::string BuildLocalHeader(const std::string &fname, const uint32_t &crc, const int &nbytes);

	void AddInfoToGlobalHeader(const std::string &fname, const std::string &local_header);

	std::string BuildFooter();

	static char BigEndianTest();

	template<typename T>
	static std::string CastToString(const T &rhs) {
		std::string res;
		res.reserve(sizeof(T)); // 预分配空间以提高性能

		const unsigned char *bytes = reinterpret_cast<const unsigned char *>(&rhs);
		for (std::size_t byte = 0; byte < sizeof(T); byte++) {
			res += static_cast<char>(bytes[byte]);
		}

		return res;
	}

	static char MapType(const std::type_info &t);

	template<typename T>
	std::string CreateNpyHeader(const std::vector<size_t> &shape) {
		std::string dict;

		dict += "{'descr': '";
		dict += BigEndianTest();
		dict += MapType(typeid(T));
		dict += std::to_string(sizeof(T));
		dict += "', 'fortran_order': False, 'shape': (";
		dict += std::to_string(shape[0]);

		for (size_t i = 1; i < shape.size(); i++) {
			dict += ", ";
			dict += std::to_string(shape[i]);
		}

		if (shape.size() == 1) dict += ",";
		dict += "), }";
		const int remainder = 16 - (10 + dict.size()) % 16;
		dict.insert(dict.end(), remainder, ' ');
		dict.back() = '\n';

		std::string header;
		header += static_cast<char>(0x93);
		header += "NUMPY";
		header += static_cast<char>(0x01);
		header += static_cast<char>(0x00);
		header += CastToString(static_cast<uint16_t>(dict.size()));
		header.insert(header.end(), dict.begin(), dict.end());

		return header;
	}

	const char *SparseNpzFooter();

	std::fstream file_;
	std::string global_header_;
	uint32_t global_header_offset_ = 0;
	uint16_t records_nums_ = 0;
	bool is_closed_ = false;
	bool path_invalid = true;
};
}
