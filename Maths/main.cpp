
#include <iostream>
#include "Maths.h"
#include "Tools.h"

using namespace std;
using namespace Tools;

int main()
{
    setlocale(LC_ALL, "Russian");
    string input = "1/(ab)";

    vector<char*>& cmp = Component::Lexer(input);
    if (cmp.size() > 0) {
        Component* comp = Component::Parse(cmp);
        //CompType cc = c->GetType();

        comp->test_print_tree();
        delete comp;
    }

    int i = 0;
    i++;
}