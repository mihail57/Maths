#include "Tools.h"


//namespace Tools {
    inline char* Tools::string_conv(string s) {
        return const_cast<char*>(_strdup(s.c_str()));
    }

    vector<char*>& Tools::string_split(string s, const char* delimeter) {
        vector<char*>* result = new vector<char*>();
        char* str = string_conv(s), * buf = nullptr, * temp = str;
        int i = 0;

        temp = strtok_s(str, delimeter, &buf);
        while (temp != NULL)
        {
            result->push_back(_strdup(temp));
            temp = strtok_s(NULL, delimeter, &buf);
        }
        return *result;
    }

    
    
    int Tools::gcd(const int& a, const int& b) {
        int mn = min(a, b), mx = max(a, b);
        while (mn > 0) {
            mx %= mn;
            swap_move(mx, mn);
        }
        return mx;
    }

    bool Tools::_xor(bool a, bool b) { return a ^ b; }
    bool Tools::_and(bool a, bool b) { return a & b; }
    bool Tools::_or(bool a, bool b) { return a | b; }
    bool Tools::_equ(bool a, bool b) { return !_xor(a, b); }
    bool Tools::_impl(bool a, bool b) { return !_or(!a, b); }

    long long get_number(vector<bool>& vec) {
        long long result = 0;
        for (int i = 0; i < vec.size() - 1; i++) { result <<= 1; result += (int)vec[i]; }
        return result;
    }

    void Tools::print_truth_table(vector<vector<bool>>* table, vector<char>* vars) {
        /*for (int i = 0; i < vars->size() - 1; i++) {
            int min = i;
            for (int j = i + 1; j < vars->size(); j++) {
                if (vars->at(j) < vars->at(min)) min = j;
            }

            if (min != i) {
                swap(vars->at(i), vars->at(min));
                for (auto& t : *table) swap(t[i], t[min]);
            }
        }

        for (int i = 0; i < table->size(); i++) {
            int min = i;
            for (int j = i + 1; j < table->size(); j++) {
                if (get_number(table->at(j)) < get_number(table->at(min))) min = j;
            }
            
            if(min != i) swap(table->at(i), table->at(min));
        }*/

        for (int i = 0; i < vars->size(); i++) cout << vars->at(i) << ' ';
        cout << "F\n";
        for (auto t : *table) {
            for (bool b : t) cout << (int)b << ' ';
            cout << '\n';
        }
    }
//}
