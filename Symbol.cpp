#include "Symbol.h"

list<string> varsListToTypesList(const list<Variable>& varList){
    list<string> result = list<string>();
    for (const Variable& var : varList){
        result.push_back(var.type);
    }
    return result;
}

Symbol::Symbol(const string &name, const string &type) : name(name), type(type) {}

Symbol::Symbol(const string &name, const string &type, int offset)
        : name(name), type(type), offset(offset){}

Variable::Variable(const string &name, const string &type) : Symbol(name, type) {}

Variable::Variable(const string &name, const string &type, int offset) : Symbol(name, type, offset) {}

Function::Function(const string &name, const string &type) : Symbol(name, type) {}

const std::list<Variable> &Function::getParameters() const {
    return params;
}
