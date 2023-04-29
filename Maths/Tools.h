#pragma once

#ifndef TOOLS_H
#define TOOLS_H

class Component;
#include <string>
#include <vector>
#include <unordered_set>
#include <iostream>

using namespace std;


namespace Tools {
	inline char* string_conv(string s);
	vector<char*>& string_split(string s, const char* delimeter);

	template <class T>
	void swap_move(T& first, T& second);

	int gcd(const int& a, const int& b);
	inline int lcm(const int& a, const int& b) { return abs(a * b) / gcd(a, b); }

	template <class T, class _Comp = std::greater_equal<T>>
	int quicksort_partition(vector<T>& to_sort, _Comp& comparer, int start, int end);

	template <class T, class _Comp = std::greater_equal<T>>
	void quicksort(vector<T>& to_sort, _Comp& comparer, int start = 0, int end = INT_MAX);
}

#include "Maths.h";
template <class T>
void Tools::swap_move(T& first, T& second) {
	T tmp(std::move(second));
	second = std::move(first);
	first = std::move(tmp);
}

template <class T, class _Comp>
int quicksort_partition(vector<T>& to_sort, _Comp& comparer, int start, int end) {
	T& pivot = to_sort[end];
	int p_index = start;

	for (int i = start; i < end; i++) {
		if (comparer(to_sort[i], pivot)) {
			swap_move(to_sort[i], to_sort[p_index]);
			p_index++;
		}
	}
	swap_move(to_sort[p_index], to_sort[end]);
	return p_index;
}

template <class T, class _Comp = std::greater_equal<T>>
void quicksort(vector<T>& to_sort, _Comp& comparer, int start, int end) {
	end = min(end, (int)to_sort.size() - 1);
	if (start >= end) return;

	int pivot = quicksort_partition<T, _Comp>(to_sort, comparer, start, end);
	quicksort<T, _Comp>(to_sort, comparer, start, pivot - 1);
	quicksort<T, _Comp>(to_sort, comparer, pivot + 1, end);
}


#endif // !TOOLS_H

