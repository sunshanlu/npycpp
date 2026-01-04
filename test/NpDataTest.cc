/**
 * Created by ssl on 2026/1/1.
 * Copyright (c) 2026 ssl. All rights reserved.
 */

#include <gtest/gtest.h>
#include <fstream>
#include <vector>
#include <cstring>
#include <ranges>

#include "npycpp/NpData.hpp"


namespace npy {
// 测试 NpData 类
class NpDataTest : public ::testing::Test {
protected:
	void SetUp() override {
		// 设置测试数据
		shape = {2, 3};
		word_size = sizeof(float);
		fortran_order = false;
	}

	std::vector<int64_t> shape;
	size_t word_size = 0;
	bool fortran_order = false;
};

// 测试默认构造函数
TEST_F(NpDataTest, DefaultConstructor) {
	const NpData data;
	EXPECT_EQ(data.Elements(), 0);
	EXPECT_EQ(data.NumBytes(), 0);
}

// 测试参数化构造函数
TEST_F(NpDataTest, ParameterizedConstructor) {
	NpData data(shape, word_size, fortran_order);
	EXPECT_EQ(data.Elements(), 6);         // 2 * 3
	EXPECT_EQ(data.NumBytes(), 24);        // 6 * sizeof(float)
	EXPECT_NE(data.Ptr<float>(), nullptr); // 不应该为nullptr
}

// 测试拷贝构造函数
TEST_F(NpDataTest, CopyConstructor) {
	NpData original(shape, word_size, fortran_order);

	// 初始化原始数据
	float *original_ptr = original.Ptr<float>();
	for (size_t i = 0; i < original.Elements(); ++i) {
		original_ptr[i] = static_cast<float>(i);
	}

	NpData copy(original);
	EXPECT_EQ(copy.Elements(), original.Elements());
	EXPECT_EQ(copy.NumBytes(), original.NumBytes());

	const float *copy_ptr = copy.Ptr<float>();
	const float *orig_ptr = original.Ptr<float>();
	for (size_t i = 0; i < original.Elements(); ++i) {
		EXPECT_EQ(copy_ptr[i], orig_ptr[i]);
	}
}

// 测试移动构造函数
TEST_F(NpDataTest, MoveConstructor) {
	NpData original(shape, word_size, fortran_order);
	char *original_data = original.Ptr<char>();

	// 确保原始对象有数据
	ASSERT_NE(original_data, nullptr);

	const NpData moved = std::move(original);

	// 移动后的对象应该有正确的数据
	EXPECT_EQ(moved.Elements(), 6);
	EXPECT_EQ(moved.NumBytes(), 24);

	// 原始对象应该被重置
	EXPECT_EQ(original.Elements(), 0);
	EXPECT_EQ(original.NumBytes(), 0);
	EXPECT_EQ(original.Ptr<char>(), nullptr);
}

// 测试 Ptr 方法
TEST_F(NpDataTest, PtrMethod) {
	NpData data(shape, word_size, fortran_order);
	float *float_ptr = data.Ptr<float>();
	const float *const_float_ptr = std::as_const(data).Ptr<float>();

	EXPECT_NE(float_ptr, nullptr);
	EXPECT_NE(const_float_ptr, nullptr);
}

// 测试 Elements 方法
TEST_F(NpDataTest, Elements) {
	NpData data({2, 3, 4}, word_size, fortran_order);
	EXPECT_EQ(data.Elements(), 24); // 2 * 3 * 4

	NpData data2({5}, word_size, fortran_order);
	EXPECT_EQ(data2.Elements(), 5);

	NpData data3({}, word_size, fortran_order);
	EXPECT_EQ(data3.Elements(), 0);
}

// 测试 NumBytes 方法
TEST_F(NpDataTest, NumBytes) {
	const NpData data({2, 3}, sizeof(int), fortran_order);
	EXPECT_EQ(data.NumBytes(), 24); // 6 elements * 4 bytes per int

	const NpData data2({1, 1, 1}, sizeof(double), fortran_order);
	EXPECT_EQ(data2.NumBytes(), 8); // 1 element * 8 bytes per double
}

// 测试析构函数
TEST_F(NpDataTest, Destructor) {
	auto *data = new NpData(shape, word_size, fortran_order);
	char *ptr = data->Ptr<char>();
	EXPECT_NE(ptr, nullptr);
	delete data;
}

// 测试零拷贝 cv::Mat 转换
TEST_F(NpDataTest, CVMatZeroCopy) {
	NpData data(shape, word_size, fortran_order);

	// 初始化数据
	float *data_ptr = data.Ptr<float>();
	for (size_t i = 0; i < data.Elements(); ++i)
		data_ptr[i] = static_cast<float>(i + 1);

	cv::Mat mat = data.CVMatZC<float>();

	EXPECT_EQ(mat.rows, 2);
	EXPECT_EQ(mat.cols, 3);
	EXPECT_EQ(mat.type(), CV_32F);

	// 验证数据内容（零拷贝，数据应该相同）
	for (int i = 0; i < mat.rows; ++i)
		for (int j = 0; j < mat.cols; ++j)
			EXPECT_FLOAT_EQ(mat.at<float>(i, j), static_cast<float>(i * 3 + j + 1));

	// 验证是零拷贝（修改 mat 应该影响原始数据）
	mat.at<float>(0, 0) = 999.0f;
	EXPECT_FLOAT_EQ(data_ptr[0], 999.0f);
}

// 测试零拷贝 Eigen Matrix 转换
TEST_F(NpDataTest, EigenMatrixZeroCopy) {
	NpData data(shape, word_size, fortran_order);

	// 初始化数据
	float *data_ptr = data.Ptr<float>();
	for (size_t i = 0; i < data.Elements(); ++i)
		data_ptr[i] = static_cast<float>(i + 1);

	auto eigen_map = data.EigenMatrixZC<float>();

	EXPECT_EQ(eigen_map.rows(), 2);
	EXPECT_EQ(eigen_map.cols(), 3);

	// 验证数据内容
	for (int i = 0; i < eigen_map.rows(); ++i)
		for (int j = 0; j < eigen_map.cols(); ++j)
			EXPECT_FLOAT_EQ(eigen_map(i, j), static_cast<float>(i * 3 + j + 1));

	// 验证是零拷贝（修改 eigen_map 应该影响原始数据）
	eigen_map(0, 0) = 999.0f;
	EXPECT_FLOAT_EQ(data_ptr[0], 999.0f);
}

// 测试 const 版本的零拷贝转换
TEST_F(NpDataTest, ConstCVMatZeroCopy) {
	NpData data(shape, word_size, fortran_order);

	// 初始化数据
	float *data_ptr = data.Ptr<float>();
	for (size_t i = 0; i < data.Elements(); ++i)
		data_ptr[i] = static_cast<float>(i + 1);

	const NpData &const_data = data;
	const cv::Mat mat = const_data.CVMatZC<float>();

	EXPECT_EQ(mat.rows, 2);
	EXPECT_EQ(mat.cols, 3);
	EXPECT_EQ(mat.type(), CV_32F);

	// 验证地址状态是否一致
	EXPECT_EQ(mat.data, reinterpret_cast<const uchar*>(const_data.Ptr<float>()));
}

// 测试 const 版本的 Eigen 零拷贝转换
TEST_F(NpDataTest, ConstEigenMatrixZeroCopy) {
	NpData data(shape, word_size, fortran_order);

	// 初始化数据
	float *data_ptr = data.Ptr<float>();
	for (size_t i = 0; i < data.Elements(); ++i)
		data_ptr[i] = static_cast<float>(i + 1);

	const NpData &const_data = data;
	auto eigen_map = const_data.EigenMatrixZC<float>();

	EXPECT_EQ(eigen_map.rows(), 2);
	EXPECT_EQ(eigen_map.cols(), 3);

	// 验证数据内容
	for (int i = 0; i < eigen_map.rows(); ++i)
		for (int j = 0; j < eigen_map.cols(); ++j)
			EXPECT_FLOAT_EQ(eigen_map(i, j), static_cast<float>(i * 3 + j + 1));

	// 验证内存地址是否一致
	EXPECT_EQ(eigen_map.data(), const_data.Ptr<float>());
}

// 测试一维数据转换
TEST_F(NpDataTest, OneDimensionalConversion) {
	std::vector<int64_t> one_dim_shape = {5};
	NpData data(one_dim_shape, sizeof(int), fortran_order);

	// 初始化数据
	int *data_ptr = data.Ptr<int>();
	for (size_t i = 0; i < data.Elements(); ++i)
		data_ptr[i] = static_cast<int>(i + 1);

	// 测试 cv::Mat 转换
	cv::Mat mat = data.CVMat<int>();
	EXPECT_EQ(mat.rows, 5);
	EXPECT_EQ(mat.cols, 1);
	EXPECT_EQ(mat.type(), CV_32S);

	// 测试 Eigen Matrix 转换
	auto eigen_mat = data.EigenMatrix<int>();
	EXPECT_EQ(eigen_mat.rows(), 5);
	EXPECT_EQ(eigen_mat.cols(), 1);

	// 测试零拷贝转换
	auto eigen_map = data.EigenMatrixZC<int>();
	EXPECT_EQ(eigen_map.rows(), 5);
	EXPECT_EQ(eigen_map.cols(), 1);
}

// 测试错误情况
TEST_F(NpDataTest, ErrorCases) {
	NpData data(shape, word_size, fortran_order);

	// 测试空数据
	NpData empty_data;
	EXPECT_THROW(empty_data.CVMat<float>(), std::runtime_error);
	EXPECT_THROW(empty_data.EigenMatrix<float>(), std::runtime_error);
	EXPECT_THROW(empty_data.CVMatZC<float>(), std::runtime_error);
	EXPECT_THROW(empty_data.EigenMatrixZC<float>(), std::runtime_error);
}

// 测试 Shape 方法
TEST_F(NpDataTest, ShapeMethod) {
	NpData data(shape, word_size, fortran_order);
	auto returned_shape = data.Shape();

	EXPECT_EQ(returned_shape.size(), 2);
	EXPECT_EQ(returned_shape[0], 2);
	EXPECT_EQ(returned_shape[1], 3);

	// 测试一维形状
	NpData data1d({4}, word_size, fortran_order);
	auto shape1d = data1d.Shape();
	EXPECT_EQ(shape1d.size(), 1);
	EXPECT_EQ(shape1d[0], 4);
}

// 测试不同数据类型的转换
TEST_F(NpDataTest, DifferentDataTypes) {
	std::vector<std::pair<size_t, std::function<void()> > > test_cases = {

		// clang-format off
		{
			sizeof(double), [this]() {
				NpData data(shape, sizeof(double), fortran_order);
				double *ptr = data.Ptr<double>();
				for (size_t i = 0; i < data.Elements(); ++i) {
					ptr[i] = static_cast<double>(i + 0.5);
				}

				cv::Mat mat = data.CVMat<double>();
				EXPECT_EQ(mat.type(), CV_64F);

				auto eigen_mat = data.EigenMatrix<double>();
				auto eigen_map = data.EigenMatrixZC<double>();

				// 添加验证操作
				EXPECT_EQ(eigen_mat.rows(), 2);  // 验证矩阵行数
				EXPECT_EQ(eigen_mat.cols(), 3);  // 验证矩阵列数
				EXPECT_EQ(eigen_map.rows(), 2);  // 验证映射行数
				EXPECT_EQ(eigen_map.cols(), 3);  // 验证映射列数

				// 验证数据内容一致性
				for (int i = 0; i < eigen_mat.rows(); ++i)
					for (int j = 0; j < eigen_mat.cols(); ++j)
						EXPECT_EQ(eigen_mat(i, j), eigen_map(i, j));
			}
		},

		{
			sizeof(int), [this]() {
				NpData data(shape, sizeof(int), fortran_order);
				int *ptr = data.Ptr<int>();
				for (size_t i = 0; i < data.Elements(); ++i) {
					ptr[i] = static_cast<int>(i + 1);
				}

				cv::Mat mat = data.CVMat<int>();
				EXPECT_EQ(mat.type(), CV_32S);

				auto eigen_mat = data.EigenMatrix<int>();
				auto eigen_map = data.EigenMatrixZC<int>();

				// 添加验证操作
				EXPECT_EQ(eigen_mat.rows(), 2);  // 验证矩阵行数
				EXPECT_EQ(eigen_mat.cols(), 3);  // 验证矩阵列数
				EXPECT_EQ(eigen_map.rows(), 2);  // 验证映射行数
				EXPECT_EQ(eigen_map.cols(), 3);  // 验证映射列数

				// 验证数据内容一致性
				for (int i = 0; i < eigen_mat.rows(); ++i)
					for (int j = 0; j < eigen_mat.cols(); ++j)
						EXPECT_EQ(eigen_mat(i, j), eigen_map(i, j));
			}
		}
		// clang-format on
	};

	for (const auto &function: test_cases | std::views::values)
		function();
}
} // namespace npy

int main(int argc, char **argv) {
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
