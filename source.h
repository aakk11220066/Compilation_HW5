#ifndef HW3_SOURCE_H
#define HW3_SOURCE_H

#include <string>
#include <list>
#include <iostream>
using namespace std;

#include "bp.hpp"
#include "Symbol.h"

typedef vector<pair<int,BranchLabelIndex>> bpatch_list;

void myExit();

class NonTerminal{
public:
    string name;
    string type;
    string place = "";
    list<Variable> namesAndTypes = list<Variable>();
    list<string> arrguments_list = list<string>();
    list<string> places_list = list<string>();
    bpatch_list trueList = bpatch_list();
    bpatch_list falseList = bpatch_list();
    bpatch_list nextList = bpatch_list();
    string label = "";

    NonTerminal() = default;
    NonTerminal(const string& name, const string& type) : name(name), type(type) {}
    NonTerminal& operator=(const NonTerminal& other){
        name = other.name;
        type = other.type;
        place = other.place;
        namesAndTypes = other.namesAndTypes;
        arrguments_list = other.arrguments_list;
        places_list = other.places_list;
        trueList = other.trueList;
        falseList = other.falseList;
        nextList = other.nextList;
        label = other.label;
        return *this;
    };
};

#endif //HW3_SOURCE_H