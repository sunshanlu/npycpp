/**
 * Created by ssl on 2026/1/5.
 * Copyright (c) 2026 ssl. All rights reserved.
 */
#include <filesystem>

#include <spdlog/spdlog.h>

#include "npycpp/npycpp.hpp"

void ReadNpyExamples() {
	namespace fs = std::filesystem;
	SPDLOG_INFO("=== [读取 | Python | NPY] ===");
	// 读取Python保存的不同类型的NPY文件
	// clang-format off
	std::vector<std::string> npy_files = {
		fs::path(EXAMPLES_DIR) / "res" / "float_array.npy",
		fs::path(EXAMPLES_DIR) / "res" / "int_array.npy",
		fs::path(EXAMPLES_DIR) / "res" / "double_array.npy",
	};
	// clang-format on

	SPDLOG_INFO("读取文件: {}", npy_files[0]);
	npy::NpyReader reader_float(npy_files[0]);
	auto float_data = reader_float.Load();
	SPDLOG_INFO("  Shape: [{}]", fmt::join(float_data.Shape(), ", "));
	SPDLOG_INFO("  Elements: {}", float_data.Elements());
	SPDLOG_INFO("  NumBytes: {}", float_data.NumBytes());
	const float *float_ptr = float_data.Ptr<float>();
	SPDLOG_INFO("  Data (float): ");
	for (size_t i = 0; i < float_data.Elements(); ++i)
		SPDLOG_INFO("    - {}", float_ptr[i]);

	SPDLOG_INFO("读取文件: {}", npy_files[1]);
	npy::NpyReader reader_int(npy_files[1]);
	auto int_data = reader_int.Load();
	SPDLOG_INFO("  Shape: [{}]", fmt::join(int_data.Shape(), ", "));
	SPDLOG_INFO("  Elements: {}", int_data.Elements());
	SPDLOG_INFO("  NumBytes: {}", int_data.NumBytes());
	const int *int_ptr = int_data.Ptr<int>();
	SPDLOG_INFO("  Data (int): ");
	for (size_t i = 0; i < int_data.Elements(); ++i)
		SPDLOG_INFO("    - {}", int_ptr[i]);

	SPDLOG_INFO("读取文件: {}", npy_files[2]);
	npy::NpyReader reader_double(npy_files[2]);
	auto double_data = reader_double.Load();
	SPDLOG_INFO("  Shape: [{}]", fmt::join(double_data.Shape(), ", "));
	SPDLOG_INFO("  Elements: {}", double_data.Elements());
	SPDLOG_INFO("  NumBytes: {}", double_data.NumBytes());
	const double *double_ptr = double_data.Ptr<double>();
	SPDLOG_INFO("  Data (double): ");
	for (size_t i = 0; i < double_data.Elements(); ++i)
		SPDLOG_INFO("    - {}", double_ptr[i]);
}

void WriteNpyExamples() {
	namespace fs = std::filesystem;
	SPDLOG_INFO("=== [保存 | C++ |  NPY] ===");

	// 创建测试数据
	std::vector float_data = {1.0f, 2.0f, 3.0f, 4.0f};
	std::vector int_data = {10, 20, 30, 40, 50};
	std::vector double_data = {1.1, 2.2, 3.3, 4.4, 5.5};

	std::vector<int64_t> shape_float = {2, 2, 1};
	std::vector<int64_t> shape_int = {5, 1};
	std::vector<int64_t> shape_double = {5};

	std::string res_path = fs::path(EXAMPLES_DIR) / "res";

	// 保存 float 数组
	{
		std::string float_path = res_path + "/cpp_float_array.npy";
		npy::NpyWriter writer(float_path);
		writer.SaveNpyData(float_data.data(), shape_float);
	}

	// 保存 int 数组
	{
		std::string int_path = res_path + "/cpp_int_array.npy";
		npy::NpyWriter writer(int_path);
		writer.SaveNpyData(int_data.data(), shape_int);
	}

	// 保存 double 数组
	{
		std::string double_path = res_path + "/cpp_double_array.npy";
		npy::NpyWriter writer(double_path);
		writer.SaveNpyData(double_data.data(), shape_double);
	}

	// 验证保存的数据
	SPDLOG_INFO("验证 C++ 保存的数据:");
	SPDLOG_INFO("  float_data: [{}]", fmt::join(float_data, ", "));
	SPDLOG_INFO("  int_data: [{}]", fmt::join(int_data, ", "));
	SPDLOG_INFO("  double_data: [{}]", fmt::join(double_data, ", "));
}


int main() {
	namespace fs = std::filesystem;

	// 确保结果目录存在
	std::string res_path = fs::path(EXAMPLES_DIR) / "res";
	if (!fs::exists(res_path))
		fs::create_directories(res_path);

	// 1. 写入C++数据供Python读取
	WriteNpyExamples();

	// 2. 执行Python脚本，读取保存数据
	// clang-format off
	std::string scripts_path = fs::path(EXAMPLES_DIR) / "scripts" / "NpypyExamples.py";
	std::vector cpp_saved_files = {
		res_path + "/cpp_float_array.npy",
		res_path + "/cpp_int_array.npy",
		res_path + "/cpp_double_array.npy"
	};
	// clang-format on
	std::string npy_paths_str = fmt::format("\"{}\"", fmt::join(cpp_saved_files, "\" \""));
	std::string cmd = fmt::format(
		"python3 {} --npy_paths {} --res_path {}",
		scripts_path, npy_paths_str, res_path
	);

	int retcode = system(cmd.c_str());
	if (retcode != 0)
		SPDLOG_ERROR("执行Python脚本时出错: {}", retcode);

	// 3. 读取Python保存的数据
	ReadNpyExamples();

	return 0;
}
