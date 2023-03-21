#pragma once

#include <iostream>

using namespace std;

struct Variable;
struct Value;
struct Function;

enum class FunctionType {
    Sum = 0, Subtr, Neg, Brack, Mult, Div, Inv, Pow, Log
};

enum class CompType {
    Var, Val, Op
};

bool Comparer(pair<int, int> a, pair<int, int> b) { return a.first < b.first; };

union Component
{
public:
    Variable* var;
    Value* val;
    Function* oper;

    Component() { };

    Component(string s);
    Component(double d);
    Component(FunctionType o, int p);

    inline CompType GetType() noexcept;
    void AddChild(Component* comps);

    static Component* Parse(vector<char*>* to_parse);

private:
    static Component* GetValuedComponent(char* to_parse);
    
    static inline bool IsOperator(char* s) noexcept;
    static FunctionType GetFunctionType(char* s);
    static void PriorityParser(vector<char*>* to_parse, vector<pair<int, int>*>* priorities);

    static Component* RecursiveParsing(vector<char*>* to_parse, pair<int, int>* priorities);

    static Component* CreateOperation(char* elem, int prior, stack<Component*>** st);

    static inline bool IsInvertOperator(char* c) noexcept;

    static void InsertOperation(pair<int, int>* pr, vector<char*>* to_parse, stack<Component*>** st, Component** cmp, Component** prev, int old_prior);
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