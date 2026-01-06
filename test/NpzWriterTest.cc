/**
 * Created by ssl on 2026/1/1.
 * Copyright (c) 2026 ssl. All rights reserved.
 */

#include <gtest/gtest.h>
#include <fstream>
#include <vector>
#include <filesystem>
#include <numeric>
#include <memory>

#include "npycpp/NpzWriter.hpp"
#include "npycpp/NpzReader.hpp"

namespace npy {
class NpzWriterTest : public ::testing::Test {
protected:
	void SetUp() override {
		spdlog::set_level(spdlog::level::err);
		test_file_path = "test_output.npz";
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
TEST_F(NpzWriterTest, Constructor) {
	EXPECT_NO_THROW({NpzWriter writer(test_file_path, WriteMode::W);});

	// 测试构造函数创建目录（如果不存在）
	std::string nested_path = "test_dir/nested/test_output.npz";
	EXPECT_NO_THROW({NpzWriter writer(nested_path, WriteMode::W);});

	// 清理测试目录
	if (std::filesystem::exists("test_dir"))
		std::filesystem::remove_all("test_dir");
}

// 测试写入单个数组
TEST_F(NpzWriterTest, WriteSingleArray) {
	const std::vector<float> data = {1.0f, 2.0f, 3.0f, 4.0f};

	// clang-format off
	{
		const std::vector<int64_t> shape = {2, 2};
		NpzWriter writer(test_file_path, WriteMode::W);

		const char *result = writer.AddNpyData("test_array", data.data(), shape);
		EXPECT_EQ(result, nullptr); // 无错误

		writer.Close();
	}
	// clang-format on

	// 验证文件已创建
	EXPECT_TRUE(std::filesystem::exists(test_file_path));

	// 尝试读取验证数据
	EXPECT_NO_THROW({
		NpzReader reader(test_file_path);
		NpData read_data = reader.Load("test_array");
		EXPECT_EQ(read_data.Elements(), 4);

		const float *read_ptr = read_data.Ptr<float>();

		// clang-format off
		for (ssize_t i = 0; i < read_data.Elements(); ++i)
			EXPECT_FLOAT_EQ(read_ptr[i], data[i]);
		// clang-format on
		});
}

// 测试写入多种数据类型
TEST_F(NpzWriterTest, WriteMultipleDataTypes) { {
		NpzWriter writer(test_file_path, WriteMode::W);

		// 写入整数数组
		std::vector<int> int_data = {1, 2, 3, 4, 5};
		std::vector<int64_t> int_shape = {5};
		writer.AddNpyData<int>("int_array", int_data.data(), int_shape);

		// 写入双精度数组
		std::vector<double> double_data = {1.1, 2.2, 3.3};
		std::vector<int64_t> double_shape = {3};
		writer.AddNpyData<double>("double_array", double_data.data(), double_shape);

		// 写入字符数组
		std::vector<char> char_data = {'a', 'b', 'c'};
		std::vector<int64_t> char_shape = {3};
		writer.AddNpyData<char>("char_array", char_data.data(), char_shape);

		writer.Close();
	}

	EXPECT_TRUE(std::filesystem::exists(test_file_path));

	// 验证读取各种类型的数据
	NpzReader reader(test_file_path);

	NpData int_data = reader.Load("int_array");
	EXPECT_EQ(int_data.Elements(), 5);
	EXPECT_EQ(int_data.NumBytes(), 5 * sizeof(int));

	NpData double_data = reader.Load("double_array");
	EXPECT_EQ(double_data.Elements(), 3);
	EXPECT_EQ(double_data.NumBytes(), 3 * sizeof(double));

	NpData char_data = reader.Load("char_array");
	EXPECT_EQ(char_data.Elements(), 3);
	EXPECT_EQ(char_data.NumBytes(), 3 * sizeof(char));
}

// 测试写入多维数组
TEST_F(NpzWriterTest, WriteMultiDimensionalArray) { {
		NpzWriter writer(test_file_path, WriteMode::W);
		const std::vector<int> data = {1, 2, 3, 4, 5, 6, 7, 8};
		const std::vector<int64_t> shape = {2, 2, 2};

		writer.AddNpyData("multi_dim_array", data.data(), shape);
		writer.Close();
	}

	NpzReader reader(test_file_path);
	const NpData read_data = reader.Load("multi_dim_array");
	EXPECT_EQ(read_data.Elements(), 8);
	EXPECT_EQ(read_data.NumBytes(), 8 * sizeof(int));
}

// 测试追加模式
TEST_F(NpzWriterTest, AppendMode) {
	// 首先创建一个文件
	{
		NpzWriter writer(test_file_path, WriteMode::W);
		std::vector<float> data = {1.0f, 2.0f};
		std::vector<int64_t> shape = {2};
		writer.AddNpyData("first_array", data.data(), shape);
		writer.Close();
	}

	// 然后以追加模式打开并添加数据
	{
		NpzWriter writer(test_file_path, WriteMode::A);
		std::vector<int> data = {3, 4, 5};
		std::vector<int64_t> shape = {3};
		writer.AddNpyData("second_array", data.data(), shape);
		writer.Close();
	}

	// 验证两个数组都存在
	NpzReader reader(test_file_path);

	const NpData first_data = reader.Load("first_array");
	EXPECT_EQ(first_data.Elements(), 2);

	const NpData second_data = reader.Load("second_array");
	EXPECT_EQ(second_data.Elements(), 3);
}

// 测试空数组
TEST_F(NpzWriterTest, WriteEmptyArray) { {
		NpzWriter writer(test_file_path, WriteMode::W);
		const std::vector<float> empty_data;
		const std::vector<int64_t> empty_shape = {0};

		writer.AddNpyData("empty_array", empty_data.data(), empty_shape);
		writer.Close();
	}

	NpzReader reader(test_file_path);
	const NpData read_data = reader.Load("empty_array");
	EXPECT_EQ(read_data.Elements(), 0);
	EXPECT_EQ(read_data.NumBytes(), 0);
}

// 测试析构函数自动关闭
TEST_F(NpzWriterTest, DestructorClosesFile) {
	// 创建writer并在作用域结束时自动析构
	{
		NpzWriter writer(test_file_path, WriteMode::W);
		const std::vector<int> data = {1, 2, 3};
		const std::vector<int64_t> shape = {3};
		writer.AddNpyData("auto_close_test", data.data(), shape);
	}

	EXPECT_TRUE(std::filesystem::exists(test_file_path));

	NpzReader reader(test_file_path);
	const NpData read_data = reader.Load("auto_close_test");
	EXPECT_EQ(read_data.Elements(), 3);
}

// 测试不同形状的数组
TEST_F(NpzWriterTest, DifferentShapes) { {
		NpzWriter writer(test_file_path, WriteMode::W);

		// 一维数组
		const std::vector<float> data1 = {1.0f, 2.0f, 3.0f};
		const std::vector<int64_t> shape1 = {3};
		writer.AddNpyData("array_1d", data1.data(), shape1);

		// 二维数组
		const std::vector<float> data2 = {1.0f, 2.0f, 3.0f, 4.0f};
		const std::vector<int64_t> shape2 = {2, 2};
		writer.AddNpyData("array_2d", data2.data(), shape2);

		// 三维数组
		const std::vector<float> data3 = {1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f};
		const std::vector<int64_t> shape3 = {2, 2, 2};
		writer.AddNpyData("array_3d", data3.data(), shape3);

		writer.Close();
	}

	NpzReader reader(test_file_path);

	const NpData data_1d = reader.Load("array_1d");
	EXPECT_EQ(data_1d.Elements(), 3);

	const NpData data_2d = reader.Load("array_2d");
	EXPECT_EQ(data_2d.Elements(), 4);

	const NpData data_3d = reader.Load("array_3d");
	EXPECT_EQ(data_3d.Elements(), 8);
}

// 测试大量数据
TEST_F(NpzWriterTest, LargeData) { {
		NpzWriter writer(test_file_path, WriteMode::W);

		// 创建较大的数据集
		std::vector<double> large_data(10000);
		std::iota(large_data.begin(), large_data.end(), 1.0);
		const std::vector<int64_t> shape = {10000};

		writer.AddNpyData("large_array", large_data.data(), shape);
		writer.Close();
	}

	NpzReader reader(test_file_path);
	NpData read_data = reader.Load("large_array");
	EXPECT_EQ(read_data.Elements(), 10000);
	EXPECT_EQ(read_data.NumBytes(), 10000 * sizeof(double));

	// 验证数据完整性
	const double *read_ptr = read_data.Ptr<double>();
	for (size_t i = 0; i < 10 && i < read_data.Elements(); ++i)
		EXPECT_DOUBLE_EQ(read_ptr[i], static_cast<double>(i + 1));
}

// 测试错误处理
TEST_F(NpzWriterTest, ErrorHandling) {
	const std::string invalid_path = "/invalid_path/test.npz";
	EXPECT_THROW(NpzWriter writer(invalid_path, WriteMode::W), std::runtime_error);
}

// 测试 Close 方法
TEST_F(NpzWriterTest, CloseMethod) {
	NpzWriter writer(test_file_path, WriteMode::W);
	const std::vector<int> data = {1, 2, 3};
	const std::vector<int64_t> shape = {3};
	writer.AddNpyData("test_close", data.data(), shape);

	const char *result = writer.Close();
	EXPECT_EQ(result, nullptr);

	// 验证文件已正确创建
	EXPECT_TRUE(std::filesystem::exists(test_file_path));

	NpzReader reader(test_file_path);
	const NpData read_data = reader.Load("test_close");
	EXPECT_EQ(read_data.Elements(), 3);
}
} // namespace npy

int main(int argc, char **argv) {
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
