#pragma once
#include <string>
#include <vector>

using namespace std;

namespace Tools {
	inline char* string_conv(string s);
	vector<char*>& string_split(string s, const char* delimeter);

	template <class T>
	void swap(T& first, T& second);
}

