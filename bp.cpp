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

void CodeBuffer::emitLoadSet(const string& src, const string& dest) {
	emit(dest + " = load " + SET_def + ", " + SET_def + "* " + src + ", align 4");
}

void CodeBuffer::emitLoadBool(const string &src, const string &dest) {
	emit(dest + " = load i1, i1* " + src + ", align 4");
}

void CodeBuffer::emitStore(const string& src, const string& dest) {
	emit("store i32 " + src + ", i32* " + dest + ", align 4");
}

void CodeBuffer::emitStoreSet(const string &src, const string &dest) {
	emit("store " + SET_def + " " + src + ", " + SET_def + "* " + dest + ", align 4");
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

void CodeBuffer::emitIf(const string &cond, const string &ifTrueLabel, const string &ifFalseLabel) {
	emit("br i1 " + cond + ", label %" + ifTrueLabel + ", label %" + ifFalseLabel);
}

void CodeBuffer::emitJmp(const string &dest) {
	emit("br label %" + dest);
}

pair<bpatch_address, bpatch_address> CodeBuffer::emitBpatchIf(const string &cond) {
	int bufferAddress = emit("br i1 " + cond + ", label @, label @");
	return {{bufferAddress, FIRST}, {bufferAddress, SECOND}};
}

bpatch_address CodeBuffer::emitBpatchJmp() {
	return {emit("br label @"), FIRST};
}

void CodeBuffer::emitFuncOpen(const string &name, const string& retType, const string &params) {
	emit("define " + retType + " @" + name + "(" + params + "){");
	emit("entry:");
}

void CodeBuffer::emitFuncClose(const string& retType) {
	if (retType == "void ") emit("ret void");
    else if (retType == SET_def + " ") {
        string emptySet = Framework::getInstance().freshTemp();
        emitNewSet("0", "5", emptySet);
        emit("ret " + retType + " " + emptySet);
    }
    else emit("ret i32 0");
	emit("}");
	emitEndLine();
}

void CodeBuffer::emitTransfer(const string &src, const string &dest) {
	emitBinop(src, "0", dest, "add");
}

void CodeBuffer::emitEndLine() {
    emit("");
}

void CodeBuffer::emitPhi(const string &label1, const string &label1_retVal,
						 const string &label2, const string &label2_retVal,
						 const string &dest) {
	emit(dest + " = phi i32 [" + label1_retVal + ", %" + label1 + "], [" + label2_retVal + ", %" + label2 + "]");
}

void CodeBuffer::emitZext(const string &sizeSrc, const string &src, const string &dest) {
	emit(dest + " = zext " + sizeSrc + " " + src + " to i32");
}

void CodeBuffer::emitSetExtractBegin(const string &setPtr, const string &dest) {
	emit(dest + " = extractvalue " + SET_def + " " + setPtr + ", 0");
}

void CodeBuffer::emitSetExtractEnd(const string &setPtr, const string &dest) {
	emit(dest + " = extractvalue " + SET_def + " " + setPtr + ", 1");
}

void CodeBuffer::emitSetExtractNumElems(const string &setPtr, const string &dest) {
	emit(dest + " = extractvalue " + SET_def + " " + setPtr + ", 2");
}

void CodeBuffer::emitException(const string &errStr) {
	emit("call void (i8*) @print(i8* " + errStr + " )");
	emit("call void (i32) @exit(i32 0)");
	emit("unreachable");
}

void CodeBuffer::emitNewSet(const string& setBegin, const string& setEnd, const string &dest) {
	string phase1 = Framework::getInstance().freshTemp();
	string phase2 = Framework::getInstance().freshTemp();
	string phase3 = Framework::getInstance().freshTemp();
	emit(phase1 + " = insertvalue " + SET_def + " undef, i32 " + setBegin + ", 0");
	emit(phase2 + " = insertvalue " + SET_def + " " + phase1 + ", i32 " + setEnd + ", 1");
	emit(phase3 + " = insertvalue " + SET_def + " " + phase2 + ", i32 0, 2");
	emit(dest + " = insertvalue " + SET_def + " " + phase3 + ", [256 x i1] zeroinitializer, 3");
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

