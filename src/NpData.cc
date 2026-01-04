/**
 * Created by ssl on 2026/1/1.
 * Copyright (c) 2026 ssl. All rights reserved.
 */
#include <numeric>
#include <cstring>

#include "npycpp/NpData.hpp"

namespace npy {
NpData::NpData() : word_size_(0),
                   fortran_order_(false) {}

NpData::NpData(const std::vector<int64_t> &shape,
               const size_t &word_size,
               const bool &fortran_order)
	: shape_(shape),
	  word_size_(word_size),
	  fortran_order_(fortran_order) {
	data_ = new char[NumBytes()];
}

NpData::NpData(NpData &&rhs) noexcept : data_(rhs.data_),
                                        shape_(std::move(rhs.shape_)),
                                        word_size_(std::move(rhs.word_size_)),
                                        fortran_order_(std::move(rhs.fortran_order_)) {
	rhs.shape_.clear();
	rhs.data_ = nullptr;
	rhs.word_size_ = 0;
	rhs.fortran_order_ = false;
}

NpData::NpData(const NpData &rhs) : shape_(rhs.shape_),
                                    word_size_(rhs.word_size_),
                                    fortran_order_(rhs.fortran_order_) {
	data_ = new char[NumBytes()];
	memcpy(data_, rhs.data_, NumBytes());
}

std::size_t NpData::Elements() const {
	if (shape_.empty())
		return 0;

	return std::accumulate(shape_.begin(), shape_.end(), 1, std::multiplies<int64_t>());
}

std::size_t NpData::NumBytes() const {
	return Elements() * word_size_;
}

std::vector<int64_t> NpData::Shape() const { return shape_; }

NpData::~NpData() {
	if (data_ == nullptr)
		return;

	delete[] data_;
}
}
