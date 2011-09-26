#include <iostream>
#include <iomanip>
#include <sstream>
#include <cmath>
#include <fstream>
#include "../include/CKCoLFAgent.h"

using namespace std;

CKCoLFAgent::CKCoLFAgent(string name,
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
					Environment* environment) : CoLFAgent(name,
			 								pos,
											numAgents,
											admissibleActions,
											stationaryLearningRate,
											nonStationaryLearningRate,
											explFactor,
											decRate,
											decRateExpl,
											discountFactor,
											maxReward,
											lambda,
											isVerbose,
									     		environment) {
	mState = S_start;
	unsigned i;
	for(i = 0; i < mNumJointActions; i++) {
		mJointLastAction[i] = NULL;
	}
}

CKCoLFAgent::~CKCoLFAgent() {
	cout << "CKCoLFAgent's destructor called" << endl;
}

void CKCoLFAgent::Reset() {
	mState = S_start;
}

Variable* CKCoLFAgent::SelectAction(bool& rIsExplorative) {
	// since the action selection is the first operation of a new step,
	// update the values of the "previous" variables
	mPreviousAction = mCurrentAction;
	mPreviousPayoff = mCurrentPayoff;
	mPreviousJointAction = mCurrentJointAction;
	mPreviousStateIndex = mCurrentStateIndex;
	Variable* action = NULL;
	ActionSelection selection = A_greedy;
	if (mState == S_start) {
		selection = A_greedy;
	}
	else if (mState == S_initial) {
		selection = A_epsilon;
	}
	else if (mState == S_changed) {
		selection = A_constant;
	}
	if (selection == A_constant) {
		mCurrentAction = mPreviousAction;
		mIsCurrentActionExplorative = false;
		rIsExplorative = false;
	}
	else if (selection == A_greedy) {
		mIsCurrentActionExplorative = false;
		rIsExplorative = false;
		//unsigned int joint_index = ComputeJointIndex(mPreviousJointAction);
		ComputeBestAction(mCurrentAction, mCurrentBestPayoff, mJointQTable[mPreviousStateIndex]);
		// TODO: questo l'ho aggiunto io: chiedere se il metodo sopra è x l'azione o per il payoff (-> in qst last caso usare ComputeBestPayoff)
		action = mCurrentAction;
	}
	else if (selection == A_epsilon) {
		// extract a random number to test whether the current action selection must be an explorative or greedy action selection
		float rand_num = (float)(rand() % 1000)/1000.0;
		if (rand_num < mExplorationFactor) {
			cout << "\t..: esploro :.."; flush(cout);
			Sgi::hash_map<int, Variable*> actions;
			mAdmissibleActions->GetVariableList(actions);
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
			/* select the action that in the current state has the highest expected payoff */
			//unsigned int joint_index = ComputeJointIndex(mPreviousJointAction);
			cout << "\t<<eseguo>>"; flush(cout);
			ComputeBestAction(mCurrentAction, mCurrentBestPayoff, mJointQTable[mPreviousStateIndex]);
			action = mCurrentAction;
			mIsCurrentActionExplorative = false;
			rIsExplorative = false;
		}
	}
	if (action == NULL)
		abort();
	cout << "CKCoLFAgent::SelectAction --> " << action->GetVarName() << endl; flush(cout);
	return action;
}

void CKCoLFAgent::UpdateInitialQTable() {
}

State CKCoLFAgent::UpdateCK() {
	State next_state = S_initial;
	if (mState == S_start) {
		next_state = S_initial;
	}
	else if (mState == S_initial) {
		if (!mCurrentAction->Equals(mJointLastAction[mPreviousStateIndex])) {
			next_state = S_changed;
		}
		else {
			next_state = S_initial;
		}
	}
	else if (mState == S_changed) {
		next_state = S_initial;
	}
	return next_state;
}

void CKCoLFAgent::Update(float payoff, VariablesInfo* stateDescription) {
	if (true == mIsVerbose)
		*mpPayoffFile << "+" << mName << ": " << payoff << endl;
	mPreviousPayoff = mCurrentPayoff;
	mCurrentPayoff = payoff;
	mCurrentJointAction = stateDescription;
	// compute the indexes for the current and the previous joint action,
	// the index for the previous joint action has been already computed
	// in the action selection phase
	//cout << "CKCoLFAgent::Update --> richiamo (1) ComputeJointIndex" << endl;
	mCurrentStateIndex = ComputeJointIndex(stateDescription);
	// update the state of the finite state machine
	State next_state = UpdateCK();
	if (next_state == S_initial) {
		if (mState == S_initial)
			mUpdatingJointAction = mPreviousJointAction;
		//cout << "CKCoLFAgent::Update --> richiamo (2) ComputeJointIndex" << endl;
		// FIXME qui segmenta************************************************************************************************************************************
		unsigned int upd_joint_index = ComputeJointIndex(mUpdatingJointAction);
		ComputeBestPayoff(mCurrentBestPayoff, mJointQTable[mCurrentStateIndex]);
		float value;
		if (mIsQLearning == false) {
			float diff = fabs(mJointPayoff[upd_joint_index][mCurrentAction]-payoff);
			if (diff > mJointPayoffStdDev[upd_joint_index][mCurrentAction]) {
				// non-stationary behavior
				mLearningRate = mInitialNonStationaryLearningRate/(1.0+mJointExperience[upd_joint_index][mCurrentAction]*mDecreasingRate);
			}
			else {
				// stationary behavior
				mLearningRate = mInitialStationaryLearningRate/(1.0+mJointExperience[upd_joint_index][mCurrentAction]*mDecreasingRate);
			}
			UpdateValue(&value,diff,0.0,mLambda);
			mJointPayoffStdDev[upd_joint_index][mCurrentAction] = value;
			UpdateValue(&value,payoff,0.0,mLambda);
			mJointPayoff[upd_joint_index][mCurrentAction] = value;
		}
		else {
			mLearningRate = mInitialNonStationaryLearningRate/(1.0+mJointExperience[upd_joint_index][mCurrentAction]*mDecreasingRate);
		}
		UpdateValue(&value,payoff,mCurrentBestPayoff,mLearningRate);
		mJointQTable[upd_joint_index][mCurrentAction] = value;
		mJointExperience[upd_joint_index][mCurrentAction]++;
		// CK Corretto (con risultati brutti)
		//cout << "CKCoLFAgent::Update --> richiamo (3) ComputeJointIndex" << endl;
		unsigned int temp_index = ComputeJointIndex(mUpdatingJointAction);
		mJointLastAction[temp_index] = mCurrentAction;
		// CK corretto (con risultati buoni: uguali a quelli della versione originale)
		// mJointLastAction[mPreviousStateIndex] = mCurrentAction;
	}
	else {
		mUpdatingJointAction = mPreviousJointAction;
	}
	mPreviousPayoff = payoff;
	mState = next_state;
	mSteps++;
	float temp_expl = mInitialExplorationFactor - mDecRateExplorationFactor*mSteps;
	if (temp_expl > 0.0) {
		mExplorationFactor = temp_expl;
	}
	else {
		mExplorationFactor = 0.0;
	}
}
