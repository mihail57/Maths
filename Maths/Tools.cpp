#include "Tools.h"

using namespace std;

namespace Tools {
    vector<char*>& string_split(string s, const char* delimeter) {
        vector<char*>* result = new vector<char*>();
        char* str = const_cast<char*>(_strdup(s.c_str())), * buf = nullptr, * temp = str;
        int i = 0;

        temp = strtok_s(str, delimeter, &buf);
        while (temp != NULL)
        {
            result->push_back(_strdup(temp));
            temp = strtok_s(NULL, delimeter, &buf);
        }
        return *result;
    }

    void test_print_tree(Component* cmp, int tab) {
        for (int i = 0; i < tab; i++) cout << '\t';
        if (cmp->GetType() == CompType::Op) {
            string out = "";
            switch (cmp->oper->op)
            {
            case FunctionType::Sum:
                out = "+";
                break;
            case FunctionType::Mult:
                out = "*";
                break;
            case FunctionType::Inv:
                out = "Inv";
                break;
            default:
                break;
            }
            cout << out << '\n';
            for (Component* c : *cmp->oper->childs) test_print_tree(c, tab + 1);
        }
        else if (cmp->GetType() == CompType::Var) cout << cmp->var->name << '\n';
        else cout << cmp->val->value << '\n';
    }
}
