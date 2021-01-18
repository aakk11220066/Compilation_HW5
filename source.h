#ifndef HW3_SOURCE_H
#define HW3_SOURCE_H

#include <string>
#include <list>
#include <iostream>
using namespace std;

#include "Symbol.h"

void myExit();

class NonTerminal{
public:
    string name;
    string type;
    string place = "";
    list<Variable> namesAndTypes = list<Variable>();
    list<string> arrguments_list = list<string>();
    NonTerminal() = default;
    NonTerminal(const string& name, const string& type) : name(name), type(type) {}
    NonTerminal& operator=(const NonTerminal& other){
        name = other.name;
        type = other.type;
        place = other.place;
        namesAndTypes = other.namesAndTypes;
        arrguments_list = other.arrguments_list;
        return *this;
    };
};

#endif //HW3_SOURCE_H