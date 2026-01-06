/**
 * @file NpData.hpp
 * @brief NumPy数据结构的C++封装类，支持与OpenCV和Eigen的互操作。
 * @author ssl
 * @date 2026/1/1
 * @copyright (c) 2026 ssl. All rights reserved.
 */

#pragma once

#include <cstdint>
#include <vector>

#include <opencv2/opencv.hpp>
#include <Eigen/Dense>
#include <spdlog/spdlog.h>

namespace npy {
/**
 * @brief NumPy数据结构的C++封装类。
 *
 * 该类封装了NumPy数组的数据，支持与OpenCV的cv::Mat和Eigen矩阵的零拷贝或拷贝转换。
 */
class NpData {
public:
	/**
	 * @brief 行主序矩阵类型别名。
	 * @tparam T 数据类型。
	 */
	template<typename T>
	using RowMajorMatrix = Eigen::Matrix<T, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor>;

	/**
	 * @brief 默认构造函数。
	 */
	NpData();

	/**
	 * @brief 构造函数，根据形状、字大小和顺序初始化数据。
	 * @param shape 数据形状。
	 * @param word_size 每个元素的字节大小。
	 * @param fortran_order 是否为Fortran顺序。
	 */
	NpData(const std::vector<int64_t> &shape, const size_t &word_size, const bool &fortran_order);

	/**
	 * @brief 移动构造函数。
	 * @param rhs 要移动的NpData对象。
	 */
	NpData(NpData &&rhs) noexcept;

	/**
	 * @brief 拷贝构造函数。
	 * @param rhs 要拷贝的NpData对象。
	 */
	NpData(const NpData &rhs);

	/**
	 * @brief 获取数据指针。
	 * @tparam T 数据类型。
	 * @return 指向数据的指针。
	 */
	template<typename T>
	T *Ptr() { return reinterpret_cast<T *>(data_); }

	/**
	 * @brief 获取常量数据指针。
	 * @tparam T 数据类型。
	 * @return 指向常量的指针。
	 */
	template<typename T>
	const T *Ptr() const { return reinterpret_cast<const T *>(data_); }

	/**
	 * @brief 转换为OpenCV的cv::Mat（拷贝）。
	 * @tparam T 数据类型。
	 * @return cv::Mat对象。
	 */
	template<typename T>
	cv::Mat CVMat();

	/**
	 * @brief 转换为OpenCV的cv::Mat（常量拷贝）。
	 * @tparam T 数据类型。
	 * @return cv::Mat对象。
	 */
	template<typename T>
	cv::Mat CVMat() const { return const_cast<NpData *>(this)->CVMat<T>(); }

	/**
	 * @brief 转换为Eigen矩阵（拷贝）。
	 * @tparam T 数据类型。
	 * @return Eigen矩阵。
	 */
	template<typename T>
	RowMajorMatrix<T> EigenMatrix();

	/**
	 * @brief 转换为Eigen矩阵（常量拷贝）。
	 * @tparam T 数据类型。
	 * @return Eigen矩阵。
	 */
	template<typename T>
	RowMajorMatrix<T> EigenMatrix() const { return const_cast<NpData *>(this)->EigenMatrix<T>(); }

	/**
	 * @brief 零拷贝转换为OpenCV的cv::Mat。
	 * @tparam T 数据类型。
	 * @return cv::Mat对象。
	 */
	template<typename T>
	cv::Mat CVMatZC();

	/**
	 * @brief 零拷贝转换为OpenCV的cv::Mat（常量）。
	 * @tparam T 数据类型。
	 * @return cv::Mat对象。
	 */
	template<typename T>
	const cv::Mat CVMatZC() const { return const_cast<NpData *>(this)->CVMatZC<T>(); }

	/**
	 * @brief 零拷贝转换为Eigen矩阵映射。
	 * @tparam T 数据类型。
	 * @return Eigen矩阵映射。
	 */
	template<typename T>
	Eigen::Map<RowMajorMatrix<T> > EigenMatrixZC();

	/**
	 * @brief 零拷贝转换为常量Eigen矩阵映射。
	 * @tparam T 数据类型。
	 * @return 常量Eigen矩阵映射。
	 */
	template<typename T>
	Eigen::Map<const RowMajorMatrix<T>> EigenMatrixZC() const;

	/**
	 * @brief 获取元素总数。
	 * @return 元素数量。
	 */
	std::size_t Elements() const;

	/**
	 * @brief 获取总字节数。
	 * @return 字节数。
	 */
	std::size_t NumBytes() const;

	/**
	 * @brief 获取数据形状。
	 * @return 形状向量。
	 */
	std::vector<int64_t> Shape() const;

	/**
	 * @brief 析构函数。
	 */
	~NpData();

private:
	/**
	 * @brief 检查模板类型和形状的有效性。
	 * @tparam T 数据类型。
	 * @param shape_limit 形状维度限制。
	 */
	template<typename T>
	void Check(const std::size_t &shape_limit) const;

	/**
	 * @brief 根据数据类型推断OpenCV类型。
	 * @tparam T 数据类型。
	 * @return OpenCV类型。
	 */
	template<typename T>
	int InferCVType() {
		if constexpr (std::is_same_v<T, float>)
			return CV_32F;

		if constexpr (std::is_same_v<T, double>)
			return CV_64F;

		if constexpr (std::is_same_v<T, int8_t> || std::is_same_v<T, char>)
			return CV_8S;

		if constexpr (std::is_same_v<T, uint8_t> || std::is_same_v<T, unsigned char>)
			return CV_8U;

		if constexpr (std::is_same_v<T, int16_t> || std::is_same_v<T, short>)
			return CV_16S;

		if constexpr (std::is_same_v<T, uint16_t> || std::is_same_v<T, unsigned short>)
			return CV_16U;

		if constexpr (std::is_same_v<T, int32_t> || std::is_same_v<T, int>)
			return CV_32S;

		throw std::runtime_error("unsupported data type for cv::Mat conversion");
	}

	char *data_ = nullptr;       /**< 数据指针。 */
	std::vector<int64_t> shape_; /**< 数据形状。 */
	size_t word_size_;           /**< 每个元素的字节大小。 */
	bool fortran_order_;         /**< 是否为Fortran顺序。 */
};

template<typename T>
cv::Mat NpData::CVMat() {
	Check<T>(3);

	const int cv_type = InferCVType<T>();

	// 处理不同维度的形状
	if (shape_.size() == 1)
		return cv::Mat(
			static_cast<int>(shape_[0]),
			1,
			cv_type,
			data_
		).clone();
	if (shape_.size() == 2)
		return cv::Mat(
			static_cast<int>(shape_[0]),
			static_cast<int>(shape_[1]),
			cv_type,
			data_
		).clone();

	return cv::Mat(
		static_cast<int>(shape_[0]),
		static_cast<int>(shape_[1]),
		CV_MAKETYPE(cv_type, static_cast<int>(shape_[2])),
		data_
	).clone();
}

template<typename T>
NpData::RowMajorMatrix<T> NpData::EigenMatrix() {
	Check<T>(2);

	// 处理不同维度的形状
	if (shape_.size() == 1) {
		Eigen::Matrix<T, Eigen::Dynamic, Eigen::Dynamic> result(shape_[0], 1);
		std::memcpy(result.data(), data_, shape_[0] * sizeof(T));
		return result;
	}

	// 二维矩阵：(rows, cols)
	Eigen::Matrix<T, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor> result(shape_[0], shape_[1]);
	std::memcpy(result.data(), data_, shape_[0] * shape_[1] * sizeof(T));
	return result;
}

template<typename T>
Eigen::Map<NpData::RowMajorMatrix<T> > NpData::EigenMatrixZC() {
	Check<T>(2);

	if (shape_.size() == 1) {
		Eigen::Map<RowMajorMatrix<T> > mapdata(
			reinterpret_cast<T *>(data_),
			shape_[0],
			1
		);

		return mapdata;
	}

	Eigen::Map<RowMajorMatrix<T> > mapdata(
		reinterpret_cast<T *>(data_),
		shape_[0],
		shape_[1]
	);

	return mapdata;
}

template<typename T>
Eigen::Map<const NpData::RowMajorMatrix<T>> NpData::EigenMatrixZC() const {
	Check<T>(2);

	if (shape_.size() == 1) {
		Eigen::Map<const RowMajorMatrix<T>> mapdata(
			reinterpret_cast<const T *>(data_),
			shape_[0],
			1
		);

		return mapdata;
	}

	Eigen::Map<const RowMajorMatrix<T>> mapdata(
		reinterpret_cast<const T *>(data_),
		shape_[0],
		shape_[1]
	);

	return mapdata;
}

template<typename T>
void NpData::Check(const std::size_t &shape_limit) const {
	if (data_ == nullptr || shape_.empty())
		throw std::runtime_error("data_ is nullptr");

	if (sizeof(T) != word_size_)
		throw std::runtime_error(fmt::format("template T's word size != {}", word_size_));

	if (shape_.size() > shape_limit)
		throw std::runtime_error("shape dimension > 2 is not supported for Eigen::Matrix");

	if constexpr (!std::is_arithmetic_v<T>)
		throw std::runtime_error("unsupported data type for Eigen::MatrixXd conversion");
}

template<typename T>
cv::Mat NpData::CVMatZC() {
	Check<T>(3);

	const int cv_type = InferCVType<T>();

	// 处理不同维度的形状
	if (shape_.size() == 1)
		return cv::Mat(
			static_cast<int>(shape_[0]),
			1,
			cv_type,
			data_
		);
	if (shape_.size() == 2)
		return cv::Mat(
			static_cast<int>(shape_[0]),
			static_cast<int>(shape_[1]),
			cv_type,
			data_
		);

	return cv::Mat(
		static_cast<int>(shape_[0]),
		static_cast<int>(shape_[1]),
		CV_MAKETYPE(cv_type, static_cast<int>(shape_[2])),
		data_
	);
}
}
