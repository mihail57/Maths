#pragma once

#ifndef MATHS_H
#define MATHS_H

#include "Tools.h"
#include <iostream>
#include <stdexcept>
#include <vector>
#include <string>
#include <queue>
#include <map>
#include <stack>

using namespace std;
using namespace Tools;

struct priority {
public:
    int global_pr;
    int local_pr;
    int ptr;

    priority(int gp, int lp, int pt) { global_pr = gp; local_pr = lp, ptr = pt; }

    inline pair<int, int>& to_pair();
};

enum class FunctionType {
    Root = -1, Or, And, Inv, Xor, Equ, Imp
};

enum class CompType {
    Var, Val, Op
};

class Component abstract
{
public:
    Component* copy();

    bool is_inverted = false;

    CompType GetType() { return type; }

    static vector<char*>& Lexer(string equation, string& error_type);
    static Component* Parse(vector<char*>& to_parse);

    virtual void test_print_tree(int tab = 0) {};

    virtual pair<int, int>& get_prior() { auto p = new pair<int, int>(0, 0); return *p; } //Виртуальные функции для Function
    virtual void set_prior(pair<int, int>& prior) { return; }
    virtual FunctionType get_func_type() { return (FunctionType)-1; }
    virtual void set_func_type(FunctionType t) { return; }
    virtual vector<Component*>& get_childs() { throw new std::invalid_argument("Попытка получить потомков не операции"); }
    virtual void set_childs(vector<Component*>& c) { return; }
    virtual void AddChild(Component* comps, int pos = INT_MAX) { return; }

    vector<vector<bool>>* get_truth_table(vector<char>* vars_container);

    static vector<priority*>& PriorityParser(vector<char*>& to_parse);

    virtual char get_name() { return 0; } //Виртуальные функции для Variable
    virtual void set_name(char c) { return; } 

    virtual bool get_value() { return 0; } //Виртуальные функции для Value
    virtual void set_value(bool b) { return; }

    inline bool operator== (Component& b) { return weak_equal(b) && is_inverted == b.is_inverted; }
    inline bool operator!= (Component& b) { return !(*this == b); }
    inline virtual bool weak_equal(Component& b) = 0;

    void Simplify();
    static Component* SimpifyVals(Component*& cmp);


    virtual void print_equation(int old_prior = 0) = 0;

private:
    static Component* GetValuedComponent(char* to_parse);
    
    static Component* CreateOperation(char* elem, pair<int, int>& prior, bool& invert);

    static inline bool IsOperator(char* s) noexcept;
    static inline bool IsOperator(char c) noexcept;
    //static inline bool IsInvertOperator(char* c) noexcept;

    static void InsertOperation(priority& pr, vector<char*>& to_parse, stack<Component*>*& st, Component*& cmp, Component*& prev, pair<int, int>& old_prior, bool& inverted);
protected:
    CompType type;
};

class Variable : public Component
{
public:
    char name;

    inline virtual bool weak_equal(Component& b) { 
        return this->GetType() == b.GetType() && name == b.get_name(); }

    virtual char get_name() { return name; }
    virtual void set_name(char c) { name = c; }

    virtual void test_print_tree(int tab = 0);

    virtual void print_equation(int old_priority = 0);
    //virtual CompType GetType() { return CompType::Var; }

    Variable(char c) { name = c; type = CompType::Var; }
};

class Value : public Component
{
public:
    bool value;

    virtual bool get_value() { return value; }
    virtual void set_value(bool b) { value = b; }

    inline virtual bool weak_equal (Component& b) { return this->GetType() == b.GetType() && value == b.get_value(); }

    virtual void test_print_tree(int tab = 0);
    //virtual CompType GetType() { return CompType::Val; }
    virtual void print_equation(int old_priority = 0);

    Value(bool d) { value = d; type = CompType::Val; }
};

class Function : public Component
{
public:
    FunctionType op;
    vector<Component*> childs = vector<Component*>();
    pair<int, int> prior;

    inline virtual bool weak_equal (Component& b) { 
        if (this->GetType() != b.GetType() || op != b.get_func_type()) return false;
        auto b_childs = b.get_childs();
        if (childs.size() != b_childs.size()) return false;
        for (int i = 0; i < childs.size(); i++) if (!childs[i]->weak_equal(*b_childs[i])) return false;
        return true;
    }

    virtual void test_print_tree(int tab = 0);
    //virtual CompType GetType() { return CompType::Op; }
    virtual void print_equation(int old_priority = 0);

    Function(FunctionType o, pair<int, int>& p, bool inv = false) {
        op = o;
        prior = p;
        type = CompType::Op;
        is_inverted = inv;
    }

    virtual pair<int, int>& get_prior() { return prior; }
    virtual void set_prior(pair<int, int>& prior) { this->prior = prior; }
    virtual FunctionType get_func_type() { return op; }
    virtual void set_func_type(FunctionType t) { op = t; }
    virtual vector<Component*>& get_childs() { return childs; }
    virtual void set_childs(vector<Component*>& c) { childs = c; }
    virtual void AddChild(Component* comps, int pos = INT_MAX);

private:
};



#endif