/**
 * Created by ssl on 2026/1/1.
 * Copyright (c) 2026 ssl. All rights reserved.
 */

#pragma once

#include <cstdint>
#include <vector>

#include <opencv2/opencv.hpp>
#include <Eigen/Dense>
#include <spdlog/spdlog.h>


namespace npy {
class NpData {
public:
	template<typename T>
	using RowMajorMatrix = Eigen::Matrix<T, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor>;

	NpData();

	NpData(const std::vector<int64_t> &shape, const size_t &word_size, const bool &fortran_order);

	NpData(NpData &&rhs) noexcept;

	NpData(const NpData &rhs);

	template<typename T>
	T *Ptr() { return reinterpret_cast<T *>(data_); }

	template<typename T>
	const T *Ptr() const { return reinterpret_cast<const T *>(data_); }

	template<typename T>
	cv::Mat CVMat();

	template<typename T>
	cv::Mat CVMat() const { return const_cast<NpData *>(this)->CVMat<T>(); }

	template<typename T>
	RowMajorMatrix<T> EigenMatrix();

	template<typename T>
	RowMajorMatrix<T> EigenMatrix() const { return const_cast<NpData *>(this)->EigenMatrix<T>(); }

	template<typename T>
	cv::Mat CVMatZC();

	template<typename T>
	const cv::Mat CVMatZC() const { return const_cast<NpData *>(this)->CVMatZC<T>(); }

	template<typename T>
	Eigen::Map<RowMajorMatrix<T> > EigenMatrixZC();

	template<typename T>
	Eigen::Map<const RowMajorMatrix<T>> EigenMatrixZC() const;

	std::size_t Elements() const;

	std::size_t NumBytes() const;

	std::vector<int64_t> Shape() const;

	~NpData();

private:
	template<typename T>
	void Check(const std::size_t &shape_limit) const;

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

	char *data_ = nullptr;
	std::vector<int64_t> shape_;
	size_t word_size_;
	bool fortran_order_;
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
