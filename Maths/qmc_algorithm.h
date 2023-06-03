
#ifndef QMC_ALGORITHM_H
#define QMC_ALGORITHM_H

#include "Maths.h"

#pragma once

class qmc_algorithm_solver
{
private:
	struct minterm
	{
	private:
		long long content;
		int num_ones = 0;
	public:
		minterm(vector<bool>& b, vector<long long>& total_comps);
		minterm(const minterm& b) : content(b.content), dont_care(b.dont_care), components(b.components) { num_ones = b.num_ones; };

		inline long long get_content() { return content; }

		bool operator==(const minterm& b);
		bool try_merge_minterms(minterm& b, int var_num);

		bool contains_component(long long cmp);

		bool is_final_form = false;
		vector<int> components;
		map<int, bool> dont_care;
	};

	void merge_minterms();
	void get_essential_implicants();
	vector<minterm>* petricks_method(vector<long long>& not_covered);

	int var_num;

	vector<minterm> minterms;
	vector<long long> total_comps;
public:
	qmc_algorithm_solver(vector<vector<bool>>* truth_table);

	Component* get_result(vector<char>* vars);
};

#endif // !QMC_ALGORITHM_H

