#include "Analyzer.h"




//Лямбда-функция для автоматической сортировки списков кандидатов
auto vec_cmp = [](const vector<Component*>* a, const vector<Component*>* b) { return a->size() < b->size(); };

auto test_cmp = [](const int& a, const int& b) { return a < b; };

vector<Component*>& intersection(vector<Component*> nums1, vector<Component*> nums2) {
	vector<Component*>* result = new vector<Component*>();
	int l = 0, r = 0;
	while (l < nums1.size() && r < nums2.size()) {
		Component* left = nums1[l], *right = nums2[r];
		if (*left == *right) {
			result->push_back(right);
			while (l < nums1.size() && *nums1[l] == *left) l++;
			while (r < nums2.size() && *nums2[r] == *right) r++;
			continue;
		}
		if (left < right) {
			while (l < nums1.size() && *nums1[l] == *left) l++;
		}
		else while (r < nums2.size() && *nums2[r] == *right) r++;
	}
	return *result;
}

vector<Component*>* get_duplicates(vector<Component*>* vec, int dup_num, int inv_dup_num) {
	for (int i = 0; i < vec->size(); i++) {
		if ((*vec)[i]->duplicates < dup_num || (*vec)[i]->inv_dups < inv_dup_num) {
			vec->erase(vec->begin() + i);
			i--;
		}
	}
	return vec;
}

vector<Component*>* get_duplicates(Component* vec, int dup_num, int inv_dup_num) {
	Component* cmp(vec);
	cmp->FindDuplicates();
	vector<Component*>* tmp = new vector<Component*>(cmp->get_childs());
	return get_duplicates(tmp, dup_num, inv_dup_num);
}

EquationMatcher::EquationMatcher(string eq) {
	string s;
	equation = Component::Parse(Component::Lexer(eq, s));
	equation->Simplify();
	equation = equation->get_childs()[0];
}

bool EquationMatcher::Match(Component* first, Component* second) {
	bool result = false;
	map<char, vector<Component*>*>::iterator it;
	vector<Component*>::iterator it2;
	switch (first->GetType())
	{
	case CompType::Op:
		if (second->GetType() != CompType::Op || second->is_inverted != first->is_inverted || first->get_func_type() != second->get_func_type()) return false;

		for (auto c : first->get_childs()) {
			result = false;
			if (c->GetType() == CompType::Var) {
				result = Match(c, second);
				occurs[c->get_name()] = true;
			}
			else {
				for (auto c2 : second->get_childs()) {
					bool match = Match(c, c2);
					if (match) result = true; //Хотя бы одно совпадение найдено - продолжаем поиск других возможных совпадений
				}
			}
			if (!result) break; //Совпадение не найдено - не совпадают
		}
		return result;
	case CompType::Val:
		return first->weak_equal(*second);
	case CompType::Var:
		vector<Component*>* candidates;
		if (first->duplicates > 0) { //Если в шаблоне на одном уровне несколько раз упоминается одна переменная - ищем дубликаты
			candidates = get_duplicates(second, first->duplicates, first->inv_dups);
			if (candidates->size() == 0) return false;
		}
		else candidates = new vector<Component*>(second->get_childs()); //Иначе кандидаты - все потомки второго узла

		//Если переменная ещё не упоминалась, то её кандидатами могут являться все потомки узла
		if (occurs.find(first->get_name()) == occurs.end()) vars[first->get_name()] = candidates; 
		else {
			auto& temp = *vars[first->get_name()];
			//Если переменная в шаблоне инвертирована - инвертируются и входные данные
			for (auto c2 : temp) c2->is_inverted ^= first->is_inverted;
			//Находим пересечение массива кандидатов с имеющимися кандидатами
			temp = intersection(*candidates, temp); 
			//Восстанавливаем оригинальное выражение (XOR отменяет сам себя)
			for (auto c2 : temp) c2->is_inverted ^= first->is_inverted;

			if (temp.size() == 0) return false;
		}
		return true;
	}

	return false;
}

bool EquationMatcher::is_in_backwards_candidates(Component* c) {
	int count = 0, inv_count = 0;
	for (int i = 0; i < backward_candidates.size(); i++) {
		if (backward_candidates[i].first->weak_equal(*c)) {
			if (backward_candidates[i].first->is_inverted == c->is_inverted) count++;
			else inv_count++;
		}

		if (count > c->duplicates && !c->is_inverted || inv_count > c->inv_dups && c->is_inverted) 
			return true;
	}
	return false;
}

bool EquationMatcher::recursive_compare(map<char, vector<Component*>*>::iterator iter) {
	bool result = false;
	auto elem = iter->second;
	for (auto cmp : *elem) {
		if (!is_in_backwards_candidates(cmp)) {
			backward_candidates.push_back(pair<Component*, char>(cmp, iter->first));
			if (++iter == vars.end()) return true;
			else result = recursive_compare(iter);

			if (result) return true; //Проблем дальше нет - выходим, иначе продолжаем подбор
			backward_candidates.pop_back(); //Откат изменений
			iter--;
		}
	}
	return false;
}

map<char, Component*>* EquationMatcher::FindMatch(Component* cmp) {
	bool res = Match(equation, cmp);
	if (!res) return nullptr;

	auto sorted = multimap<vector<Component*>*, char, decltype(vec_cmp)>(vec_cmp);
	for (auto c : vars) sorted.insert(pair<vector<Component*>*, char>(c.second, c.first));

	vars.clear();
	for (auto c : sorted) vars.insert(pair<char, vector<Component*>*>(c.second, c.first));

	backward_candidates = vector<pair<Component*, char>>();
	auto result = new map<char, Component*>();
	recursive_compare(vars.begin());

	vector<int> a{ 5, 4, 3, 2, 5 };
	std::less_equal<int> cmpr;
	quicksort(a, test_cmp);

	swap_move(backward_candidates[0].first, backward_candidates[1].first);
	int i = 0;
	i++;
}