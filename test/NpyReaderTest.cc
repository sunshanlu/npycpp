/**
 * Created by ssl on 2026/1/5.
 * Copyright (c) 2026 ssl. All rights reserved.
 */

#include <gtest/gtest.h>
#include <fstream>
#include <vector>
#include <filesystem>
#include <numeric>

#include "npycpp/NpyReader.hpp"
#include "npycpp/NpyWriter.hpp"

namespace npy {
class NpyReaderTest : public ::testing::Test {
protected:
	void SetUp() override {
		spdlog::set_level(spdlog::level::err);
		test_file_path = "test_input.npy";
		// 确保测试文件不存在
		if (std::filesystem::exists(test_file_path)) {
			std::filesystem::remove(test_file_path);
		}
	}

	void TearDown() override {
		// 清理测试文件
		if (std::filesystem::exists(test_file_path)) {
			std::filesystem::remove(test_file_path);
		}
	}

	std::string test_file_path;
};

// 测试构造函数
TEST_F(NpyReaderTest, Constructor) {
	// 先创建一个有效的npy文件
	{
		std::vector<float> data = {1.0f, 2.0f, 3.0f};
		std::vector<int64_t> shape = {3};
		NpyWriter writer(test_file_path);
		writer.SaveNpyData(data.data(), shape);
	}

	EXPECT_NO_THROW({NpyReader reader(test_file_path);});
}

// 测试构造函数异常处理
TEST_F(NpyReaderTest, ConstructorInvalidPath) {
	const std::string invalid_path = "nonexistent_file.npy";
	EXPECT_THROW(NpyReader reader(invalid_path), std::runtime_error);
}

// 测试读取数据
TEST_F(NpyReaderTest, LoadData) {
	// 创建测试文件
	const std::vector<float> original_data = {1.0f, 2.0f, 3.0f, 4.0f};

	// clang-format off
	{
		const std::vector<int64_t> shape = {2, 2};
		NpyWriter writer(test_file_path);
		writer.SaveNpyData(original_data.data(), shape);
	}
	// clang-format on

	// 读取并验证数据
	NpyReader reader(test_file_path);
	NpData read_data = reader.Load();

	EXPECT_EQ(read_data.Elements(), 4);
	EXPECT_EQ(read_data.NumBytes(), 4 * sizeof(float));

	const float *read_ptr = read_data.Ptr<float>();
	for (size_t i = 0; i < original_data.size(); ++i)
		EXPECT_FLOAT_EQ(read_ptr[i], original_data[i]);
}

// 测试读取不同数据类型
TEST_F(NpyReaderTest, LoadDifferentDataTypes) {
	// 测试整数类型
	{
		std::vector<int> int_data = {1, 2, 3, 4, 5};
		std::vector<int64_t> shape = {5};
		NpyWriter writer("test_int.npy");
		writer.SaveNpyData(int_data.data(), shape);
	}

	// clang-format off
	{
		NpyReader reader("test_int.npy");
		NpData read_data = reader.Load();
		EXPECT_EQ(read_data.Elements(), 5);
		EXPECT_EQ(read_data.NumBytes(), 5 * sizeof(int));

		const int *read_ptr = read_data.Ptr<int>();
		std::vector<int> expected = {1, 2, 3, 4, 5};
		for (size_t i = 0; i < expected.size(); ++i)
			EXPECT_EQ(read_ptr[i], expected[i]);
		// clang-format on
	}

	// 测试双精度类型
	{
		std::vector<double> double_data = {1.1, 2.2, 3.3};
		std::vector<int64_t> shape = {3};
		NpyWriter writer("test_double.npy");
		writer.SaveNpyData(double_data.data(), shape);
	}

	// clang-format off
	{
		NpyReader reader("test_double.npy");
		NpData read_data = reader.Load();
		EXPECT_EQ(read_data.Elements(), 3);
		EXPECT_EQ(read_data.NumBytes(), 3 * sizeof(double));

		const double *read_ptr = read_data.Ptr<double>();
		std::vector<double> expected = {1.1, 2.2, 3.3};
		for (size_t i = 0; i < expected.size(); ++i)
			EXPECT_DOUBLE_EQ(read_ptr[i], expected[i]);
	}
	// clang-format on
}

// 测试读取多维数组
TEST_F(NpyReaderTest, LoadMultiDimensionalArray) {
	const std::vector<int> original_data = {1, 2, 3, 4, 5, 6, 7, 8};

	// clang-format off
	{
		const std::vector<int64_t> shape = {2, 2, 2};
		NpyWriter writer(test_file_path);
		writer.SaveNpyData(original_data.data(), shape);
	}
	// clang-format on

	NpyReader reader(test_file_path);
	NpData read_data = reader.Load();

	EXPECT_EQ(read_data.Elements(), 8);
	EXPECT_EQ(read_data.NumBytes(), 8 * sizeof(int));

	const int *read_ptr = read_data.Ptr<int>();
	for (size_t i = 0; i < original_data.size(); ++i)
		EXPECT_EQ(read_ptr[i], original_data[i]);
}

// 测试读取空数组
TEST_F(NpyReaderTest, LoadEmptyArray) {
	std::vector<float> empty_data;

	// clang-format off
	{
		const std::vector<int64_t> empty_shape = {0};
		NpyWriter writer(test_file_path);
		writer.SaveNpyData(empty_data.data(), empty_shape);
	}
	// clang-format on

	NpyReader reader(test_file_path);
	NpData read_data = reader.Load();

	EXPECT_EQ(read_data.Elements(), 0);
	EXPECT_EQ(read_data.NumBytes(), 0);
}

// 测试读取大数组
TEST_F(NpyReaderTest, LoadLargeArray) {
	std::vector<double> large_data(1000);
	std::iota(large_data.begin(), large_data.end(), 1.0);

	// clang-format off
	{
		const std::vector<int64_t> shape = {1000};
		NpyWriter writer(test_file_path);
		writer.SaveNpyData(large_data.data(), shape);
	}
	// clang-format on

	NpyReader reader(test_file_path);
	NpData read_data = reader.Load();

	EXPECT_EQ(read_data.Elements(), 1000);
	EXPECT_EQ(read_data.NumBytes(), 1000 * sizeof(double));

	const double *read_ptr = read_data.Ptr<double>();
	for (size_t i = 0; i < 10 && i < read_data.Elements(); ++i)
		EXPECT_DOUBLE_EQ(read_ptr[i], static_cast<double>(i + 1));
}

// 测试错误处理 - 空文件
TEST_F(NpyReaderTest, LoadEmptyFile) {
	// 创建一个空文件
	std::ofstream empty_file(test_file_path);
	empty_file.close();

	// 尝试读取空文件应该失败或返回空数据
	EXPECT_NO_THROW({
		NpyReader reader(test_file_path);
		NpData read_data = reader.Load();
	});
}
} // namespace npy

int main(int argc, char **argv) {
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}