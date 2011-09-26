#include <iostream>
#include <sstream>
#include "Bar.h"
#include <ext/hash_map>
#include <mystl.h>

using namespace std;

Bar::Bar() {
}
	
Bar::~Bar() {
	delete mpStateInfo;
}

void  Bar::CustomInit() {
}

void Bar::Init(VariablesInfo* pActions) {
	VariablesInfo* new_pActions = pActions->Clone();
	mpStateInfo = new_pActions;
}

void Bar::AppendToInit(VariablesInfo* pActions) {
}

void  Bar::Reset() {
	delete mpStateInfo;
}

// 2B completed
float Bar::GetMaxReward() {
	return 1;
}

// 2B implemented
float Bar::EvaluateActions(string AgentName) {
	return 0;
}

bool Bar::IsFeasibleAction(Variable* pAction, string AgentName) {
/* (21/10/2005)
così su due piedi dovrebbero essere possibili tutte le azioni (supponendo 
il bar con capacità infinita...)
*/
	return true;
}

VariablesInfo* Bar::GetStateDescription() {
	return mpStateInfo;
}

VariablesInfo* Bar::GetPartialStateDescription() {
	return mpStateInfo;
}

string Bar::ToString() {
	ostringstream ret(ostringstream::out);
	Sgi::hash_map<int, Variable*> states;
	Sgi::hash_map<int, Variable*>::iterator it;
	mpStateInfo->GetVariableList(states);
	it = states.begin();
	while (it != states.end()) {
		ret << it->second->GetVarName() << endl;
		ret << it->second->GetVarValue() << endl;
		it++;
	}
	return ret.str();
}
