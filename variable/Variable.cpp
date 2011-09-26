#include <sstream>
#include <iostream>
#include "../include/Variable.h"
#include "../include/constants.h"

// NAMESPACES
using namespace std;
using namespace Sgi;

Variable::Variable() {
	mVarName  = "";
	mMinValue = 0;
	mMaxValue = 0;
	mVarValue = 0;
}

Variable::Variable(string varName, float minValue, float maxValue) {
	mVarName  = varName;
	mMinValue = minValue;
	mMaxValue = maxValue;
	mVarValue = minValue;
}
	
string Variable::GetVarName() const{
	return mVarName;
}

void Variable::SetVarName(string varName) {
	mVarName = varName;
}

float Variable::GetMinValue() const{
	return mMinValue;
}
    
void Variable::SetMinValue(float minValue) {
	mMinValue = minValue;
}

float Variable::GetMaxValue() const{
	return mMaxValue;
}

void Variable::SetMaxValue (float maxValue) {
	mMaxValue = maxValue;
}
	
float Variable::GetVarValue() const{
	return mVarValue;
}

bool Variable::SetVarValue(float varValue) {
	mVarValue = varValue;
	return true;
}

void Variable::Equal(const Variable*const pVar) {
	mVarName  = pVar->GetVarName() ;
	mMinValue = pVar->GetMinValue();
	mMaxValue = pVar->GetMaxValue();
	mVarValue = pVar->GetVarValue();
}

Variable* Variable::Clone() {
	Variable* var = new Variable();
	if (var == NULL) {
		cout << endl << endl << "\t\tVariable::Clone --> no memory available, now I will exit!" << endl << endl;
		exit(NO_MEM_AVAILABLE);
	}
	var->Equal(this);
	return var;
}

bool Variable::Equals(Variable* pVar) {
	return ((mVarName == pVar->GetVarName()) && (mMinValue == pVar->GetMinValue()) && 
			(mMaxValue == pVar->GetMaxValue()) && (mVarValue == pVar->GetVarValue()));
}
