#include "Tools.h"

namespace Tools {
    inline char* string_conv(string s) {
        return const_cast<char*>(_strdup(s.c_str()));
    }

    vector<char*>& string_split(string s, const char* delimeter) {
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

    template <class T>
    void swap(T& first, T& second) {
        T& buf = second;
        second = first;
        first = buf;
    }
}
