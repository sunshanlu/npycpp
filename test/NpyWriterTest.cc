/**
 * Created by ssl on 2026/1/5.
 * Copyright (c) 2026 ssl. All rights reserved.
 */

#include <gtest/gtest.h>
#include <fstream>
#include <vector>
#include <filesystem>
#include <numeric>

#include "npycpp/NpyWriter.hpp"
#include "npycpp/NpyReader.hpp"

namespace npy {
class NpyWriterTest : public ::testing::Test {
protected:
	void SetUp() override {
		spdlog::set_level(spdlog::level::err);
		test_file_path = "test_output.npy";
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
TEST_F(NpyWriterTest, Constructor) {
	EXPECT_NO_THROW({
		NpyWriter writer(test_file_path);
		});

	// 测试构造函数创建目录（如果不存在）
	std::string nested_path = "test_dir/nested/test_output.npy";
	EXPECT_NO_THROW({
		NpyWriter writer(nested_path);
		});

	// 清理测试目录
	if (std::filesystem::exists("test_dir"))
		std::filesystem::remove_all("test_dir");
}

// 测试构造函数异常处理
TEST_F(NpyWriterTest, ConstructorInvalidPath) {
	const std::string invalid_path = "/invalid_path/test.npy";
	EXPECT_THROW(NpyWriter writer(invalid_path), std::runtime_error);
}

// 测试写入单个数组
TEST_F(NpyWriterTest, WriteSingleArray) {
	const std::vector<float> data = {1.0f, 2.0f, 3.0f, 4.0f};
	// clang-format off
	{
		const std::vector<int64_t> shape = {2, 2};
		NpyWriter writer(test_file_path);
		const char *result = writer.SaveNpyData(data.data(), shape);
		EXPECT_EQ(result, nullptr); // 无错误
	}
	// clang-format on

	// 验证文件已创建
	EXPECT_TRUE(std::filesystem::exists(test_file_path));

	// 尝试读取验证数据
	EXPECT_NO_THROW({
		NpyReader reader(test_file_path);
		NpData read_data = reader.Load();
		EXPECT_EQ(read_data.Elements(), 4);

		const float *read_ptr = read_data.Ptr<float>();
		for (ssize_t i = 0; i < read_data.Elements(); ++i)
		EXPECT_FLOAT_EQ(read_ptr[i], data[i]);
		});
}

// 测试写入多种数据类型
TEST_F(NpyWriterTest, WriteMultipleDataTypes) {
	// 测试整数类型
	{
		std::vector<int> int_data = {1, 2, 3, 4, 5};
		std::vector<int64_t> int_shape = {5};
		NpyWriter writer("test_int.npy");
		writer.SaveNpyData(int_data.data(), int_shape);
	} {
		NpyReader reader("test_int.npy");
		NpData read_data = reader.Load();
		EXPECT_EQ(read_data.Elements(), 5);
		EXPECT_EQ(read_data.NumBytes(), 5 * sizeof(int));
	}

	// 测试双精度类型
	{
		std::vector<double> double_data = {1.1, 2.2, 3.3};
		std::vector<int64_t> double_shape = {3};
		NpyWriter writer("test_double.npy");
		writer.SaveNpyData(double_data.data(), double_shape);
	} {
		NpyReader reader("test_double.npy");
		NpData read_data = reader.Load();
		EXPECT_EQ(read_data.Elements(), 3);
		EXPECT_EQ(read_data.NumBytes(), 3 * sizeof(double));
	}

	// 测试字符类型
	{
		std::vector<char> char_data = {'a', 'b', 'c'};
		std::vector<int64_t> char_shape = {3};
		NpyWriter writer("test_char.npy");
		writer.SaveNpyData(char_data.data(), char_shape);
	} {
		NpyReader reader("test_char.npy");
		NpData read_data = reader.Load();
		EXPECT_EQ(read_data.Elements(), 3);
		EXPECT_EQ(read_data.NumBytes(), 3 * sizeof(char));
	}
}

// 测试写入多维数组
TEST_F(NpyWriterTest, WriteMultiDimensionalArray) {
	const std::vector<int> data = {1, 2, 3, 4, 5, 6, 7, 8};

	// clang-format off
	{
		const std::vector<int64_t> shape = {2, 2, 2};
		NpyWriter writer(test_file_path);
		writer.SaveNpyData(data.data(), shape);
	}
	// clang-format on

	NpyReader reader(test_file_path);
	const NpData read_data = reader.Load();
	EXPECT_EQ(read_data.Elements(), 8);
	EXPECT_EQ(read_data.NumBytes(), 8 * sizeof(int));
}

// 测试空数组
TEST_F(NpyWriterTest, WriteEmptyArray) {
	std::vector<float> empty_data;

	// clang-format off
	{
		const std::vector<int64_t> empty_shape = {0};
		NpyWriter writer(test_file_path);
		writer.SaveNpyData(empty_data.data(), empty_shape);
	}
	// clang-format on

	NpyReader reader(test_file_path);
	const NpData read_data = reader.Load();
	EXPECT_EQ(read_data.Elements(), 0);
	EXPECT_EQ(read_data.NumBytes(), 0);
}

// 测试 Close 方法
TEST_F(NpyWriterTest, CloseMethod) { {
		NpyWriter writer(test_file_path);
		const std::vector<int> data = {1, 2, 3};
		const std::vector<int64_t> shape = {3};
		writer.SaveNpyData(data.data(), shape);
		const char *result = writer.Close();
		EXPECT_EQ(result, nullptr);
	}

	// 验证文件已正确创建
	EXPECT_TRUE(std::filesystem::exists(test_file_path));

	NpyReader reader(test_file_path);
	const NpData read_data = reader.Load();
	EXPECT_EQ(read_data.Elements(), 3);
}
} // namespace npy


int main(int argc, char **argv) {
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
