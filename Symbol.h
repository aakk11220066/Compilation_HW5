#ifndef HW3_NONTERMINALS_H
#define HW3_NONTERMINALS_H

#include <string>
#include <unordered_map>
#include <list>
#include "hw3_output.hpp"
using std::string;

//ABSTRACT CLASS
class Symbol{
public:
    string name, type;
    int offset = 0;
    virtual ~Symbol() = default;

protected:
    Symbol(const string &name, const string &type);
    Symbol(const string &name, const string &type, int offset);
};

class Variable : public Symbol{
public:
    Variable(const string &name, const string &type); //For Shlomi's use
    Variable(const string &name, const string &type, int offset); //For Akiva's use
    bool operator==(const Variable& other) const{
        return (name==other.name && type==other.type && offset == other.offset);
    }
    virtual ~Variable() = default;
};

class Function : public Symbol{
public:
    std::list<Variable> params;

    Function(const string &name, const string &type);

    //For Akiva's use
    const std::list<Variable>& getParameters() const;

    virtual ~Function() = default;
};


#endif //HW3_NONTERMINALS_H