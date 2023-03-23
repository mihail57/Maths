#pragma once
#include <string>
#include <vector>
#include "Maths.h"

namespace Tools {
	vector<char*>& string_split(string s, const char* delimeter);

	void test_print_tree(Component* cmp, int tab = 0);
}

