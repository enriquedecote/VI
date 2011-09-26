#include <iostream>
#include <iomanip>
#include <sstream>
#include <cmath>
#include <fstream>
#include "../include/CoLFAgent.h"

using namespace std;

CoLFAgent::CoLFAgent(string name,
					unsigned int pos,
					unsigned int numAgents,
					VariablesInfo*const admissibleActions,
					float stationaryLearningRate,
					float nonStationaryLearningRate,
					float explFactor,
					float decRate,
					float decRateExpl,
					float discountFactor,
					float maxReward,
					float lambda,
					bool isVerbose,
					Environment* environment) : LearningAgent(name,
																pos,
																numAgents,
																admissibleActions,
																stationaryLearningRate,
																explFactor,
																decRate,
																decRateExpl,
																maxReward,
																isVerbose,
																environment) {
	mInitialStationaryLearningRate = stationaryLearningRate;
	mInitialNonStationaryLearningRate = nonStationaryLearningRate;
	mDiscountFactor = discountFactor;
	mLambda = lambda;
	if (stationaryLearningRate == nonStationaryLearningRate)
		mIsQLearning = true;
	/*float init_value = maxReward / (1.0-mDiscountFactor);
	Sgi::hash_map<int, Variable*> actions;
	unsigned i;
	for(i = 0; i < mNumJointActions; i++) {
		mAdmissibleActions->GetVariableList(actions);
		Sgi::hash_map<int, Variable*>::iterator it = actions.begin();
		Variable* action = NULL;
		while (it != actions.end()) {
			action = it->second;
			mJointPayoff[i][action] = maxReward;
			mJointPayoffStdDev[i][action] = 0.0;
			mJointQTable[i][action] = init_value;
			mJointExperience[i][action] = 0;
			it++;
		}
	}*/
	/*if (true == isVerbose) {
		ofstream params("parameters.txt", ios::app);
		params << "+" << mName << ": initialization " << init_value << endl;
		params << "+" << mName << ": lambda = " << mLambda << endl;
		params << "+" << mName << ": gamma = " << mDiscountFactor << endl;
		params << "+" << mName << ": isQLearning = " << mIsQLearning << endl;
		params << endl;
		params.close();
	}*/
}

void CoLFAgent::UpdateInitialValues() {
	/* Settare i valori iniziali di
	 * 	mJointPayoff
	 * 	mJointPayoffStdDev
	 * 	mJointQTable
	 * 	mJointExperience
	 * per OGNI possibile valore dello stato. Max valore dello stato: suppongo che 
	 * tutti gli "a" agenti siano nelle ultime "a" celle
	 * Detto
	 * 	* v = valore cella occupata dall'agente i (all'inizio i = 0)
	 * 	* n = n° tot celle
	 *  * a = n° tot agenti
	 * --> val max = v*n^(i) + v*n^(i+1) + v*n^(i+2) + ... + v*n^(a-1)
	 */
	Sgi::hash_map<int, Variable*> actions;
	mAdmissibleActions->GetVariableList(actions);
	Sgi::hash_map<int, Variable*>::iterator it_actions;
	Variable* action = NULL;
	Sgi::hash_map<int, Variable*> state;
	//VariablesInfo* stateDescription = mEnvironment->GetPartialStateDescription();
	stateDescription->GetVariableList(state);
	Sgi::hash_map<int, Variable*>::iterator it_state = state.begin();
	unsigned environmentSize = (unsigned) it_state->second->GetVarValue();
	unsigned i;
	mMaxStateValue = 0;
	float initialValue = mMaxReward / (1.0 - mDiscountFactor);
	for (i = 0; i < mNumAgents; i++) {
		mMaxStateValue+= (environmentSize-mNumAgents+i)*powf((float) environmentSize,(float) i);
	}
	cout << "CoLFAgent::UpdateInitialValues --> max state size = " << mMaxStateValue << endl; flush(cout);
	for (i = 0, it_actions = actions.begin(); i <= (unsigned) mMaxStateValue; i++, it_actions = actions.begin()) {
		while (it_actions != actions.end()) {
			action = it_actions->second;
			mJointPayoff[i][action] = mMaxReward;
			mJointPayoffStdDev[i][action] = 0.0;
			mJointQTable[i][action] = initialValue;
			mJointExperience[i][action] = 0;
			it_actions++;
		}
	}
}

CoLFAgent::~CoLFAgent () {
	cout << "CoLFAgent::~CoLFAgent" << endl;
}

void CoLFAgent::Update(float payoff, VariablesInfo* stateDescription) {
	if (true == mIsVerbose)
		*mpPayoffFile << "+" << mName << ": " << payoff << endl;
	mPreviousPayoff = mCurrentPayoff;
	mCurrentPayoff = payoff;
	// compute the indexes for the current and the previous joint action
	// the index for the previous joint action has been already computed in the action selection phase
	mCurrentStateIndex = ComputeJointIndex(stateDescription);
	// compute the highest expected payoff from the current state on
	ComputeBestPayoff(mCurrentBestPayoff, mJointQTable[mCurrentStateIndex]);
	//Variable* action_temp = NULL; // it seems to be unused
	//ComputeBestAction(*&action_temp, mCurrentBestPayoff, mJointQTable[mCurrentStateIndex]);
	float value = 0.0;
	//cout << endl << "\tCoLFAgent::Update --> aggiorno valore in posizione [" << mPreviousStateIndex << "][" << mCurrentAction->GetVarValue() << "] -- value = ";
	if (!mIsQLearning) {
		float diff = fabs(mJointPayoff[mPreviousStateIndex][mCurrentAction]-payoff);
		float denom = (1.0+mJointExperience[mPreviousStateIndex][mCurrentAction]*mDecreasingRate);
		if(diff > mJointPayoffStdDev[mPreviousStateIndex][mCurrentAction]) {
			mLearningRate = mInitialNonStationaryLearningRate/denom;	// non-stationary behavior
		}
		else {
			mLearningRate = mInitialStationaryLearningRate/denom;	// stationary behavior
		}
		UpdateValue(&value, diff, 0.0, mLambda);
		mJointPayoffStdDev[mPreviousStateIndex][mCurrentAction] = value;
		UpdateValue(&value, payoff, 0.0, mLambda);
		mJointPayoff[mPreviousStateIndex][mCurrentAction] = value;
    	}
	else {
		mLearningRate = mInitialNonStationaryLearningRate/(1.0+mJointExperience[mPreviousStateIndex][mCurrentAction]*mDecreasingRate);
	}
	// update the old Q-value according to the instant payoff and the highest expected payoff with the specified learning rate
	if (true == mIsVerbose)
		(*mpAlphaFile) << mLearningRate << endl;
	UpdateValue(&value, payoff, mCurrentBestPayoff, mLearningRate);
	mJointQTable[mPreviousStateIndex][mCurrentAction] = value;
	//cout << value;
	mJointExperience[mPreviousStateIndex][mCurrentAction]++;
	mPreviousPayoff = payoff;
	mSteps++;
	// update the exploration factor
	float temp_expl = mInitialExplorationFactor - mDecRateExplorationFactor*mSteps;
	if (temp_expl > 0.0)
		mExplorationFactor = temp_expl;
	else
		mExplorationFactor = 0.0;
}

void CoLFAgent::UpdateValue (float* rNewValue,  float payoff,  float bestValue, float learningRate) {
	if (learningRate <= 0.0)
		learningRate = mLearningRate;
	*rNewValue = (1-learningRate)*(*rNewValue) + learningRate*(payoff + mDiscountFactor*bestValue);
}
