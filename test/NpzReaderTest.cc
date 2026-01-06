/**
 * Created by ssl on 2026/1/1.
 * Copyright (c) 2026 ssl. All rights reserved.
 */

#include <gtest/gtest.h>
#include <fstream>
#include <vector>
#include <cstring>
#include <filesystem>
#include <memory>

#include "npycpp/NpzReader.hpp"
#include "npycpp/NpzWriter.hpp"


namespace npy {
// 测试 NpzReader 类
class NpzReaderTest : public ::testing::Test {
protected:
	void SetUp() override {
		spdlog::set_level(spdlog::level::err);

		// 创建测试用的 .npz 文件
		test_npz_path = "test_data.npz";
		CreateTestNpzFile();
	}

	void TearDown() override {
		namespace fs = std::filesystem;

		// 清理测试文件
		if (fs::exists(test_npz_path))
			fs::remove(test_npz_path);
	}

	void CreateTestNpzFile() {
		// 使用 NpzWriter 创建测试文件
		NpzWriter writer(test_npz_path, WriteMode::W);

		// 准备测试数据
		const std::vector<float> data1 = {1.0f, 2.0f, 3.0f, 4.0f};
		const std::vector<int> data2 = {10, 20, 30};

		const std::vector<int64_t> shape1 = {2, 2};
		const std::vector<int64_t> shape2 = {3};

		writer.AddNpyData<float>("array1", data1.data(), shape1);
		writer.AddNpyData<int>("array2", data2.data(), shape2);
		writer.Close();
	}

	std::string test_npz_path;
};

// 测试 NpzReader 构造函数
TEST_F(NpzReaderTest, Constructor) {
	EXPECT_NO_THROW({NpzReader reader(test_npz_path);});
	EXPECT_THROW({NpzReader reader("nonexistent_file.npz");}, std::runtime_error);
}

// 测试 Load 方法
TEST_F(NpzReaderTest, Load) {
	NpzReader reader(test_npz_path);

	const NpData data1 = reader.Load("array1");
	EXPECT_EQ(data1.Elements(), 4);
	EXPECT_EQ(data1.NumBytes(), 16);

	const NpData data2 = reader.Load("array2");
	EXPECT_EQ(data2.Elements(), 3);
	EXPECT_EQ(data2.NumBytes(), 12);

	// 测试不存在的变量
	const NpData data3 = reader.Load("nonexistent");
	EXPECT_EQ(data3.Elements(), 0);
	EXPECT_EQ(data3.NumBytes(), 0);
}

// 测试操作符[]
TEST_F(NpzReaderTest, SubscriptOperator) {
	NpzReader reader(test_npz_path);

	const NpData data1 = reader["array1"];
	EXPECT_EQ(data1.Elements(), 4);

	const NpData data2 = reader["array2"];
	EXPECT_EQ(data2.Elements(), 3);
}

// 测试 ParseNpyHeader 方法
TEST_F(NpzReaderTest, ParseNpyHeader) {
	// 因为 ParseNpyHeader 是私有方法, 我们通过 Load 方法间接测试它
	NpzReader reader(test_npz_path);
	const NpData data = reader.Load("array1");

	// 验证数据被正确解析
	EXPECT_GT(data.Elements(), 0);
	EXPECT_GT(data.NumBytes(), 0);
}

// 集成测试：完整的读写流程
TEST_F(NpzReaderTest, ReadWriteIntegration) {
	namespace fs = std::filesystem;

	// 创建一个新文件用于写入
	const std::string test_write_path = "test_write.npz";

	// 写入数据
	{
		NpzWriter writer(test_write_path, WriteMode::W);
		std::vector write_data = {1.1, 2.2, 3.3, 4.4, 5.5};
		std::vector<int64_t> shape = {5};
		writer.AddNpyData<double>("test_array", write_data.data(), shape);
	}

	// 读取数据
	{
		NpzReader reader(test_write_path);
		NpData data = reader.Load("test_array");

		EXPECT_EQ(data.Elements(), 5);
		EXPECT_EQ(data.NumBytes(), 40);

		const double *read_data = data.Ptr<double>();
		const std::vector expected = {1.1, 2.2, 3.3, 4.4, 5.5};

		for (size_t i = 0; i < data.Elements(); ++i)
			EXPECT_DOUBLE_EQ(read_data[i], expected[i]);
	}

	// 清理测试文件
	fs::remove(test_write_path);
}

// 边界条件测试
TEST_F(NpzReaderTest, EdgeCases) {
	namespace fs = std::filesystem;

	// 测试空数组
	const std::string edge_test_path = "edge_test.npz"; {
		NpzWriter writer(edge_test_path, WriteMode::W);
		const std::vector<float> empty_data;
		const std::vector<int64_t> empty_shape = {0};
		writer.AddNpyData("empty_array", empty_data.data(), empty_shape);
		writer.Close();
	}

	// clang-format off
	{
		NpzReader reader(edge_test_path);
		NpData data = reader.Load("empty_array");
		EXPECT_EQ(data.Elements(), 0);
		EXPECT_EQ(data.NumBytes(), 0);
	}
	// clang-format on

	fs::remove(edge_test_path);
}

// 测试一维数组
TEST_F(NpzReaderTest, OneDimensionalArray) {
	namespace fs = std::filesystem;

	const std::string test_1d_path = "test_1d.npz";

	// clang-format off
	{
		NpzWriter writer(test_1d_path, WriteMode::W);
		std::vector<int> data = {1, 2, 3, 4, 5, 6};
		std::vector<int64_t> shape = {6};
		writer.AddNpyData("oned_array", data.data(), shape);
		writer.Close();
	}
	// clang-format on

	{
		NpzReader reader(test_1d_path);
		NpData npdata = reader.Load("oned_array");

		EXPECT_EQ(npdata.Elements(), 6);
		EXPECT_EQ(npdata.NumBytes(), 24);

		const int *read_data = npdata.Ptr<int>();
		for (int i = 0; i < 6; ++i)
			EXPECT_EQ(read_data[i], i + 1);
	}

	fs::remove(test_1d_path);
}
}

int main(int argc, char **argv) {
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
