
#include <iostream>
#include "Maths.h"
#include "Analyzer.h"

using namespace std;
//using namespace Tools;

int main()
{
    setlocale(LC_ALL, "Russian");
    string input = "a|(!a&b)", error = "";
    //cin >> input;
    vector<char*>& cmp = Component::Lexer(input, error);
    if (error == "") {
        Component* comp = Component::Parse(cmp);
        EquationMatcher eq = EquationMatcher("!A|B");

        comp->Simplify();
        comp->test_print_tree();

        eq.FindMatch(comp->get_childs()[0]);
        delete comp;
    }
    else cout << error;

    int i = 0;
    i++;
}