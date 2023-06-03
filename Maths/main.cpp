
#include <iostream>
#include "Maths.h"
//#include "Analyzer.h"
#include "qmc_algorithm.h"

using namespace std;
//using namespace Tools;

int main()
{
    setlocale(LC_ALL, "Russian");
    cout << "Welcome. Enter logical function to simplify:\n";
    string input = "", error = "";
    cin >> input;
    vector<char*>& cmp = Component::Lexer(input, error);
    if (error == "") {
        Component* comp = Component::Parse(cmp);

        comp->Simplify();
        cout << endl;

        vector<char> vars;
        auto t_t1 = comp->get_truth_table(&vars);
        qmc_algorithm_solver s(t_t1);

        cout << "Result (full disjunctive normal form):\n";
        Component* simplified = s.get_result(&vars);
        vars.clear();
        auto t_t2 = simplified->get_truth_table(&vars);
        simplified->print_equation();
        cout << endl;
        cout << "Truth table:\n";
        Tools::print_truth_table(t_t2, &vars);

        delete comp;
        delete t_t1;
        delete simplified;
        delete t_t2;
    }
    else cout << error;

    int i = 0;
    i++;
}