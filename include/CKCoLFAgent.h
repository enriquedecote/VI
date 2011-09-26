#ifndef CK_COLF_AGENT_H
#define CK_COLF_AGENT_H

#include <iostream>
#include <stdlib.h>
#include <map>
#include <vector>
#include <string>
#include "CoLFAgent.h"
#include "../include/VariablesInfo.h"
#include "../include/Variable.h"

// NAMESPACES
using namespace std;

typedef enum {S_start = -1, S_initial, S_changed} State;
typedef enum {A_constant, A_epsilon, A_greedy} ActionSelection;

/**
 * Class CKCoLFAgent
 */
class CKCoLFAgent : public CoLFAgent {
	
	public:
		/**
		* Empty Constructor
		*/
		CKCoLFAgent();
		
		/**
		* The constructor of the agent.
		* NOTE: the stationary learning rate must be greater or equal to the non stationary
		*       learning rate.
		* @param name                      the name of the agent, it is meant to be a unique identifier
		* @param pos                       the ordinal position of the agent in the pool of agents used 
		*                                  in the experiment
		* @param numAgents                 the number of agents involved in the experiment
		* @param admissibleActions         the list of admissible values for the actions
		* @param stationarylearningRate    the learning rate used in the update formula when the
		*                                  behavior of the other agents is considered to be
		*                                  stationary
		* @param nonStationaryLearningRate the learning rate used in the update formula when the
		*                                  behavior of the other agents is considered to be
		*                                  non stationary
		* @param explFactor                the initial value of the exploration factor
		* @param decRate                   the decreasing rate used to update the learning rate
		* @param decRateExpl               the decreasing rate used to update the exploration 
		*                                  factor
		* @param discountFactor            the discount factor used to weight the future rewards
		*                                  with respect to recent ones
		* @param maxReward                 the maximum reward that can be obtained in the
		*                                  experiment, it is used to compute the initial
		*                                  values for the Q-table (to be specified by each 
		*                                  implementation of this class)
		* @param lambda                    the step size used to update the estimation of the
		*                                  average reward
		* @param isVerbose         if true the agent create a file where it writes
		*                          all its parameters and payoff during all the experiemnt
		*/
		CKCoLFAgent(string name,
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
			    Environment* environment);
		
		/**
		* Virtual Destructor
		*/
		virtual ~CKCoLFAgent();
		
		/**
		* Select the action to be performed at the next step.
		* @param rIsExplorative  true if the action selected has been extracted
		*                        randomly according to the exploration strategy
		* @return the value of the action selected
		*/
		virtual Variable* SelectAction(bool& rIsExplorative);

		virtual void UpdateInitialQTable();
		
		/**
		* Update the Q-table with the payoff obtained after the execution of
		* the last joint action.
		* @param payoff      the paoyff obtained
		* @param jointAction the last executed joint action
		*/
		virtual void Update(float payoff, VariablesInfo* stateDescription);
		
		/**
		* Reset the state of the agent.
		*/
		virtual void Reset();
		
	protected:
		
		/**
		* Update the state of the finite state machine according
		* to the rules proper of the CK algorithm.
		* @return the new state
		*/
		State UpdateCK();
		
		/**
		* The internal state of the finite state machine characterizing
		* the CK algorithm.
		*/
		State mState;
		
		/**
		* The joint action (i.e. the state) that is actually updated. In
		* fact in CK it is not always the previous joint action the state
		* updated.
		*/
		VariablesInfo* mUpdatingJointAction;
		
		/**
		* Contains the last action executed when the agent was at a
		* a specified state (i.e. a joint action).
		*/
		map<int, Variable*> mJointLastAction;
	
};
#endif // COLF_AGENT_H
