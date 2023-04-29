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
//}
