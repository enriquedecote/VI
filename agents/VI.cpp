#include <iostream>
#include <iomanip>
#include <sstream>
#include <cmath>
#include <fstream>
#include "../include/VI.h"

// NAMESPACES
using namespace std;

VI::VI(string name, 
       unsigned int pos, 
       unsigned int numAgents, 
       VariablesInfo*const admissibleActions,
       float discountFactor,
       float maxReward,
       bool isVerbose,
       Environment* environment) 
{
  mName = name;
  mPos = pos;
  mNumAgents = numAgents;
  mNumActions = admissibleActions->size();
  mAdmissibleActions = admissibleActions;
  mdiscountFactor  = discountFactor;
  mCurrentAction = NULL;
  mCurrentPayoff = 0.0;
  mPreviousAction = NULL;
  mPreviousPayoff = 0.0;
  mMaxReward = maxReward;
  //mPreviousJointAction= NULL;
  mPreviousStateIndex = 0;
  //mCurrentJointAction = NULL;
  mCurrentStateIndex = 0;
  mIsVerbose = isVerbose;
  mEnvironment = environment;
}

VI::~VI() {
	
}

void VI::Reset() {
	
}

Variable* VI::SelectAction( ) {
  // since the action selection is the first operation of a new step, update the values of the "previous" variables
  mPreviousAction = mCurrentAction;
  mPreviousPayoff = mCurrentPayoff;
  mPreviousStateIndex = mCurrentStateIndex;
  // extract a random number to test whether the current action selection must be an explorative or greedy action selection
  Variable* action = NULL;
  
  // select the action that in the current state has the highest expected payoff
  // unsigned int joint_index = ComputeJointIndex(mPreviousJointAction);
  // just for debugging purpose
  /*cout << "VI::SelectAction --> Agent " << mName << " is calling ComputeBestAction; ";
    cout << "contenuto di mJointQTable[" << mPreviousStateIndex << "]" << endl; flush(cout);
    map<Variable* ,float>::iterator it = mJointQTable[mPreviousStateIndex].begin();
    while (it != mJointQTable[mPreviousStateIndex].end()) {
    cout << "\tNome: " << it->first->GetVarName() << "; payoff = " << it->second << endl; flush(cout);
    it++;
    }*/
 // ComputeBestAction(mCurrentAction, mCurrentBestPayoff, mJointQTable[mPreviousStateIndex]);
  action = mCurrentAction;
  
  return action;
}

// table è la coppia <azione,payoff dell'azione> di uno specifico stato
void VI::ComputeBestAction(Variable*& rBestAction, float& rBestValue, map<Variable* , float>& table) {

 
  //cout << "VI::ComputeBestAction --> action = " << rBestAction->GetVarName() << ", payoff = " << rBestValue << endl;
}

void VI::ComputeBestPayoff(float& rBestValue, map<Variable* , float>& table) {
  // Filling temporary arrays of actions and associated payoff
  map<Variable* ,float>::iterator temp_it = table.begin();
  float best=-9999.999;
  while (temp_it != table.end()) {
    if (temp_it->second > best)
      best = temp_it->second;
    temp_it++;
  }
  rBestValue = best;
}

unsigned int VI::ComputeJointIndex(VariablesInfo* stateDescription) {
	/* ogni variabile contenuta da stateDescription è così strutturata:
	 *  (+) Prima variabile --> dimensione dell'ambiente (n° celle nella griglia, max agenti al bar, ...)
	 *  (+) Variabili seguenti:
	 * 		[*] name = Agent1
	 *  	[*] value = valore numerico descrittivo dello stato (es.: num cella occupata, num dell'agente, ...)
	 *  index = somma (valore_cella * (dim_ambiente)^pos)
	 */
	if (stateDescription == NULL) {
		cout << endl << endl << "\t\tVI::ComputeJointIndex --> stateDescription is NULL" << endl << endl;
		flush(cout);
		exit(SOMETHING_WRONG);
	}
	unsigned index = 0;
	unsigned pos = 0;
	unsigned environmentSize = 0;
	Sgi::hash_map<int, Variable*> state;
	stateDescription->GetVariableList(state);
	Sgi::hash_map<int, Variable*>::iterator it = state.begin();
	if (it == state.end()) {
		cout << endl << endl << "\t\tVI::ComputeJointIndex --> stateDescription is empty" << endl << endl;
		flush(cout);
		exit(SOMETHING_WRONG);
	}
	environmentSize = (unsigned) it->second->GetVarValue();
	//cout << "\t\tVI::ComputeJointIndex(" << mName << "):" << endl; flush(cout);
	it++;
	if (it == state.end()) {
		cout << endl << endl << "\t\tVI::ComputeJointIndex --> malformed stateDescription" << endl;
		cout << "\t\tIt contains only the total number of cells in this grid" << endl << endl;
		flush(cout);
		exit(SOMETHING_WRONG);
	}
	//cout << "\t\t\t+ environmentSize = " << environmentSize << endl; flush(cout);
	while (it != state.end()) {
		/*cout << "\t\t\t+ var value (n° cella occupata) = " << it->second->GetVarValue() << endl;
		cout << "\t\t\t  --> partial index = " << (unsigned) ((it->second->GetVarValue())*((unsigned)powf((float)environmentSize, (float)pos))) << endl;
		flush(cout);*/
		index += (unsigned) ((it->second->GetVarValue())*((unsigned)powf((float)environmentSize, (float)pos)));
		pos++;
		it++;
	}
	//cout << "\t................VI::ComputeJointIndex --> index = " << index << endl; flush(cout);
	if (index > mMaxStateValue) { // just to avoid segmentation ;)
		cout << endl << endl << "\t\tVI::ComputeJointIndex --> bad index computed" << endl;
		cout << "\t\tmMaxStateValue is " << mMaxStateValue << "while index is " << index << endl << endl;
		flush(cout);
		exit(SOMETHING_WRONG);
	}
	return index;
}

string VI::GetName() {
	return mName;
}

string VI::PrintQTable() {
	return NULL;
}

string VI::PrintVFunction() {
return NULL;
}
