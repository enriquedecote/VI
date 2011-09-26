#ifndef CE_VI_H
#define CE_VIT_H

#include <iostream>
#include <stdlib.h>
#include <map>
#include <vector>
#include <string>
#include "VI.h"
#include "VariablesInfo.h"
#include "Variable.h"
#include "Environment.h"
#include <stdlib.h>
#include "glpk.h"
#include "../environments/GridWorld.h"

#define MAX 1
#define MIN -1
using namespace std;

/**
 * Class CE_VI
 */
class CE_VI : public VI {
	
	public:
		/**
		* Empty Constructor
		*/
		CE_VI();
		
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
		CE_VI(string name,
		      unsigned int pos,
		      unsigned int numAgents,
		      VariablesInfo*const admissibleActions,
		      float discountFactor,
		      float maxReward,
		      bool isVerbose,
		      char algorithm,
		      char opt,
		      float weight,
		      Environment* environment);
		
		/**
		* Virtual Destructor
		*/
		virtual ~CE_VI();
		
		virtual void  SetTransitionModel (const TransitionModel*const pTransitionModel);
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
		virtual void Folk();
		
		virtual void BuildConvexHull();

		virtual void DebugStateIndex();
		
		virtual void  Execute();
		
		virtual string PrintQTable();
		
		virtual string PrintVTable();
		
		virtual string PrintJointStrat();
		
		virtual string PrintInfo();
		
		virtual string DebugDynamics(vector <unsigned> state, vector <unsigned> actions);
		
		virtual string WriteStrategy(unsigned state);
		
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
		virtual void UpdateValue(float* rNewValue, float payoff, float bestValue);
		
		virtual void Reset();
		
		virtual float  ComputeVI(char algorithm);
	   /**
		* Computes the joint Strategy equilibrium
		*/
		virtual void uCE(unsigned state);
		
		virtual vector <double> CE(unsigned state, unsigned agent);
		
		virtual void eCE(unsigned i);
		
		virtual void rCE(unsigned i);
		
		void MiniMax(unsigned state);
		
		void Defend(unsigned state, int agent);
		
		virtual vector<double> MiniMax(unsigned state, unsigned agent, int direction, double* sum);
			
		virtual double Min(vector <double>);	
	   /**
		* Computes Friend-VI using the following objective function:
		* Vi(s)=max_a Qi(s,a)
		*/
		virtual void Friend(unsigned state);

		virtual void ParetOptimalSearch();

	   /**
		* Our algorithm
		*/
		virtual void TB(unsigned state);		

	   /**
		* Our algorithm
		*/
		virtual void TB(unsigned state, float weight);
		
	   /**
		* Solves the MDP looking at the joint actions as one "meta" action
		*/
		virtual void MDP(unsigned state, double weight);

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
		
		char mAlgorithm;
		
		char mOpt;
		
		unsigned mDefendAgent;
		
		double mWeight;
		
	   	/**
		* The number of states
		*/
		unsigned mNumStates;
		
	   /**
		* The initial state in each game
		*/
		unsigned mInitState;
		
		/**
		* The number of Joint actions
		*/
		unsigned mNumJointActions;
		
		/**
		* The Joint strategy 
		* <state; <jointAction, prob> >
		*/
		map <unsigned, map <unsigned, float> > mJointStrat;
		
		/**
   	  	* This is an alias to the transition model, that does not really belongs
     	* to this class.
     	*/
    	const TransitionModel* mpTransitionModel;
    	
    	GridWorld* mpEnvironment;
    	
    	ofstream debug;
    	
    	/**
		* The V-table. 
		* < state, expected payoff >
		*/
		map<unsigned, pair <float,float> > mJointVTable;
		
	 
};
#endif // COLF_AGENT_H
