#include "bp.hpp"
#include <vector>
#include <iostream>
#include <sstream>
using namespace std;

bool replace(string& str, const string& from, const string& to, const BranchLabelIndex index);

CodeBuffer::CodeBuffer() : buffer(), globalDefs() {}

CodeBuffer &CodeBuffer::instance() {
	static CodeBuffer inst;//only instance
	return inst;
}

string CodeBuffer::genLabel(){
	std::stringstream label;
	label << "label_";
	label << buffer.size();
	std::string ret(label.str());
	label << ":";
	emit(label.str());
	return ret;
}

int CodeBuffer::emit(const string &s){
    buffer.push_back(s);
	return buffer.size() - 1;
}

void CodeBuffer::bpatch(const vector<pair<int,BranchLabelIndex>>& address_list, const std::string &label){
    for(vector<pair<int,BranchLabelIndex>>::const_iterator i = address_list.begin(); i != address_list.end(); i++){
    	int address = (*i).first;
    	BranchLabelIndex labelIndex = (*i).second;
		replace(buffer[address], "@", "%" + label, labelIndex);
    }
}

void CodeBuffer::printCodeBuffer(){
	for (std::vector<string>::const_iterator it = buffer.begin(); it != buffer.end(); ++it) 
	{
		cout << *it << endl;
    }
}

vector<pair<int,BranchLabelIndex>> CodeBuffer::makelist(pair<int,BranchLabelIndex> item)
{
	vector<pair<int,BranchLabelIndex>> newList;
	newList.push_back(item);
	return newList;
}

vector<pair<int,BranchLabelIndex>> CodeBuffer::merge(const vector<pair<int,BranchLabelIndex>> &l1,const vector<pair<int,BranchLabelIndex>> &l2)
{
	vector<pair<int,BranchLabelIndex>> newList(l1.begin(),l1.end());
	newList.insert(newList.end(),l2.begin(),l2.end());
	return newList;
}

// ******** Methods to handle the global section ********** //
void CodeBuffer::emitGlobal(const std::string& dataLine) 
{
	globalDefs.push_back(dataLine);
}

void CodeBuffer::printGlobalBuffer()
{
	for (vector<string>::const_iterator it = globalDefs.begin(); it != globalDefs.end(); ++it)
	{
		cout << *it << endl;
	}
}

void CodeBuffer::emitLoad(const string& src, const string& dest) {
	emit(dest + " = load i32, i32* " + src + ", align 4");
}

void CodeBuffer::emitLoadBool(const string &src, const string &dest) {
	emit(dest + " = load i1, i1* " + src + ", align 4");
}

void CodeBuffer::emitStore(const string& src, const string& dest) {
	emit("store i32 " + src + ", i32* " + dest + ", align 4");
}
void CodeBuffer::emitStoreBool(const string &src, const string &dest) {
	emit("store i1 " + src + ", i1* " + dest + ", align 4");
}

void CodeBuffer::emitBinop(const string& src1, const string& src2, const string& dest, const string& binop) {
	emit(dest + " = " + binop + " i32 " + src1 + ", " + src2);
}

void CodeBuffer::emitLabel(const string &name) {
	emit(name + ":");
}

void CodeBuffer::emitCond(const string &operation, const string &src1, const string &src2, const string &dest) {
	emit(dest + " = icmp " + operation + " i32 " + src1 + ", " + src2);
}

pair<bpatch_address, bpatch_address> CodeBuffer::emitIf(const string &cond, const string &ifTrueLabel, const string &ifFalseLabel) {
	int bufferAddress = emit("br i1 " + cond + ", label %" + ifTrueLabel + ", label %" + ifFalseLabel);
	return {{bufferAddress, FIRST}, {bufferAddress, SECOND}};
}

bpatch_address CodeBuffer::emitJmp(const string &dest) {
	return {emit("br label %" + dest), FIRST};
}

void CodeBuffer::emitFuncOpen(const string &name, const string& retType, const string &params) {
	emit("define " + retType + " @" + name + "(" + params + "){");
	emit("entry:");
}

void CodeBuffer::emitFuncClose() {
	emit("}");
}

void CodeBuffer::emitTransfer(const string &src, const string &dest) {
	emitBinop(src, "0", dest, "add");
}

// ******** Helper Methods ********** //
bool replace(string& str, const string& from, const string& to, const BranchLabelIndex index) {
	size_t pos;
	if (index == SECOND) {
		pos = str.find_last_of(from);
	}
	else { //FIRST
		pos = str.find_first_of(from);
	}
    if (pos == string::npos)
        return false;
    str.replace(pos, from.length(), to);
    return true;
}

