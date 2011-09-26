#include <iostream>
#include <iomanip>
#include <sstream>
#include <cmath>
#include <fstream>
#include "../include/LearningAgent.h"

// NAMESPACES
using namespace std;

LearningAgent::LearningAgent(string name,
								unsigned int pos,
								unsigned int numAgents,
								VariablesInfo*const admissibleActions,
								float learningRate,
								float explFactor,
								float decRate,
								float decRateExpl,
								float maxReward,
								bool isVerbose,
								Environment* environment) {
	mName = name;
	mPos = pos;
	mNumAgents = numAgents;
	mNumActions = admissibleActions->size();
	mAdmissibleActions = admissibleActions;
	mExplorationFactor = explFactor;
	mInitialExplorationFactor = explFactor;
	mDecRateExplorationFactor = decRateExpl;
	mLearningRate = learningRate;
	mInitialExplorationFactor = learningRate;
	mDecreasingRate = decRate;
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

LearningAgent::~LearningAgent() {
	cout << "LearningAgent::~LearningAgent(" << mName << ")" << endl; flush(cout);
	/*if (mIsVerbose) {
		mpPayoffFile->close();
		delete mpPayoffFile;
		mpAlphaFile->close();
		delete mpAlphaFile;
		for (unsigned int i = 0; i < mProbFiles.size(); i++) {
			mProbFiles[i]->close();
			delete mProbFiles[i];
		}
	}*/
	cout << "LearningAgent::~LearningAgent(" << mName << "); deleting mAdmissibleActions" << endl; flush(cout);
	delete mAdmissibleActions;
	//delete mPreviousJointAction;
	//delete mCurrentJointAction;
	cout << "LearningAgent::~LearningAgent(" << mName << "); deleting mExplorativeAction" << endl; flush(cout);
	delete mExplorativeAction;
	cout << "LearningAgent::~LearningAgent(" << mName << "); deleting mCurrentAction" << endl; flush(cout);
	delete mCurrentAction;
	cout << "LearningAgent::~LearningAgent(" << mName << "); deleting mPreviousAction" << endl; flush(cout);
	delete mPreviousAction;
	map<float, map<Variable*, float> >::iterator it_q = mJointQTable.begin();
	cout << "LearningAgent::~LearningAgent(" << mName << "); deleting mJointQTable" << endl; flush(cout);
	while (it_q != mJointQTable.end()) {
		map<Variable*, float>::iterator it_internal = it_q->second.begin();
		while (it_internal != it_q->second.end()) {
			delete it_internal->first;
			it_internal++;
		}
		it_q++;
	}
	map<float, map<Variable*, unsigned> >::iterator it_je = mJointExperience.begin();
	cout << "LearningAgent::~LearningAgent(" << mName << "); deleting mJointExperience" << endl; flush(cout);
	while (it_je != mJointExperience.end()) {
		map<Variable*, unsigned>::iterator it_internal = it_je->second.begin();
		while (it_internal != it_je->second.end()) {
			delete it_internal->first;
			it_internal++;
		}
		it_je++;
	}
}

void LearningAgent::Reset() {
	cout << endl << endl << "LearningAgent::Reset" << endl << endl;
	mCurrentAction = NULL;
	mCurrentPayoff = 0.0;
	mPreviousAction = NULL;
	mPreviousPayoff = 0.0;
	mMaxStateValue = 0;
	/*if (mPreviousJointAction != NULL)
		mPreviousJointAction->Clear();
	if (mCurrentJointAction != NULL)
		mCurrentJointAction->Clear();*/
	delete mAdmissibleActions;
	//delete mPreviousJointAction;
	//delete mCurrentJointAction;
	delete mExplorativeAction;
	delete mCurrentAction;
	delete mPreviousAction;
	map<float, map<Variable*, float> >::iterator it_q = mJointQTable.begin();
	while (it_q != mJointQTable.end()) {
		map<Variable*, float>::iterator it_internal = it_q->second.begin();
		while (it_internal != it_q->second.end()) {
			delete it_internal->first;
			it_internal++;
		}
		it_q++;
	}
	map<float, map<Variable*, unsigned> >::iterator it_je = mJointExperience.begin();
	while (it_je != mJointExperience.end()) {
		map<Variable*, unsigned>::iterator it_internal = it_je->second.begin();
		while (it_internal != it_je->second.end()) {
			delete it_internal->first;
			it_internal++;
		}
		it_je++;
	}
}

Variable* LearningAgent::SelectAction(bool& rIsExplorative) {
	// since the action selection is the first operation of a new step, update the values of the "previous" variables
	mPreviousAction = mCurrentAction;
	mPreviousPayoff = mCurrentPayoff;
	mPreviousStateIndex = mCurrentStateIndex;
	// extract a random number to test whether the current action selection must be an explorative or greedy action selection
	float rand_num = (float)(rand() % 1000)/1000.0;
	Variable* action = NULL;
	if (rand_num < mExplorationFactor) {
		//cout << "LearningAgent::SelectAction --> Agent " << mName << " is randomly choosing an action" << endl; flush(cout);
		Sgi::hash_map<int, Variable*> actions;
		mAdmissibleActions->GetVariableList(actions);
		/*for (unsigned i=0; i <mNumActions; i++) {
			cout << "  LearningAgent::SelectAction --> Agent " << mName << " can do this action: " << actions[i]->GetVarName() << endl;
			flush(cout);
		}*/
		srand(time(NULL));
		do {
			action = actions[(rand()/rand()+rand()*rand()*rand()) % (mNumActions)];
		} while (!mEnvironment->IsFeasibleAction(action,mName));
		// "action" is the current action choosen
		mCurrentAction = action;
		mExplorativeAction = mCurrentAction;
		mIsCurrentActionExplorative = true;
		rIsExplorative = true;
	}
	else {
		// select the action that in the current state has the highest expected payoff
		// unsigned int joint_index = ComputeJointIndex(mPreviousJointAction);
		// just for debugging purpose
		/*cout << "LearningAgent::SelectAction --> Agent " << mName << " is calling ComputeBestAction; ";
		cout << "contenuto di mJointQTable[" << mPreviousStateIndex << "]" << endl; flush(cout);
		map<Variable* ,float>::iterator it = mJointQTable[mPreviousStateIndex].begin();
		while (it != mJointQTable[mPreviousStateIndex].end()) {
			cout << "\tNome: " << it->first->GetVarName() << "; payoff = " << it->second << endl; flush(cout);
			it++;
		}*/
		ComputeBestAction(mCurrentAction, mCurrentBestPayoff, mJointQTable[mPreviousStateIndex]);
		action = mCurrentAction;
		mIsCurrentActionExplorative = false;
		rIsExplorative = false;
	}
	return action;
}

// table è la coppia <azione,payoff dell'azione> di uno specifico stato
void LearningAgent::ComputeBestAction(Variable*& rBestAction, float& rBestValue, map<Variable* , float>& table) {
	// Filling temporary arrays of actions and associated payoff
	map<Variable* ,float>::iterator temp_it = table.begin();
	Variable* v[table.size()]; // actions
	float p[table.size()]; // payoff associated to each actions
	unsigned size = 0;
	int pos;
	for (pos = 0 ; temp_it != table.end(); temp_it++, pos++) {
		v[pos] = temp_it->first;
		p[pos] = temp_it->second;
		size = pos;
	}
	int i;
	int j; // i and j must NOT be unsigned, since they will be decremented!!!
	// Let's start sorting (insertion sort) in descending order based on payoff of each action
	Variable* index_v;
	float index_p;
	for (i = 1; i < (int)table.size(); i++) {
		index_v = v[i];
		index_p = p[i];
		j = i;
		while ((j > 0) && (p[j-1] > index_p)) {
			p[j] = p[j-1];
			v[j] = v[j-1];
			j--;
		}
		p[j] = index_p;
		v[j] = index_v;
	}
	// Now actions are sorted in ascending order respect to their payoff
	for (i = size; i >= 0; i--) {
		if (mEnvironment->IsFeasibleAction(v[i],mName)) {
			rBestAction = v[i];
			rBestValue = p[i];
			break;
		}
	}
	//cout << "LearningAgent::ComputeBestAction --> action = " << rBestAction->GetVarName() << ", payoff = " << rBestValue << endl;
}

void LearningAgent::ComputeBestPayoff(float& rBestValue, map<Variable* , float>& table) {
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

unsigned int LearningAgent::ComputeJointIndex(VariablesInfo* stateDescription) {
	/* ogni variabile contenuta da stateDescription è così strutturata:
	 *  (+) Prima variabile --> dimensione dell'ambiente (n° celle nella griglia, max agenti al bar, ...)
	 *  (+) Variabili seguenti:
	 * 		[*] name = Agent1
	 *  	[*] value = valore numerico descrittivo dello stato (es.: num cella occupata, num dell'agente, ...)
	 *  index = somma (valore_cella * (dim_ambiente)^pos)
	 */
	if (stateDescription == NULL) {
		cout << endl << endl << "\t\tLearningAgent::ComputeJointIndex --> stateDescription is NULL" << endl << endl;
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
		cout << endl << endl << "\t\tLearningAgent::ComputeJointIndex --> stateDescription is empty" << endl << endl;
		flush(cout);
		exit(SOMETHING_WRONG);
	}
	environmentSize = (unsigned) it->second->GetVarValue();
	//cout << "\t\tLearningAgent::ComputeJointIndex(" << mName << "):" << endl; flush(cout);
	it++;
	if (it == state.end()) {
		cout << endl << endl << "\t\tLearningAgent::ComputeJointIndex --> malformed stateDescription" << endl;
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
	//cout << "\t................LearningAgent::ComputeJointIndex --> index = " << index << endl; flush(cout);
	if (index > mMaxStateValue) { // just to avoid segmentation ;)
		cout << endl << endl << "\t\tLearningAgent::ComputeJointIndex --> bad index computed" << endl;
		cout << "\t\tmMaxStateValue is " << mMaxStateValue << "while index is " << index << endl << endl;
		flush(cout);
		exit(SOMETHING_WRONG);
	}
	return index;
}

string LearningAgent::GetName() {
	return mName;
}

string LearningAgent::PrintQTable() {
	ostringstream str;
	str << setprecision(6);
	str << "\n(" << mPos+1 << ") Agent: " << mName << endl;
	str << "mJointQTable (experience)" << endl;
	map<float, map<Variable*, float> >::const_iterator joint_it;
	map<float, map<Variable*, unsigned int> >::const_iterator joint_exp_it = mJointExperience.begin();
	for (joint_it = mJointQTable.begin(); joint_it != mJointQTable.end(); joint_it++) {
		str << "j[" << joint_it->first << "]:\t";
		map<Variable*, float>::const_iterator action_it;
		map<Variable*, unsigned int>::const_iterator exp_it = joint_exp_it->second.begin();
		for (action_it = joint_it->second.begin(); action_it != joint_it->second.end(); action_it++) {
			str << setw(8) << setprecision(4) << action_it->first->GetVarName() << ",p=" << action_it->second << " (" << exp_it->second << ")\t\t";
			exp_it++;
		}
		str << endl;
		joint_exp_it++;
	}
	return str.str();
}

string LearningAgent::PrintVFunction() {
	ostringstream str;
	str << setprecision(6);
	map<float, map<Variable*, float> >::const_iterator joint_it;
	for (joint_it = mJointQTable.begin(); joint_it != mJointQTable.end(); joint_it++) {
		str << joint_it->first << "\t";
		map<Variable*, float>::const_iterator action_it = joint_it->second.begin();
		float best_value = action_it->second;
		for (action_it++; action_it != joint_it->second.end(); action_it++) {
			if (best_value < action_it->second) {
				best_value = action_it->second;
			}
		}
		str << setprecision(4) << best_value << endl;
	}
	return str.str();
}
