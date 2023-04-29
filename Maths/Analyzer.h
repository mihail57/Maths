#pragma once

#include "Maths.h"
#include <map>
#include <stack>
#include <algorithm>
#include <unordered_map>

using namespace std;
using namespace Tools;


typedef stack<Component*> comp_stack;

class EquationMatcher {
	Component* equation;

	map<char, vector<Component*>*> vars;

	map<char, bool> occurs;
	vector<pair<Component*, char>> backward_candidates;

	bool recursive_compare(map<char, vector<Component*>*>::iterator iter);
	bool is_in_backwards_candidates(Component* c);

	bool Match(Component* first, Component* second);
public:
	EquationMatcher(string eq);

	map<char, Component*>* FindMatch(Component* cmp);
};

