#include "Maths.h";

using namespace std;

typedef vector<Component*>::iterator comp_iterator;

inline pair<int, int>& priority::to_pair() {
    auto p = new pair<int, int>(global_pr, local_pr);
    return *p;
}

void Function::AddChild(Component* comps, int pos) {
    if (pos == INT_MAX) { childs.push_back(comps); return; }
    vector<Component*>::iterator it = childs.begin();
    if (pos > 0) {
        advance(it, pos);
        childs.insert(it, comps);
    }
    else childs.push_back(comps);
}

vector<char*>& Component::Lexer(string equation, string& error_type) {
    auto result = new vector<char*>();
    int i = 0;
    while ((i = equation.find(' ')) != -1) equation.erase(i, 1);

    i = 0;

    int bracket_counter = 0;
    string buf = "";
    bool is_last_oper = false;
    while (i < equation.size()) {
        if (IsOperator(equation[i])) {
            if (is_last_oper && equation[i] != '!') { error_type = "Несколько операторов подряд"; return *result; }
            if (buf != "") {
                result->push_back(string_conv(buf));
                buf = "";
            }
            result->push_back(string_conv(equation.substr(i, 1)));
            is_last_oper = true;
        }
        else if (equation[i] == ')' || equation[i] == '(')
        {
            if (equation[i] == ')') bracket_counter--;
            else bracket_counter++;

            if(bracket_counter < 0) { error_type = "Больше закрывающих скобок чем открывающих"; return *result; }

            if (buf != "") result->push_back(string_conv(buf));
            if (equation[i] == '(' && (!(buf[0] > '0' && buf[0] < '9' || buf.size() == 0)) || i > 0 && equation[i - 1] == ')' && equation[i] == '(') result->push_back(const_cast<char*>("&"));
            buf = "";

            result->push_back(string_conv(equation.substr(i, 1)));
        }
        else {
            if (i > 0 && equation[i - 1] == ')' && equation[i] == '(') result->push_back(const_cast<char*>("&"));
            is_last_oper = false;
            if (!(buf[0] > '0' && buf[0] < '9' || buf.size() == 0)) {
                result->push_back(string_conv(buf));
                buf = "";
                result->push_back(const_cast<char*>("&"));
            }
            buf += equation[i];
        }
        i++;
    }

    if(buf != "") result->push_back(string_conv(buf));
    if(bracket_counter > 0) error_type = "Не все скобки закрыты";

    return *result;
}

void Component::Simplify() {
    vector<Component*>& cmps = this->get_childs();
    for (Component* c : cmps) {
        if (c->GetType() == CompType::Op) c->Simplify();
    }

    for (int i = 0; i < cmps.size(); i++) {
        Component* c = cmps[i];
        if (c->GetType() == CompType::Op && (c->get_func_type() == this->get_func_type() || c->get_func_type() == FunctionType::Inv) && c->get_func_type() != FunctionType::Imp) {
            int j = 0;
            for (Component* c2 : c->get_childs()) {
                if (c->get_func_type() == FunctionType::Inv) c2->is_inverted = !c2->is_inverted;
                this->AddChild(c2, i + j++ + 1);
                if (c2->GetType() == CompType::Op) c2->Simplify();
            }
            cmps.erase(cmps.begin() + i);
            i--;
            i += j;
        }
        else if (c->GetType() == CompType::Val && c->is_inverted) {
            c->set_value(c->get_value() ^ true); 
            c->is_inverted = false;
        }
    }
    FindDuplicates();
}

void Component::FindDuplicates() {
    if (GetType() != CompType::Op) return;
    if (get_childs().size() < 2) return;

    vector<Component*> no_dups;
    for (auto c : get_childs())
    {
        bool match = false;
        for (auto c2 : no_dups) {
            if (c->weak_equal(*c2)) {
                if (c->is_inverted == c2->is_inverted) {
                    c2->duplicates += c->duplicates + 1;
                    c2->inv_dups += c->inv_dups;
                }
                else {
                    c2->inv_dups += c->duplicates + 1;
                    c2->duplicates += c->inv_dups;
                }
                match = true;
            }
        }
        if (!match) 
            no_dups.push_back(c);
    }
    set_childs(no_dups);
}

Component* Component::Parse(vector<char*>& to_parse) {
    auto st = new stack<Component*>();
    auto pr = pair<int, int>(-1, 0);
    Component* cmp = new Function(FunctionType::Root, pr), * prev = cmp;
    st->push(cmp);

    auto prior = PriorityParser(to_parse);

    if (prior.size() == 0) { cmp->AddChild(GetValuedComponent(to_parse[0])); return cmp; }

    pair<int, int> pre, next, old_prior = pair<int, int>(-1, 0);
    bool inverted = false, pre_added = false;
    for (int i = 0; i < prior.size(); i++) {
        priority pr = *prior[i];

        if (i == 0) pre = pair<int, int>(INT_MIN, INT_MIN);
        else pre = prior[i - 1]->to_pair();

        if (i == prior.size() - 1) next = pair<int, int>(INT_MIN, INT_MIN);
        else next = prior[i + 1]->to_pair();

        InsertOperation(pr, to_parse, st, cmp, prev, old_prior, inverted);

        if (!pre_added && pr.ptr - 1 >= 0 && !IsOperator(to_parse[pr.ptr - 1]))
        {
            Component* child = GetValuedComponent(to_parse[pr.ptr - 1]); //Стоящий до оператора операнд не может быть инвертирован
            //if (inverted) child->is_inverted = true;
            cmp->AddChild(child, 0);
        }
        pre_added = false;
        if (next < cmp->get_prior() && !IsOperator(to_parse[pr.ptr + 1]))
        {
            Component* child = GetValuedComponent(to_parse[pr.ptr + 1]);
            if (inverted) child->is_inverted = true;
            cmp->AddChild(child);
            pre_added = true;
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
    while (to_parse[i] != '\0') if (to_parse[i] < '0' || to_parse[i++] > '9') return new Variable(to_parse[0]);
    return new Value(stoi(string(to_parse)));
}

void Component::InsertOperation(priority& pr, vector<char*>& to_parse, stack<Component*>*& st, Component*& cmp, Component*& prev, pair<int, int>& old_prior, bool& inverted) {
    //if (*to_parse[pr.ptr] != '!') inverted = false;
    cmp = CreateOperation(to_parse[pr.ptr], pr.to_pair(), inverted);

    if (pr.to_pair() >= old_prior) {
        prev->AddChild(cmp);
        st->push(cmp);
    }
    else {
        Component* next = nullptr;
        while (!st->empty() && pr.to_pair() < old_prior)
        {
            next = prev;
            prev = st->top(); st->pop();
            old_prior = prev->get_prior();
        }
        if (pr.to_pair() < old_prior) {
            if (prev) cmp->AddChild(prev);
            st->push(cmp);
        }
        else {
            st->push(prev);
            vector<Component*>& v = prev->get_childs();
            comp_iterator it = v.begin();
            while (*it != next) it++;
            v.erase(it);
            prev->AddChild(cmp);
            st->push(cmp);
            cmp->AddChild(next);
        }
    }
}

Component* Component::CreateOperation(char* elem, pair<int, int>& prior, bool& invert) {
    Component* cmp = NULL;
    switch (elem[0])
    {
    case '|':
        cmp = new Function(FunctionType::Or, prior, invert);
        //invert = false;
        break;
    case '&':
        cmp = new Function(FunctionType::And, prior, invert);
        //invert = false;
        break;
    case '!':
        cmp = new Function(FunctionType::Inv, prior, invert);
        //invert = true;
        break;
    case '^':
        cmp = new Function(FunctionType::Xor, prior, invert);
        //invert = true;
        break;
    case '=':
        cmp = new Function(FunctionType::Equ, prior, invert);
        //invert = true;
        break;
    case '~':
        cmp = new Function(FunctionType::Imp, prior, invert);
        //invert = true;
        break;
    default:
        throw std::invalid_argument("Несуществующий оператор");
    }
    return cmp;
}

//inline bool Component::IsInvertOperator(char* c) noexcept { return c[1] == '\0' && (c[0] == '-' || c[0] == '/'); };

inline bool Component::IsOperator(char* s) noexcept { return s[1] == '\0' && (s[0] == '&' || s[0] == '|' || s[0] == '!' || s[0] == '^' || s[0] == '=' || s[0] == '~' || s[0] == '(' || s[0] == ')'); }

inline bool Component::IsOperator(char c) noexcept { return c == '&' || c == '|' || c == '!' || c == '^' || c == '=' || c == '~'; }

vector<priority*>& Component::PriorityParser(vector<char*>& to_parse) {
    auto priorities = new vector<priority*>();
    int prev_ptr = 0, cur_ptr = 0, priority_lvl = 0, pow_pr = 0;
    while (prev_ptr < to_parse.size()) {
        if (cur_ptr < to_parse.size() && !IsOperator(to_parse[cur_ptr])) cur_ptr++;
        if (cur_ptr == to_parse.size()) break;

        switch (to_parse[cur_ptr][0])
        {
        case '|':
            priorities->push_back(new priority(priority_lvl, 2, cur_ptr));
            break;
        case '&':
            priorities->push_back(new priority(priority_lvl, 4, cur_ptr));
            break;
        case '!':
            priorities->push_back(new priority(priority_lvl, INT_MAX, cur_ptr));
            break;
        case '^':
            priorities->push_back(new priority(priority_lvl, 3, cur_ptr));
            break;
        case '=':
            priorities->push_back(new priority(priority_lvl, 0, cur_ptr));
            break;
        case '~':
            priorities->push_back(new priority(priority_lvl, 1, cur_ptr));
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
    string out = "";
    if(duplicates == 0 && inv_dups == 0) out = (is_inverted) ? "(Inv)" : "";
    else {
        out = (duplicates > 0) ? "x" + to_string(duplicates) + " " : "";
        out = (inv_dups > 0) ? "x" + to_string(inv_dups) + "(Inv) " : "";
    }
    switch (op)
    {
    case FunctionType::Root:
        out += "Root";
        break;
    case FunctionType::Or:
        out += "|";
        break;
    case FunctionType::And:
        out += "&";
        break;
    case FunctionType::Inv:
        out += "!";
        break;
    case FunctionType::Xor:
        out += "^";
        break;
    case FunctionType::Equ:
        out += "=";
        break;
    case FunctionType::Imp:
        out += "~ (->)";
        break;
    default:
        break;
    }
    cout << out << '\n';
    for (Component* c : childs) c->test_print_tree(tab + 1);
}

void Variable::test_print_tree(int tab) {
    for (int i = 0; i < tab; i++) cout << '\t';
    string out = "";
    if (duplicates == 0 && inv_dups == 0) out = (is_inverted) ? "(Inv)" : "";
    else {
        out = (duplicates > 0) ? "x" + to_string(duplicates) + " " : "";
        out = (inv_dups > 0) ? "x" + to_string(inv_dups) + "(Inv) " : "";
    }
    cout << out << name << '\n';
}

void Value::test_print_tree(int tab) {
    for (int i = 0; i < tab; i++) cout << '\t';
    string out = "";
    if (duplicates == 0 && inv_dups == 0) out = (is_inverted) ? "(Inv)" : "";
    else {
        out = (duplicates > 0) ? "x" + to_string(duplicates) + " " : "";
        out = (inv_dups > 0) ? "x" + to_string(inv_dups) + "(Inv) " : "";
    }
    cout << out << value << '\n';
}