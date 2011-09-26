#ifndef COLF_AGENT_H
#define COLF_AGENT_H

#include <iostream>
#include <stdlib.h>
#include <map>
#include <vector>
#include <string>
#include "LearningAgent.h"
#include "VariablesInfo.h"
#include "Variable.h"
#include "Environment.h"
#include "../environments/Grid.h"

using namespace std;

/**
 * Class CoLFAgent
 */
class CoLFAgent : public LearningAgent {
	
	public:
		/**
		* Empty Constructor
		*/
		CoLFAgent();
		
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
		CoLFAgent(string name,
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
		virtual ~CoLFAgent();
		
		/**
		* Return all the Q-table and some specific variables of the agent.
		* @return the string the contains the information
		*/
		//virtual string PrintQTable() ;
		
		/**
		* Update the Q-table with the payoff obtained after the execution of
		* the last joint action.
		* @param payoff      the paoyff obtained
		* @param jointAction the last executed joint action
		*/
		virtual void Update(float payoff,  VariablesInfo* stateDescription);

		virtual void UpdateInitialValues();
		
	protected:
	
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
		virtual void UpdateValue(float* rNewValue, float payoff, float bestValue, float learningRate = 0.0);
		
		/**
		* The initial value of the learning rate to be used when
		* the behoavior of the other agents is considered to be
		* stationary
		*/
		float mInitialStationaryLearningRate;
		
		/**
		* The initial value of the learning rate to be used when
		* the behoavior of the other agents is considered to be
		* non stationary
		*/
		float mInitialNonStationaryLearningRate;
		
		/**
		* The step size used to update the average reward
		*/
		float mLambda;
		
		/**
		* The instant payoff expected for each pair (state, action)
		* < riga (id dello stato); <azione,payoff> >
		*/
		map<float, map<Variable* , float> > mJointPayoff;
		
		/**
		* The standard deviation of the instant payoff expected
		* for each pair (state, action)
		* < riga (id dello stato); <azione,payoff> >
		*/
		map<float, map<Variable* , float> > mJointPayoffStdDev;
		
		/**
		* The discount factor used to weight future expected payoffs
		* with respect to recent ones.
		*/
		float mDiscountFactor;
		
		/**
		* If the current object is a simple Q-learning version of CoLF.
		* This situation occurs when the stationary and non stationasry
		* learning rate have the same values.
		*/
		bool mIsQLearning;
};
#endif // COLF_AGENT_H
