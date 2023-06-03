#include "qmc_algorithm.h"

qmc_algorithm_solver::qmc_algorithm_solver(vector<vector<bool>>* truth_table) {
	var_num = truth_table->at(0).size() - 1;
	for (auto tmp : *truth_table)
	{
		if (tmp[tmp.size() - 1]) {
			minterms.push_back(minterm(tmp, total_comps));
		}
	}
	merge_minterms();
	get_essential_implicants();
}

qmc_algorithm_solver::minterm::minterm(vector<bool>& b, vector<long long>& total_comps) {
	content = 0; 
	num_ones = 0;
	for (int i = 0; i < b.size() - 1; i++) {
		content <<= 1;
		if (b[i]) {
			content += 1;
			num_ones++;
		}
	}
	components.push_back(content);
	total_comps.push_back(content);
}

bool qmc_algorithm_solver::minterm::try_merge_minterms(minterm& b, int var_num) {
	if (abs(num_ones - b.num_ones) != 1) return false;

	long long diff = content ^ b.content;
	int index = 0, num_diff = 0, iter = 0;
	while (iter < var_num)
	{
		if (diff % 2 || (dont_care[iter] && !b.dont_care[iter]) || (!dont_care[iter] && b.dont_care[iter])) {
			index = iter;
			num_diff++;
			if (num_diff > 1) return false;
		}
		diff >>= 1;
		iter++;
	}

	dont_care[var_num - index - 1] = true;
	for (auto t : b.components) components.push_back(t);
	return true;
}

template<typename T>
bool are_vectors_similar(const vector<T>& a, const vector<T>& b) {
	if (a.size() != b.size()) return false;

	for (auto t : a) {
		bool has_similar = false;
		for (auto t2 : b) if (t == t2) {
			has_similar = true;
			break;
		}

		if (!has_similar) return false;
	}

	return true;
}

bool qmc_algorithm_solver::minterm::operator==(const minterm& b) {
	return are_vectors_similar(components, b.components);
}

bool qmc_algorithm_solver::minterm::contains_component(long long c) {
	for (auto t : components) if (t == c) return true;
	return false;
}

template<typename T>
void remove_duplicates(vector<T>& minterms) {
	for (int i = 0; i < minterms.size(); i++) {
		for (int j = i + 1; j < minterms.size(); j++) {
			if (minterms.at(i) == minterms.at(j))
			{
				minterms.erase(minterms.begin() + j);
				i--;
				break;
			}
		}
	}
}

void qmc_algorithm_solver::merge_minterms() {
	bool has_merges = true;
	while (has_merges)
	{
		has_merges = false;
		vector<minterm>* next_lvl_minterms = new vector<minterm>();
		vector<bool>* had_merges = new vector<bool>(minterms.size(), false);

		for (int i = 0; i < minterms.size(); i++) {
			if (minterms[i].is_final_form) continue;

			for (int j = i + 1; j < minterms.size(); j++) {
				minterm tmp(minterms[i]);

				if (tmp.try_merge_minterms(minterms[j], var_num)) {
					has_merges = true;
					had_merges->at(i) = true;
					had_merges->at(j) = true;
					next_lvl_minterms->push_back(tmp);
				}
			}
		}

		remove_duplicates(*next_lvl_minterms);

		for (int i = 0; i < minterms.size(); i++) if (!had_merges->at(i)) {
			minterms[i].is_final_form = true;
			next_lvl_minterms->push_back(minterms[i]);
		}

		if (has_merges) {
			minterms.clear();
			for (minterm m : *next_lvl_minterms) minterms.push_back(m);
		}

		delete next_lvl_minterms;
		delete had_merges;
	}
}

void qmc_algorithm_solver::get_essential_implicants() {
	vector<minterm>* essential = new vector<minterm>();
	vector<long long>* covered = new vector<long long>();

	for (auto t : total_comps) {
		int total_count = 0;
		int index = 0;
		for (int i = 0; i < minterms.size(); i++) {
			if (minterms[i].contains_component(t)) {
				total_count++;
				index = i;
			}
		}
		if (total_count == 1)
		{
			essential->push_back(minterms[index]);
			for (auto i : minterms[index].components) covered->push_back(i);
			minterms.erase(minterms.begin() + index);
		}
		int h = 0;
		h++;
	}

	remove_duplicates(*covered);
	if (!are_vectors_similar(*covered, total_comps)) {
		vector<long long>* n_cov = new vector<long long>(total_comps);
		for (int i = 0; i < n_cov->size(); i++) {
			for (auto t : *covered) {
				if (n_cov->at(i) == t) {
					n_cov->erase(n_cov->begin() + i); 
					i--;
					break;
				}
			}
		}

		auto t = petricks_method(*n_cov);
		for (auto tmp : *t) essential->push_back(tmp);
		delete t;
	}

	minterms.clear();
	for (auto m : *essential) minterms.push_back(m);

	delete essential;
	delete covered;
}

vector<qmc_algorithm_solver::minterm>* qmc_algorithm_solver::petricks_method(vector<long long>& not_covered) {
	vector<vector<int>>* candidates = new vector<vector<int>>(not_covered.size());
	for (int j = 0; j < not_covered.size(); j++) {
		for (int i = 0; i < minterms.size(); i++) {
			if (minterms[i].contains_component(not_covered[j])) candidates->at(j).push_back(i);
		}
	}

	vector<vector<int>>* sum_of_prod_form = new vector<vector<int>>();
	for (auto t : candidates->at(0)) sum_of_prod_form->push_back(vector<int>(1, t));

	if (candidates->size() > 1) {
		for (int i = 1; i < candidates->size(); i++) {
			int old_size = sum_of_prod_form->size();
			for (int j = 0; j < candidates->at(i).size() - 1; j++) {
				for (int g = 0; g < old_size; g++) sum_of_prod_form->push_back(vector<int>(sum_of_prod_form->at(g)));
			}

			for (int j = 0; j < candidates->at(i).size(); j++) {
				for (int g = 0; g < old_size; g++) 
					sum_of_prod_form->at(g + j * old_size).push_back(candidates->at(i)[j]);
			}
		}

		for (auto& t : *sum_of_prod_form) remove_duplicates(t);
	}

	delete candidates;

	vector<int>* result = nullptr;
	int minimal = INT_MAX;
	for (auto& t : *sum_of_prod_form) {
		if (t.size() < minimal) {
			result = &t;
			minimal = t.size();
		}
	}

	vector<minterm>* res = new vector<minterm>();
	for (int i = 0; i < result->size(); i++) res->push_back(minterms[result->at(i)]);

	delete sum_of_prod_form;

	return res;
}

Component* qmc_algorithm_solver::get_result(vector<char>* vars) {
	pair<int, int> tmp(0, 0);
	Component* root = new Function(FunctionType::Root, tmp);
	Component* tr_or = new Function(FunctionType::Or, tmp);
	root->AddChild(tr_or);
	for (auto t : minterms) {
		Component* c = new Function(FunctionType::And, tmp);
		int i = 0;
		long long minterm_vars = t.get_content();
		while (i++ < var_num)
		{
			if (!t.dont_care[var_num - i]) {
				bool non_inverted = (bool)(minterm_vars % 2);
				Component* var = new Variable(vars->at(var_num - i));
				var->is_inverted = !non_inverted;
				c->AddChild(var, 0);
			}
			minterm_vars >>= 1;
		}
		tr_or->AddChild(c);
	}

	return root;
}