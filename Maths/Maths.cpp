
#include "Maths.h";

using namespace std;
using namespace Tools;

typedef vector<Component*>::iterator comp_iterator;

void Function::AddChild(Component* comps, int pos) {
    if (pos == INT_MAX) { childs.push_back(comps); return; }
    vector<Component*>::iterator it = childs.begin();
    if (pos > 0) {
        advance(it, pos);
        childs.insert(it, comps);
    }
    else childs.push_back(comps);
}

vector<char*>& Component::Lexer(string equation) {
    auto result = new vector<char*>();
    int i = 0;
    while ((i = equation.find(' ')) != -1) equation.erase(i, 1);

    i = 0;

    string buf = "";
    while (i < equation.size()) {
        if (IsOperator(equation[i])) {
            if (buf != "") {
                result->push_back(string_conv(buf));
                buf = "";
            }
            result->push_back(string_conv(equation.substr(i, 1)));
        }
        //else if(buf == "sin(")
        else {
            if (!(buf[0] > '0' && buf[0] < '9' || buf.size() == 0)) {
                result->push_back(string_conv(buf));
                buf = "";
                result->push_back(const_cast<char*>("*"));
            }
            buf += equation[i];
        }
        i++;
    }

    if(buf != "") result->push_back(string_conv(buf));

    return *result;
}

//void Component::Simplify() {
//    for (Component* c : *this->oper->childs) 
//        if (c->GetType() == CompType::Op && (c->oper->op == FunctionType::Sum || c->oper->op == FunctionType::Mult)) 
//            c->Simplify();
//
//    for (int i = 0; i < this->oper->childs->size(); i++) {
//        Component* c = (*this->oper->childs)[i];
//        if (c->GetType() == CompType::Op && c->oper->op == this->oper->op) {
//            for (Component* c2 : *c->oper->childs) {
//                this->AddChild(c2);
//                c2->Simplify();
//            }
//            this->oper->childs->erase(this->oper->childs->begin() + i);
//            i--;
//        }
//    }
//}

Component* Component::Parse(vector<char*>& to_parse) {
    auto st = new stack<Component*>();
    Component* cmp = nullptr, *prev = nullptr;

    auto prior = PriorityParser(to_parse);

    int pre, next, old_prior = INT_MAX;
    bool inverted = false;
    for (int i = 0; i < prior.size(); i++) {
        pair<int, int>* pr = prior[i];

        if (i == 0) pre = INT_MIN;
        else pre = prior[i - 1]->first;

        if (i == prior.size() - 1) next = INT_MIN;
        else next = prior[i + 1]->first;

        InsertOperation(pr, to_parse, st, cmp, prev, old_prior, inverted);

        if (pre < cmp->get_prior() && pr->second - 1 >= 0 && !IsOperator(to_parse[pr->second - 1]))
        {
            Component* child = GetValuedComponent(to_parse[pr->second - 1]); //Стоящий до оператора операнд не может быть инвертирован
            //if (inverted) child->is_inverted = true;
            cmp->AddChild(child, 0);
        }
        if (next <= cmp->get_prior() && !IsOperator(to_parse[pr->second + 1]))
        {
            Component* child = GetValuedComponent(to_parse[pr->second + 1]);
            if (inverted) child->is_inverted = true;
            cmp->AddChild(child);
        }

        prev = cmp;
        old_prior = prev->get_prior();
    }

    while (!st->empty())
    {
        cmp = st->top(); st->pop();
    }

    delete st;

    return cmp;
}

Component* Component::GetValuedComponent(char* to_parse) {
    int i = 0;
    while (to_parse[i] != '\0') if (to_parse[i] < '0' || to_parse[i++] > '9') return new Variable(string(to_parse));
    return new Value(stoi(string(to_parse)));
}

void Component::InsertOperation(pair<int, int>* pr, vector<char*>& to_parse, stack<Component*>*& st, Component*& cmp, Component*& prev, int old_prior, bool& inverted) {
    if (pr->first == old_prior) {
        if (IsInvertOperator(to_parse[pr->second])) inverted = true;
        else inverted = false;
    }
    else if (pr->first > old_prior) {
        cmp = CreateOperation(to_parse[pr->second], pr->first, inverted);
        prev->AddChild(cmp);
        st->push(cmp);
    }
    else {
        Component* next = nullptr;
        while (!st->empty() && pr->first < old_prior)
        {
            next = prev;
            prev = st->top(); st->pop();
            old_prior = prev->get_prior();
        }
        if (pr->first < old_prior) {
            cmp = CreateOperation(to_parse[pr->second], pr->first, inverted);
            if (prev) cmp->AddChild(prev);
            st->push(cmp);
        }
        else if (pr->first > old_prior) {
            cmp = CreateOperation(to_parse[pr->second], pr->first, inverted);
            st->push(prev);
            vector<Component*>& v = prev->get_childs();
            comp_iterator it = v.begin();
            while (*it != next) it++;
            v.erase(it);
            prev->AddChild(cmp);
            st->push(cmp);
            cmp->AddChild(next);
        }
        else {
            cmp = prev;
            st->push(prev);
            if (IsInvertOperator(to_parse[pr->second])) inverted = true;
            else inverted = false;
        }
    }
}

Component* Component::CreateOperation(char* elem, int prior, bool& invert) {
    Component* cmp = NULL;
    switch (elem[0])
    {
    case '+':
        cmp = new Function(FunctionType::Sum, prior, invert);
        invert = false;
        break;
    case '-':
        cmp = new Function(FunctionType::Sum, prior, invert);
        invert = true;
        break;
    case '*':
        cmp = new Function(FunctionType::Mult, prior, invert);
        invert = false;
        break;
    case '/':
        cmp = new Function(FunctionType::Mult, prior, invert);
        invert = true;
        break;
    case '^':
        cmp = new Function(FunctionType::Pow, prior, invert);
        invert = false;
        break;
    default:
        throw std::invalid_argument("Несуществующий оператор");
    }
    return cmp;
}

inline bool Component::IsInvertOperator(char* c) noexcept { return c[1] == '\0' && (c[0] == '-' || c[0] == '/'); };

inline bool Component::IsOperator(char* s) noexcept { return s[1] == '\0' && (s[0] == '+' || s[0] == '-' || s[0] == '/' || s[0] == '*' || s[0] == '^' || s[0] == '(' || s[0] == ')'); }

inline bool Component::IsOperator(char c) noexcept { return c == '+' || c == '-' || c == '/' || c == '*' || c == '^' || c == '(' || c == ')'; }

vector<pair<int, int>*>& Component::PriorityParser(vector<char*>& to_parse) {
    auto priorities = new vector<pair<int, int>*>();
    int prev_ptr = 0, cur_ptr = 0, priority_lvl = 0, pow_pr = 0;
    while (prev_ptr < to_parse.size()) {
        if (cur_ptr < to_parse.size() && !IsOperator(to_parse[cur_ptr])) cur_ptr++;
        if (cur_ptr == to_parse.size()) break;

        switch (to_parse[cur_ptr][0])
        {
        case '+':
        case '-':
            priorities->push_back(new pair<int, int>(priority_lvl * to_parse.size(), cur_ptr));
            break;
        case '/':
        case '*':
            priorities->push_back(new pair<int, int>(2 + priority_lvl * to_parse.size(), cur_ptr));
            break;
        case '^':
            priorities->push_back(new pair<int, int>(4 + pow_pr + priority_lvl * to_parse.size(), cur_ptr));
            pow_pr++;
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
    return *priorities;
}

void Function::test_print_tree(int tab) {
    for (int i = 0; i < tab; i++) cout << '\t';
    string out = (is_inverted) ? "(Inv)" : "";
    switch (op)
    {
    case FunctionType::Sum:
        out += "+";
        break;
    case FunctionType::Mult:
        out += "*";
        break;
    case FunctionType::Pow:
        out += "^";
    default:
        break;
    }
    cout << out << '\n';
    for (Component* c : childs) c->test_print_tree(tab + 1);
}

void Variable::test_print_tree(int tab) {
    for (int i = 0; i < tab; i++) cout << '\t';
    string out = (is_inverted) ? "(Inv)" : "";
    cout << out << name << '\n';
}

void Value::test_print_tree(int tab) {
    for (int i = 0; i < tab; i++) cout << '\t';
    string out = (is_inverted) ? "(Inv)" : "";
    cout << out << value << '\n';
}