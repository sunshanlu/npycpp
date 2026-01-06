/**
 * Created by ssl on 2026/1/5.
 * Copyright (c) 2026 ssl. All rights reserved.
 */
#include <complex>

#include "npycpp/AbsWriter.hpp"

namespace npy {
char AbsWriter::MapType(const std::type_info &t) {
	if (t == typeid(float)) return 'f';
	if (t == typeid(double)) return 'f';
	if (t == typeid(long double)) return 'f';

	if (t == typeid(int)) return 'i';
	if (t == typeid(char)) return 'i';
	if (t == typeid(short)) return 'i';
	if (t == typeid(long)) return 'i';
	if (t == typeid(long long)) return 'i';

	if (t == typeid(unsigned char)) return 'u';
	if (t == typeid(unsigned short)) return 'u';
	if (t == typeid(unsigned long)) return 'u';
	if (t == typeid(unsigned long long)) return 'u';
	if (t == typeid(unsigned int)) return 'u';

	if (t == typeid(bool)) return 'b';

	if (t == typeid(std::complex<float>)) return 'c';
	if (t == typeid(std::complex<double>)) return 'c';
	if (t == typeid(std::complex<long double>)) return 'c';

	return '?';
}

char AbsWriter::BigEndianTest() {
	int x = 1;
	return (reinterpret_cast<char *>(&x)[0]) ? '<' : '>';
}
} // npy
