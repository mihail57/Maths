
#include <iostream>
#include "Maths.h"
#include "Tools.h"

using namespace std;
using namespace Tools;

int main()
{
    setlocale(LC_ALL, "Russian");
    string input = "a * ( b + 2 ) / ( 3 + x ) / ( 2 - x ) - 5";
    vector<char*> c = Tools::string_split(input, " ");
    Component::Lexer(input);

    Component* comp = Component::Parse(c);
    //CompType cc = c->GetType();

    test_print_tree(comp);
    delete comp;

    int i = 0;
    i++;
}