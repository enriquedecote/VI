#ifndef GRID_H
#define GRID_H

#include "../include/Environment.h"
#include "../include/Variable.h"
#include "../include/VariablesInfo.h"
#include "../include/constants.h"
#include "../models/TransitionModel.h"

class Grid : public Environment {

	public:

		Grid();
	
		~Grid();
	
		void CustomInit();
	
		void Reset();
	
		void Init(VariablesInfo* pActions);
	
		void AppendToInit(VariablesInfo* pActions);
	
		float GetMaxReward();
	
		float EvaluateActions(string AgentName);
	
		bool IsFeasibleAction(Variable* pAction, string AgentName);
		
		void printGrid(VariablesInfo* aGrid[GRID_SIZE][GRID_SIZE]); 
	
		VariablesInfo* GetStateDescription();

		VariablesInfo* GetPartialStateDescription();
	
		string ToString();
		
		    /**
     * Get the pointer of the transition model
     */
    const TransitionModel*const GetTransitionModel(); 
    
    /**
     * Checks if a move U,D,R,L is avalilable according to the environment present state
     */
    bool AvailMove(unsigned int i, unsigned int action_value,
		       vector <unsigned int> cur_row,  vector <unsigned int> cur_col);

    /**
     * Checks if a move U,D,R,L is avalilable according to the environment present state
     */
    bool AvailMove( vector <unsigned int> state);
    
    /**
     * Computes the joint index
     * @param size the max magnitude that the variables in vector can assume NON ZERO BASED
     */
    unsigned int  ComputeJointIndex  (vector <unsigned> vector, unsigned int size);

    /**
     * Transoforms from an index to a vector 
     * @param size the number of elements of the resulting vector
     * @param base the max magnitude that the variables in vector can assume
     */
    vector <unsigned> GetFromIndex(unsigned int index, unsigned int size, unsigned base);
		

	private:

		VariablesInfo* grid[GRID_SIZE][GRID_SIZE];

		VariablesInfo* originalGrid[GRID_SIZE][GRID_SIZE];

		VariablesInfo* gainGrid[GRID_SIZE][GRID_SIZE];

		VariablesInfo* copyGrid[GRID_SIZE][GRID_SIZE];

		bool isTheEndOfThisStep;

		float SimpleTokenPayoff(string AgentName);

		float CoinPayoff(string AgentName);

		float GenericPayoff(string AgentName, VariablesInfo* grid[GRID_SIZE][GRID_SIZE]);

		float GenericPayoff(VariablesInfo* grid[GRID_SIZE][GRID_SIZE]);

		void RemoveAgent(string AgentName, VariablesInfo* grid[GRID_SIZE][GRID_SIZE]);

		void RestoreTokensTakenByAgent(string AgentName, VariablesInfo* grid[GRID_SIZE][GRID_SIZE]);

		void UpdateStateDescription();

		void UpdatePartialStateDescription();

		void BackupEnvironmentConfiguration(VariablesInfo* aGrid[GRID_SIZE][GRID_SIZE]);

		void RestoreEnvironmentConfiguration(VariablesInfo* aGrid[GRID_SIZE][GRID_SIZE]);

		void AssertConditionsOnGrid(VariablesInfo*const grid[GRID_SIZE][GRID_SIZE]);

    /**
     * Generates the tranistion model for the GW
     */
    void GenerateTransitionModel ( );
    
     /**
     * constructs a vector of available joint moves
     */
    vector <unsigned>  AvailJointMoves( vector <unsigned int> state);

    /**
     * computes the next state
     */
    unsigned int ComputeNextState( vector <unsigned int> state, unsigned int jointAction);

    /**
     * computes the next state
     */
    vector <unsigned> ComputeNextState( vector <unsigned int> state, vector <unsigned> jointAction);

    /**
     * Pointer to the transition model
     */
    TransitionModel transitionModel;
};

#endif // GRID_H
