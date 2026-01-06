/**
 * Created by ssl on 2026/1/1.
 * Copyright (c) 2026 ssl. All rights reserved.
 */
#include <filesystem>

#include <spdlog/spdlog.h>

#include "npycpp/npycpp.hpp"

void ReadNpzExamples() {
	namespace fs = std::filesystem;
	SPDLOG_INFO("=== 读取 Python 保存的 NPZ 文件 ===");

	try {
		npy::NpzReader reader(fs::path(EXAMPLES_DIR) / "res" / "python_saved.npz");

		// 读取 float_array
		SPDLOG_INFO("读取 float_array:");
		auto float_data = reader["float_array"];
		if (float_data.Elements() > 0) {
			// 使用 Shape 函数
			auto shape = float_data.Shape();
			SPDLOG_INFO("  Shape: [{}]", fmt::join(shape, ", "));
			SPDLOG_INFO("  Elements: {}", float_data.Elements());
			SPDLOG_INFO("  NumBytes: {}", float_data.NumBytes());

			// 使用类型转换函数
			cv::Mat cv_mat = float_data.CVMat<float>();
			SPDLOG_INFO("  CV Mat shape: {}x{}", cv_mat.rows, cv_mat.cols);

			Eigen::MatrixXf eigen_mat = float_data.EigenMatrix<float>();
			SPDLOG_INFO("  Eigen Matrix shape: {}x{}", eigen_mat.rows(), eigen_mat.cols());

			const float *float_ptr = float_data.Ptr<float>();
			SPDLOG_INFO("  Data: ");
			for (size_t i = 0; i < float_data.Elements(); ++i)
				SPDLOG_INFO("    - {}", float_ptr[i]);
		}

		// 读取 int_array
		SPDLOG_INFO("读取 int_array:");
		auto int_data = reader.Load("int_array");
		if (int_data.Elements() > 0) {
			// 使用 Shape 函数
			auto shape = int_data.Shape();
			SPDLOG_INFO("  Shape: [{}]", fmt::join(shape, ", "));
			SPDLOG_INFO("  Elements: {}", int_data.Elements());
			SPDLOG_INFO("  NumBytes: {}", int_data.NumBytes());

			// 使用零拷贝类型转换函数
			auto eigen_map = int_data.EigenMatrixZC<int>();
			SPDLOG_INFO("  Eigen Map shape: {}x{}", eigen_map.rows(), eigen_map.cols());

			const int *int_ptr = int_data.Ptr<int>();
			SPDLOG_INFO("  Data: ");
			for (size_t i = 0; i < int_data.Elements(); ++i)
				SPDLOG_INFO("    - {}", int_ptr[i]);
		}

		// 读取 double_array
		SPDLOG_INFO("读取 double_array:");
		auto double_data = reader["double_array"];
		if (double_data.Elements() > 0) {
			// 使用 Shape 函数
			auto shape = double_data.Shape();
			SPDLOG_INFO("  Shape: [{}]", fmt::join(shape, ", "));
			SPDLOG_INFO("  Elements: {}", double_data.Elements());
			SPDLOG_INFO("  NumBytes: {}", double_data.NumBytes());

			// 使用零拷贝类型转换函数
			cv::Mat cv_mat_zc = double_data.CVMatZC<double>();
			SPDLOG_INFO("  CV Mat ZC shape: {}x{}", cv_mat_zc.rows, cv_mat_zc.cols);

			const double *double_ptr = double_data.Ptr<double>();
			SPDLOG_INFO("  Data: ");
			for (size_t i = 0; i < double_data.Elements(); ++i)
				SPDLOG_INFO("    - {}", double_ptr[i]);
		}
	} catch (const std::exception &e) {
		SPDLOG_ERROR("读取文件时出错: {}", e.what());
	}
}


void WriteNpzExamples() {
	namespace fs = std::filesystem;
	SPDLOG_INFO("=== C++ 保存 NPZ 文件供 Python 读取 ===");

	// 创建测试数据
	const std::vector float_data = {1.0f, 2.0f, 3.0f, 4.0f};
	const std::vector int_data = {10, 20, 30, 40, 50};
	const std::vector double_data = {1.1, 2.2, 3.3, 4.4};

	const std::vector<int64_t> shape2d = {2, 2};
	const std::vector<int64_t> shape1d_int = {5};
	const std::vector<int64_t> shape1d_double = {4};
	const std::string npz_path = fs::path(EXAMPLES_DIR) / "res" / "cpp_saved.npz";

	try {
		// 首先使用写入模式创建文件
		{
			npy::NpzWriter writer(npz_path, npy::WriteMode::W);
			writer.AddNpyData("cpp_float_array", float_data.data(), shape2d);
			writer.AddNpyData("cpp_int_array", int_data.data(), shape1d_int);

			SPDLOG_INFO("C++ 已保存初始 NPZ 文件: {}", npz_path);
		}

		// 然后使用追加模式添加更多数据
		{
			npy::NpzWriter writer(npz_path, npy::WriteMode::A);
			writer.AddNpyData("cpp_double_array", double_data.data(), shape1d_double);

			SPDLOG_INFO("C++ 已追加数据到 NPZ 文件: {}", npz_path);
		}

		// 验证保存的数据 - 使用 NpData 进行验证
		SPDLOG_INFO("验证 C++ 保存的数据:");

		// 创建 NpData 对象并测试 Shape 和类型转换
		npy::NpData float_npd({2, 2}, sizeof(float), false);
		float *float_ptr = float_npd.Ptr<float>();
		std::ranges::copy(float_data, float_ptr);

		auto float_shape = float_npd.Shape();
		SPDLOG_INFO("float_data shape: [{}]", fmt::join(float_shape, ", "));

		// 测试类型转换
		cv::Mat float_cv_mat = float_npd.CVMat<float>();
		SPDLOG_INFO("float_data CV Mat shape: {}x{}", float_cv_mat.rows, float_cv_mat.cols);

		auto float_eigen_mat = float_npd.EigenMatrix<float>();
		SPDLOG_INFO("float_data Eigen Matrix shape: {}x{}", float_eigen_mat.rows()
		            , float_eigen_mat.cols());

		SPDLOG_INFO("float_data: ");
		for (const auto &val: float_data)
			SPDLOG_INFO("  - {}", val);

		npy::NpData int_npd({5}, sizeof(int), false);
		int *int_ptr = int_npd.Ptr<int>();
		std::ranges::copy(int_data, int_ptr);

		auto int_shape = int_npd.Shape();
		SPDLOG_INFO("int_data shape: [{}]", fmt::join(int_shape, ", "));

		SPDLOG_INFO("int_data: ");
		for (const auto &val: int_data)
			SPDLOG_INFO("  - {}", val);

		npy::NpData double_npd({4}, sizeof(double), false);
		double *double_ptr = double_npd.Ptr<double>();
		std::ranges::copy(double_data, double_ptr);

		auto double_shape = double_npd.Shape();
		SPDLOG_INFO("double_data shape: [{}]", fmt::join(double_shape, ", "));

		SPDLOG_INFO("double_data: ");
		for (const auto &val: double_data)
			SPDLOG_INFO("  - {}", val);
	} catch (const std::exception &e) {
		SPDLOG_ERROR("保存文件时出错: {}", e.what());
	}
}

void TypeConversionExamples() {
	SPDLOG_INFO("=== 类型转换函数使用示例 ===");

	// 创建测试数据
	std::vector<float> test_data = {1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f};
	std::vector<int64_t> shape = {2, 3};

	// 创建 NpData 对象
	npy::NpData npd(shape, sizeof(float), false);
	float *data_ptr = npd.Ptr<float>();
	std::ranges::copy(test_data, data_ptr);

	SPDLOG_INFO("原始数据: [{}]", fmt::join(test_data, ", "));

	// 使用 Shape 函数
	auto shape_info = npd.Shape();
	SPDLOG_INFO("数据形状: [{}]", fmt::join(shape_info, ", "));

	// 使用 CVMat 转换
	cv::Mat cv_mat = npd.CVMat<float>();
	SPDLOG_INFO("CV Mat 转换成功 - shape: {}x{}", cv_mat.rows, cv_mat.cols);

	// 使用零拷贝 CVMat 转换
	cv::Mat cv_mat_zc = npd.CVMatZC<float>();
	SPDLOG_INFO("CV Mat 零拷贝转换成功 - shape: {}x{}", cv_mat_zc.rows, cv_mat_zc.cols);

	// 使用 EigenMatrix 转换
	auto eigen_mat = npd.EigenMatrix<float>();
	SPDLOG_INFO("Eigen Matrix 转换成功 - shape: {}x{}", eigen_mat.rows(), eigen_mat.cols());

	// 使用零拷贝 EigenMatrix 转换
	auto eigen_map = npd.EigenMatrixZC<float>();
	SPDLOG_INFO("Eigen Map 零拷贝转换成功 - shape: {}x{}", eigen_map.rows(), eigen_map.cols());

	// 验证零拷贝转换的数据一致性
	eigen_map(0, 0) = 999.0f;                     // 修改映射中的值
	SPDLOG_INFO("修改映射后原始数据[0]: {}", data_ptr[0]); // 应该等于 999.0f
}


int main() {
	namespace fs = std::filesystem;

	// 1. 先写入 C++ 数据供 Python 读取
	WriteNpzExamples();

	// 2. 使用 python 调用文件进行Python文件的读取
	const std::string res_path = fs::path(EXAMPLES_DIR) / "res";
	const std::string npz_path = fs::path(EXAMPLES_DIR) / "res" / "cpp_saved.npz";
	const std::string scripts_path = fs::path(EXAMPLES_DIR) / "scripts" / "NpzpyExamples.py";
	const std::string cmd = fmt::format(
		"python3 {} --npz_path {} --res_path {}",
		scripts_path, npz_path, res_path
	);

	int python_result = system(cmd.c_str());
	if (python_result != 0)
		SPDLOG_WARN("Python 脚本执行可能存在问题，返回码: {}", python_result);

	// 3. 读取 Python 保存的数据
	ReadNpzExamples();

	// 4. 演示类型转换函数使用
	TypeConversionExamples();

	return 0;
}
