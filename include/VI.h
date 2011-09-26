#ifndef LEARNING_AGENT_H
#define LEARNING_AGENT_H

#include <iostream>
#include <stdlib.h>
#include <map>
#include <vector>
#include <string>
#include <fstream>
#include <mystl.h>
#include "VariablesInfo.h"
#include "Variable.h"
#include "Environment.h"
#include "../environments/Grid.h"

using namespace std;
using namespace Sgi;

/**
 * Class VI
 */
class VI {

	public:
	
		/**
		* Empty Constructor
		*/
		VI();
		
		/**
		* The constructor of the agent.
		* @param name the name of the agent, it is meant to be a unique identifier
		* @param pos  the ordinal position of the agent in the pool of agents used 
		*             in the experiment
		* @param numAgents         the number of agents involved in the experiment
		* @param admissibleActions the list of admissible values for the actions
		* @param learningRate      the learning rate used in the update formula
		* @param explFactor        the initial value of the exploration factor
		* @param decRate           the decreasing rate used to update the learning rate
		* @param decRateExpl       the decreasing rate used to update the exploration 
		*                          factor
		* @param maxReward         the maximum reward that can be obtained in the
		*                          experiment, it is used to compute the initial
		*                          values for the Q-table (to be specified by each 
		*                          implementation of this class)
		* @param isVerbose         if true the agent create a file where it writes
		*                          all its parameters and payoff during all the experiemnt
		*/
		VI(string name,
		   unsigned int pos,
		   unsigned int numAgents,
		   VariablesInfo*const admissibleActions,
		   float discountFactor,
		   float maxReward,
		   bool isVerbose,
		   Environment* environment);
		
		/**
		* Virtual Destructor
		*/
		virtual ~VI();
		
		/**
		* Return all the Q-table and some specific variables of the agent.
		* @return the string the contains the information
		*/
		virtual string PrintQTable() ;
		
		/**
		* Return the Q-function.
		* @return the string the contains the information
		*/
		//virtual string PrintQFunction();
		
		/**
		* Return the V-function, that is the highest expected discounted
		* payoff for each state (i.e. for each joint action). Each joint action
		* is identified by a uniqe identifier.
		* @return the string the contains the information
		*/
		virtual string PrintVFunction() ;
		
		/**
		* Select the action to be performed at the next step.
		* @param rIsExplorative  true if the action selected has been extracted
		*                        randomly according to the exploration strategy
		* @return the value of the action selected
		*/
		virtual Variable* SelectAction();
		
		/**
		* Reset the state of the agent.
		* NOTE: all the other parameters (learning rate, exploration factor, etc.)
		* are kept the same
		*/
		virtual void Reset();
		
		/**
		 * Return the agent name
		 * @return string with agent name
		 */
		 string GetName();

	protected:
	
		/**
		* Compute the best action and its value on the Q-table row passed as parameter.
		* @param rBestAction the value of the action with the highest expected payoff
		* @param rBestValue  the expected payoff of the best action
		* @param table       the Q-table row the best action is computed on
		*/
		void ComputeBestAction(Variable*& rBestAction, float& rBestValue, map<Variable* ,float>& table);
		
		/**
		 * Compute the highest expected payoff from the current state
		 * @param rBestValue the highest expected payoff
		 * @param table the Q-table row the best action is computed on
		 */
		void ComputeBestPayoff(float& rBestValue, map<Variable* , float>& table);
		
		/**
		* Return a string containing the specified joint action.
		* @param jointAction the joint action to be stringfied
		* @return the string containing the specified joint action
		*/
		//string JointToString(vector<float>& jointAction);
		
		/**
		* Update the old value of a variable according to a Q-learning like formula with
		* the specified learning rate, payoff and best value for the next state.
		* @param rNewValue    the reference to the value to be updated, it contains the old
		*                     value and after the execution of the method it will contain
		*                     the new value
		* @param payoff       the payoff obtained after the last execution
		* @param bestValue    the maximum expected payoff in the next state (i.e. joint-action)
		* @param learningRate the learning rate used in the mobile mean formula to update
		*                     the old value
		*/
		virtual void UpdateValue(float* rNewValue, float payoff, float bestValue) = 0;
		
		/**
		* Compute the action selection probabilities in the current joint action
		* according to the Boltzmann action selection strategy. The temperature
		* is set to 1.0.
		* @param rProb  the action selection probability for each action
		*/
		//virtual void ComputeBoltzmannProb(vector<float>& rProb);
		
		/**
		* Compute the unique index corresponding to the specified joint action.
		* @param jointAction the joint action to be converted into a unique index
		* @return the index of the joint action
		*/
		unsigned int ComputeJointIndex(VariablesInfo* jointAction);
       
		/**
		* The discount factor gamma.
		*/
		float mdiscountFactor;
		 
		/**
		* The number of time steps.
		*/
		float mSteps;
		
		/**
		* The number of actions available to the agent.
		*/
		unsigned int mNumActions;
		
		/**
		* The list of values corresponding to the available actions.
		*/
		VariablesInfo* mAdmissibleActions;
		
		/**
		* The name of the agent. It is meant to be a unique identifier
		* for the agent.
		*/
		string mName;
		
		/**
		* The ordinal position of the agent in the pool of created agents.
		* NOTE: up to now this information is not used.
		*/
		unsigned int mPos;
		
		/**
		* The number of agents involved in the experiment. This value is
		* necessary to compute the number of possible joint actions and
		* build the Q-table.
		*/
		unsigned int mNumAgents;

		float mMaxReward;
		
		/**
		* The index corresponding to the previous state
		*/
		unsigned int mPreviousStateIndex;
		
		/**
		* The index corresponding to the actual state
		*/
		unsigned int mCurrentStateIndex;
	 
		/**
		* The Q-table. Each row of the Q-table is identified by a unique 
		* identifier specific for each possible joint action. Each row
		* contains a list of actions and the corresponding expected payoff.
		* < row (state); < action,payoff > >
		*/
		map<unsigned, map<unsigned, pair<float,float> > > mJointQTable;
		
		/**
		* The number of visits for each pair (state,action) 
		* < row (state); < action,payoff > >
		*/
		map<unsigned, map<unsigned, pair<float,float> > > mJointExperience;
		
	 		
		/**
		* The payoff obtained after the execution of the last action.
		*/
		float mCurrentPayoff;
		
		/**
		* The payoff obtained at the previous time step.
		*/
		float mPreviousPayoff;
		
		/**
		* The action selected to be executed.
		*/
		Variable* mCurrentAction;
		
		/**
		* The Q-value correspondent to the best action
		*/
		float mCurrentBestPayoff;
		
		/**
		* The action selected at the previous time step.
		*/
		Variable* mPreviousAction;
		
		/**
		* The file to print the list of payoffs to
		*/
		ofstream *mpPayoffFile;
		
		/**
		* The file to print the list of learning rates used
		* in the updates
		*/
		ofstream *mpAlphaFile;
		
		/**
		* The files to print the list of probabilities to
		* select the M actions according to Boltzmann action
		* selection
		*/
		vector<ofstream *> mProbFiles;
		
		/**
		* If the agent must dump its parameters and payoffs
		* accumulated through the experiment to a data file.
		*/
		bool mIsVerbose;
		
		/**
		 * The environment where the agent acts
		 */
		Environment* mEnvironment;

		/**
		 * just for debugging purpose, to be deleted
		 */
		float mMaxStateValue;
};
#endif // LEARNING_AGENT_H
