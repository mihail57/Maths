#pragma once

#include <iostream>
#include <iostream>
#include <stdexcept>
#include <stack>
#include <vector>
#include <string>

using namespace std;

struct Variable;
struct Value;
struct Function;

enum class FunctionType {
    Sum = 0, Mult, Inv, Pow, Exp
};

enum class CompType {
    Var, Val, Op
};

union Component
{
public:
    Variable* var;
    Value* val;
    Function* oper;

    Component(string s);
    Component(double d);
    Component(FunctionType o, int p);

    inline CompType GetType() noexcept;
    void AddChild(Component* comps);

    static vector<char*>* Lexer(string equation);
    static Component* Parse(vector<char*>& to_parse);

private:
    static Component* GetValuedComponent(char* to_parse);
    
    static inline bool IsOperator(char* s) noexcept;
    static vector<pair<int, int>*>& PriorityParser(vector<char*>& to_parse);

    static Component* CreateOperation(char* elem, int prior, stack<Component*>*& st);

    static inline bool IsInvertOperator(char* c) noexcept;

    static void InsertOperation(pair<int, int>* pr, vector<char*>& to_parse, stack<Component*>*& st, Component*& cmp, Component*& prev, int old_prior);
};

struct Variable
{
public:
    CompType type;
    char name[5];

    Variable(string s) { strcpy_s(name, s.substr(0, 4).c_str()); type = CompType::Var; }
    Variable(char* s) { strcpy_s(name, 4, s); type = CompType::Var; }
};

struct Value
{
public:
    CompType type;
    double value;

    Value(double d) { value = d; type = CompType::Val; }
};

struct Function
{
public:
    CompType type;
    FunctionType op;
    vector<Component*>* childs = new vector<Component*>();
    int prior;

    Function(FunctionType o, int p) {
        op = o;
        prior = p;
        type = CompType::Op;
    }
};