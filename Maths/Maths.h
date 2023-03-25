#pragma once

#include <iostream>
#include <iostream>
#include <stdexcept>
#include <stack>
#include <vector>
#include <string>
#include "Tools.h"

using namespace std;

struct priority {
public:
    int global_pr;
    int local_pr;
    int ptr;
    priority(int gp, int lp, int pt) { global_pr = gp; local_pr = lp, ptr = pt; }
};

enum class FunctionType {
    Sum = 0, Mult, Pow
};

enum class CompType {
    Var, Val, Op
};

class Component abstract
{
public:
    bool is_inverted = false;

    CompType GetType() { return type; }

    static vector<char*>& Lexer(string equation);
    static Component* Parse(vector<char*>& to_parse);

    virtual void test_print_tree(int tab = 0) {};

    virtual int get_prior() { return 0; } //Виртуальные функции для Function
    virtual FunctionType get_func_type() { return (FunctionType)-1; }
    virtual void set_func_type(FunctionType t) { return; }
    virtual vector<Component*>& get_childs() { throw new std::invalid_argument("Попытка получить потомков не операции"); }
    virtual void AddChild(Component* comps, int pos = INT_MAX) { return; }

    void Simplify();

private:
    static Component* GetValuedComponent(char* to_parse);
    
    static vector<pair<int, int>*>& PriorityParser(vector<char*>& to_parse);
    static Component* CreateOperation(char* elem, int prior, bool& invert);

    static inline bool IsOperator(char* s) noexcept;
    static inline bool IsOperator(char c) noexcept;
    static inline bool IsInvertOperator(char* c) noexcept;

    static void InsertOperation(pair<int, int>* pr, vector<char*>& to_parse, stack<Component*>*& st, Component*& cmp, Component*& prev, int old_prior, bool& inverted);
protected:
    CompType type;
};

class Variable : public Component
{
public:
    char name[5];

    virtual void test_print_tree(int tab = 0);
    //virtual CompType GetType() { return CompType::Var; }

    Variable(string s) { strcpy_s(name, s.substr(0, 4).c_str()); type = CompType::Var; }
    Variable(char* s) { strcpy_s(name, 4, s); type = CompType::Var; }
};

class Value : public Component
{
public:
    double value;

    virtual void test_print_tree(int tab = 0);
    //virtual CompType GetType() { return CompType::Val; }

    Value(double d) { value = d; type = CompType::Val; }
};

class Function : public Component
{
public:
    FunctionType op;
    vector<Component*> childs = vector<Component*>();
    int prior;

    virtual void test_print_tree(int tab = 0);
    //virtual CompType GetType() { return CompType::Op; }

    Function(FunctionType o, int p, bool inv = false) {
        op = o;
        prior = p;
        type = CompType::Op;
        is_inverted = inv;
    }

    virtual int get_prior() { return prior; } 
    virtual FunctionType get_func_type() { return op; }
    virtual void set_func_type(FunctionType t) { op = t; }
    virtual vector<Component*>& get_childs() { return childs; }
    virtual void AddChild(Component* comps, int pos = INT_MAX);

private:
};