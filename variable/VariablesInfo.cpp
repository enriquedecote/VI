#include <sstream>
#include <assert.h>
#include <ext/hash_map>
#include <iostream>
#include "../include/VariablesInfo.h"
#include "../include/constants.h"

// NAMESPACES
using namespace std;

VariablesInfo::~VariablesInfo() {
	unsigned i;
	for (i = 0; i < mVariables.size(); i++) {
		delete mVariables[i];
	}
}
	
bool VariablesInfo::GetVarValue(string varName, float& rValue) {
	Variable* var = NULL;
	unsigned i;
	for (i = 0; i < mVariables.size() && var == NULL; i++) {
		if (mVariables[i]->GetVarName() == varName) var = mVariables[i];
	}
	if (var != NULL) {
		rValue = var->GetVarValue();
		return true;
	}
	return false;
}

bool VariablesInfo::SetVarValue(string varName, float varValue) {
	Variable* var = NULL;
	unsigned i;
	for (i = 0; i < mVariables.size() && var == NULL; i++) {
		if (mVariables[i]->GetVarName() == varName) var = mVariables[i];
	}
	if (var != NULL) {
		var->SetVarValue(varValue);
		return true;
	}
	return false;
}

void VariablesInfo::AddVariable(Variable* var) {
	Variable* newVar = var->Clone();
	if (newVar == NULL) {
		cout << endl << endl << "\t\tVariablesInfo::AddVariable --> no memory available, now I will exit!" << endl << endl;
		exit(NO_MEM_AVAILABLE);
	}
	mVariables.push_back(newVar);
}

void VariablesInfo::AddVariableInFront(Variable* var) {
	Variable* foo = new Variable("foo",0.0,0.0);
	int i; // it must NOT be usigned, since it will be decremented!!!
	mVariables.push_back(foo);
	/*cout << "Var 2b added: " << var->GetVarName() << endl;
	cout << "Before:" << endl;
	for (unsigned j = 0; j < mVariables.size(); j++)
		cout << "\t" << mVariables[j]->GetVarName() << endl;*/
	for (i = mVariables.size()-2; i >= 0; i--) {
		//cout << "step i = " << i << endl;
		mVariables[i+1] = mVariables[i]->Clone();
		delete mVariables[i];
	}
	mVariables[0] = var->Clone();
	/*cout << endl << "After:" << endl;
	for (unsigned j = 0; j < mVariables.size(); j++)
		cout << "\t" << mVariables[j]->GetVarName() << endl;*/
}

const Variable*const VariablesInfo::GetVariable(string varName) {
	Variable* this_var = NULL;
	//cout << endl<<"VariablesInfo::GetVariable: " << varName << endl;
	unsigned i;
	for (i = 0; i < mVariables.size(); i++) {
		this_var = mVariables[i];
		//cout << "->" << this_var->GetVarName();
		if (this_var->GetVarName().find(varName) != string::npos){
			//cout << "==" << varName << endl;
            return this_var;
		}
	}
	return NULL;
}
	
const Variable*const  VariablesInfo::GetVariable (const unsigned int pos)
{
    Variable* var = NULL;
   // if (pos < mNumVars)
        var = mVariables[pos];

    return var;
}
	
void VariablesInfo::GetVariableList(Sgi::hash_map<int, Variable*>& variables) {
	int dim = mVariables.size();
	int i;
	for (i = 0; i < dim; i++) {
		variables[i] = mVariables[i];
	}
}

unsigned int VariablesInfo::size() {
	return mVariables.size();
}

void VariablesInfo::Equal(VariablesInfo* pVariables) {
	unsigned i;
	for (i = 0; i < mVariables.size(); i++) {
		string varName = mVariables[i]->GetVarName();
		const Variable*const var = pVariables->GetVariable(varName);
		assert(var != NULL);
		mVariables[i]->Equal(var);		
	}
}

VariablesInfo* VariablesInfo::Clone() {;
	VariablesInfo* variables = new VariablesInfo();
	unsigned i;
	for (i = 0; i < mVariables.size(); i++) {
		Variable* var = mVariables[i]->Clone();
		variables->AddVariable(var);
		delete var;
	}
	return variables;
}

void VariablesInfo::Clear() {
	mVariables.clear();
}

Variable* VariablesInfo::End() {
	return mVariables[mVariables.size()-1];
}

Variable* VariablesInfo::Find(Variable* pVar) {
	if (pVar == NULL)
		return NULL;
	vector<Variable*>::iterator it = mVariables.begin();
	while (it != mVariables.end()) {
		if ((*it)->GetVarName() == pVar->GetVarName())
			return *it;
		it++;
	}
	return NULL;
}
