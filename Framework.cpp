#include <assert.h>
#include "Framework.h"
#include "bp.hpp"
#include <iostream>
#define DEBUG_PRINT(str) std::cout << str << std::endl
extern CodeBuffer& codeManager;

//NOTE: must modify default offset of Variable to correct offset!
Variable& Scope::insert(const Variable &newVar) {
    Variable addedVar = Variable("BAD", "BAD");
    if (newVar.offset < 0) addedVar = Variable(newVar.name, newVar.type, newVar.offset);
    else addedVar = Variable(newVar.name, newVar.type, newVar.offset + (nextOffset++));
    variables.push_back(addedVar);
    return variables.back();
}

Scope::Scope(Scope::ScopeType scopeType, int offset) : scopeType(scopeType), nextOffset(offset){}

Scope::ScopeType Scope::getScopeType() const {
    return scopeType;
}

int Scope::getNextOffset() const {
    return nextOffset;
}

void Framework::insertVariableIntoTopScope(const Variable &newVar) {

    if (contains(newVar.name)) throw Exceptions::AlreadyExistsException(0, newVar.name);
    Variable& addedVar = scopes.top().insert(newVar);
    symbol_table.insert({newVar.name, &addedVar});
}


void Framework::addFunction(const Function &newFunc) {

    if (contains(newFunc.name)) throw Exceptions::AlreadyExistsException(0, newFunc.name);
    assert(newFunc.offset == 0);
    Function funcToAdd = Function(newFunc.name, newFunc.type);
    functions.push_back(funcToAdd);
    Function& addedFunc = functions.back();
    symbol_table.insert({newFunc.name, &addedFunc});
}


Symbol &Framework::operator[](const string &name) {
    try{
        return *(symbol_table.at(name));
    } catch(std::out_of_range&) {
        throw Exceptions::IdentifierDoesNotExistException(0, name);
    }

}

bool Framework::contains(const string &name) {
    return symbol_table.find(name) != symbol_table.end();
}

void Framework::addScope(enum Scope::ScopeType scopeType) {
    scopes.push(Scope(scopeType, scopes.top().getNextOffset()));
}

Scope &Framework::getTopScope() {
    return scopes.top();
}

Framework::Framework() {

    scopes.push(std::move(Scope(Scope::BLOCK, 0)));

    Function printFunction = Function("print",  "VOID");
    printFunction.params.emplace_back(Variable("stringToPrint", "STRING"));
    addFunction(printFunction);
    addParamToLastFunc(printFunction.params.back());

    Function printiFunction = Function("printi", "VOID");
    printiFunction.params.emplace_back(Variable("intToPrint", "INT"));
    addFunction(Function("printi",  "VOID"));
    addParamToLastFunc(printiFunction.params.back());
}

Framework singleton = Framework();
Framework& Framework::getInstance() {
    return singleton;
}

void Framework::popScope() {
    output::endScope();

    for (const Variable& var : scopes.top().variables){
        output::printID(var.name, var.offset, var.type);
        symbol_table.erase(var.name);
    }

    scopes.pop();
}

Framework::~Framework() {
    if (!exitOnError) {
        popScope();

        for (const Function &func : functions) {
            const list<string> argsTypesList = varsListToTypesList(func.getParameters());
            vector<string> argsTypesVector = vector<string>(argsTypesList.begin(), argsTypesList.end());
            output::printID(func.name, func.offset, output::makeFunctionType(func.type, argsTypesVector));
        }
    }
}

void Framework::addParamToLastFunc(const Variable &param) {
    functions.back().params.push_back(param);
}

const Function &Framework::getLastAddedFunction() {
    return functions.back();
}

bool Framework::isFunction(const string &name) {
    for (const Function& func : functions){
        if (func.name == name) return true;
    }
    return false;
}



std::string Framework::freshVarBool() {
    string result = "%v" + to_string(nextRegister++);
    codeManager.emit(result + " = alloca i1" + ", align 4");
    return result;
}

std::string Framework::freshVar(){
    string result = "%v" + to_string(nextRegister++);
    codeManager.emit(result + " = alloca i32" + ", align 4");
    return result;
}

std::string Framework::freshTemp(){
    string result = "%t" + to_string(nextRegister++);
    return result;
}

std::string Framework::freshLabel(const std::string &name) {
    return "label" + to_string(nextLabel++) + "_" + name;
}

std::string Framework::freshString(const string& str) {
    string result = "%str_ptr" + to_string(nextRegister);
    string hardCodedStr = "@.str" + to_string(nextRegister++);
    string finalLength = to_string(str.length() + 2);
    codeManager.emitGlobal(hardCodedStr + " = internal constant [" + finalLength + " x i8] c\"" + str + "\\0A\\00\"");
    codeManager.emit(result + " = getelementptr [" + finalLength + " x i8], [" + finalLength + " x i8]* " + hardCodedStr + ", i32 0, i32 0");
    return result;
}
