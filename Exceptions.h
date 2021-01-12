#ifndef HW3_EXCEPTIONS_H
#define HW3_EXCEPTIONS_H

#include <bits/exception.h>
#include <string>
#include <list>
#include <vector>

#include "Symbol.h"

using std::string;
using std::list;
using std::vector;

extern list<string> varsListToTypesList(const list<Variable>& varList);

namespace Exceptions {
    class HW3_Exception : std::exception {
    public:
        virtual void printError() = 0;
    };

    class AlreadyExistsException : HW3_Exception {
    public:
        const string name;
        const int lineno;
        AlreadyExistsException(int lineno, const string& name) : name(name), lineno(lineno) {}
        void printError() override{}
    };

    class IdentifierDoesNotExistException : HW3_Exception{
    public:
        const string name;
        const int lineno;
        IdentifierDoesNotExistException(int lineno, const string& name) : name(name), lineno(lineno) {}
        void printError() override{
            output::errorDef(lineno, name);
        }
    };
}
#endif //HW3_EXCEPTIONS_H