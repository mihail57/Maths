
#include <iostream>
#include <stdexcept>
#include <stack>
#include <vector>
#include <string>
#include "Maths.h";

using namespace std;

Component::Component(string s) { var = new Variable(s); }
Component::Component(double d) { val = new Value(d); }
Component::Component(FunctionType o, int p) { oper = new Function(o, p); }

inline CompType Component::GetType() noexcept { 
    if (var) return var->type;
    else return (CompType)-1;
}

void Component::AddChild(Component* comps) {
    if (this->GetType() != CompType::Op) throw std::invalid_argument("Попытка добавить следующие узлы НЕ операции");
    this->oper->childs->push_back(comps);
}

Component* Component::Parse(vector<char*>* to_parse) {
    auto st = new stack<Component*>();
    Component* cmp = nullptr, *prev = nullptr;

    auto prior = new vector<pair<int, int>*>();
    PriorityParser(to_parse, prior);

    int old_prior = INT_MAX;
    for (pair<int, int>* pr : *prior) {
        InsertOperation(pr, to_parse, &st, &cmp, &prev, old_prior);

        if(prev != cmp) st->push(cmp);
        prev = cmp;
        old_prior = pr->first;
    }

    while (!st->empty())
    {
        cmp = st->top(); st->pop();
    }

    delete st;
    delete prior;

    return cmp;
}

Component* Component::GetValuedComponent(char* to_parse) {
    int i = 0;
    while (to_parse[i] != '\0') if (to_parse[++i] < '0' || to_parse[i] > '9') return new Component(string(to_parse));
    return new Component(stoi(string(to_parse)));
}

void Component::InsertOperation(pair<int, int>* pr, vector<char*>* to_parse, stack<Component*>** st, Component** cmp, Component** prev, int old_prior) {
    if (old_prior == pr->first) {
        if (IsInvertOperator((*to_parse)[pr->second])) {
            *cmp = new Component(FunctionType::Inv, old_prior + 1);
            (*prev)->AddChild(*cmp);
            (*st)->push(*prev);
        }
    }
    else {
        if (old_prior > pr->first) {
            if ((*st)->empty()) {
                *cmp = new Component(GetFunctionType((*to_parse)[pr->second]), pr->first);
                if(*prev) (*cmp)->AddChild(*prev);
            }
            else {
                while (!(*st)->empty() && old_prior > pr->first) {
                    *prev = (*st)->top();
                    (*st)->pop();
                    old_prior = (*prev)->oper->prior;
                }
                InsertOperation(pr, to_parse, st, cmp, prev, old_prior);
            }
        }
        else {
            *cmp = new Component(GetFunctionType((*to_parse)[pr->second]), pr->first);
            (*prev)->AddChild(*cmp);
        }
    }
}

Component* Component::CreateOperation(char* elem, int prior, stack<Component*>** st) {
    Component* cmp = NULL;
    switch (elem[0])
    {
    case '+':
        cmp = new Component(FunctionType::Sum, prior);
        break;
    case '-':
        cmp = new Component(FunctionType::Sum, prior);
        (*st)->push(cmp);
        cmp->AddChild(new Component(FunctionType::Inv, prior + 1));
        cmp = (*cmp->oper->childs)[0];
        break;
    case '*':
        cmp = new Component(FunctionType::Mult, prior);
        break;
    case '/':
        cmp = new Component(FunctionType::Mult, prior);
        (*st)->push(cmp);
        cmp->AddChild(new Component(FunctionType::Inv, prior + 1));
        cmp = (*cmp->oper->childs)[0];
        break;
    default:
        throw std::invalid_argument("Несуществующий оператор");
    }
    return cmp;
}

Component* Component::RecursiveParsing(vector<char*>* to_parse, pair<int, int>* priorities) 
{
    return nullptr;
}

inline bool Component::IsInvertOperator(char* c) noexcept { return c[1] == '\0' && (c[0] == '-' || c[0] == '/'); };

inline bool Component::IsOperator(char* s) noexcept { return s[1] == '\0' && (s[0] == '+' || s[0] == '-' || s[0] == '/' || s[0] == '*' || s[0] == '(' || s[0] == ')'); }

FunctionType Component::GetFunctionType(char* s) {
    switch (s[0])
    {
    case '+':
        return FunctionType::Sum;
    case '-':
        return FunctionType::Subtr;
    case '/':
        return FunctionType::Div;
    case '*':
        return FunctionType::Mult;
    default:
        throw std::invalid_argument("Несуществующий оператор");
    }
}

void Component::PriorityParser(vector<char*>* to_parse, vector<pair<int, int>*>* priorities) {
    int prev_ptr = 0, cur_ptr = 0, priority_lvl = 0;
    while (prev_ptr < to_parse->size()) {
        if (cur_ptr < to_parse->size() && !IsOperator((*to_parse)[cur_ptr])) cur_ptr++;
        if (cur_ptr == to_parse->size()) break;

        switch ((*to_parse)[cur_ptr][0])
        {
        case '+':
        case '-':
            priorities->push_back(new pair<int, int>(priority_lvl * to_parse->size(), cur_ptr));
            break;
        case '/':
        case '*':
            priorities->push_back(new pair<int, int>(2 + priority_lvl * to_parse->size(), cur_ptr));
            break;
        case '(':
            priority_lvl++;
            break;
        case ')':
            priority_lvl--;
            break;
        }

        cur_ptr++;
        prev_ptr = cur_ptr;
    }
}

vector<char*>* string_split(string s, const char* delimeter) {
    vector<char*>* result = new vector<char*>();
    char* str = const_cast<char*>(s.c_str()), *buf = nullptr, *temp = str;
    int i = 0;
    
    temp = strtok_s(str, delimeter, &buf);
    while (temp != NULL) 
    {
#pragma warning(suppress : 4996)
        result->push_back(strdup(temp));
        temp = strtok_s(NULL, delimeter, &buf);
    }
    return result;
}


int main()
{
    setlocale(LC_ALL, "Russian");
    string input = "a * ( b + 2 ) / ( 3 + x ) / 2 - 5";
    vector<char*>* c = string_split(input, " ");

    Component* comp = Component::Parse(c);
    //CompType cc = c->GetType();

    int i = 0;
    i++;
}