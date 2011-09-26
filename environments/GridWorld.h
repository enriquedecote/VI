#ifndef GRIDWORLD_H
#define GRIDWORLD_H

#include "Environment.h"
#include "../models/TransitionModel.h"


// Variables
#define ROW "Row"
#define COL "Col"

// Action Ids
#define D 0 // Down
#define R 1 // Right
#define U 2 // Up
#define L 3 // Left

/**
 * Class GridWorld
 * This class simulates the dynamic of a simple discrete state and action grid
 * world. The four possibles actions are up, right, down, left. Each one has an
 * error probability that you can set via xml. According to this probability
 * the agent can execute another different action choosen random. For instance
 * if the agent choose action up and action up in this environment has an error
 * probability of 0.3, then with p=0.7 the agent will go up, and with p=0.3
 * will choose anothe action. So it will go:<br>
 * - up		with p=0.7
 * - right	with p=0.1
 * - down	with p=0.1
 * - left	with p=0.1
 */
class GridWorld : public Environment
{
public:
    /**
     * Empty Constructor.
     */
    GridWorld ();
    /**
     * Constructor.
     */
    GridWorld (char game);

    /**
     * Virtual Destructor.
     */
    virtual ~GridWorld ( );

    /**
     * Initialize the environment on the basis of what specified in the
     * xml fragment.
     * @param pDomState the pointer to the xml fragment
     */
    virtual void  Init (VariablesInfo* pActions);

    /**
     * Reset the environment to its initial condition.
     */
    virtual void  Reset ( );

    /**
       * Return the size of the state space
       */
    virtual unsigned Size() const;

    /**
     * Reset the environment to a random condition.
     */
    virtual void  ResetRandom ( );

    /**
     * Execute the specified actions.
     * @param pActions the structure that contains all the details about
     *                 the actions to be executed
     */

    /**
    * Checks if a move U,D,R,L is avalilable according to the environment present state
    * @param i the agent
    */

    virtual bool AvailMove(unsigned int i, unsigned int action_value,
                           vector <unsigned int> cur_row,  vector <unsigned int> cur_col) const;

	virtual bool NoOppInCell(unsigned int i, unsigned int action_value,
                           vector <unsigned int> cur_row,  vector <unsigned int> cur_col) const;
    /**
     * Checks if a move U,D,R,L is avalilable according to the environment present state
     */
    virtual bool AvailMove( vector <unsigned int> state) const;

    /**
    * The reward function: given a state and a joint action, returns the t+1 reward
    */
    virtual float Reward(unsigned agent, unsigned state, unsigned actions);

    /**
    * Set the goal for an agent
    */
    virtual void SetGoal(unsigned int agent, pair <unsigned, unsigned> coord, float reward);

    /**
     * Inserts other elements of the environment (tokens, walls)
     */
    virtual void AppendInit(VariablesInfo* pState);

    virtual void  ExecuteActions (VariablesInfo* pActions);

    /**
     * Return all the information about the grid world
     * @param n the number of spaces needed to indent the output
     * @return the string containing all the information about the grid world
     */
    virtual string  ToString (unsigned int i = 0) const;

    /**
     * Return some information about the grid world
     * @param n the number of spaces needed to indent the output
     * @return the string containing some information about the grid world
     */
    virtual string  ToShortString (unsigned int i = 0) const;

    virtual string  GetInfo (unsigned int i = 0) const;

    /**
     * Get the pointer of the transition model
     */
    virtual const TransitionModel*const GetTransitionModel();

    /**
     * constructs a vector of available joint moves
     */
    virtual vector <unsigned>  AvailJointMoves( vector <unsigned int> state) ;

    /**
     * constructs a vector of available joint moves
     */
    virtual vector <unsigned>  AvailJointMoves( unsigned int state)const;

    /**
     * Computes the joint index
     * @param size the max magnitude that the variables in vector can assume NON ZERO BASED
     * if x = [0:1:3], size = 4
     */
    virtual unsigned int  ComputeJointIndex  (vector <unsigned> vector, unsigned int size) const;

    /**
     * Computes the joint index
     * @param size the max magnitude that the variables in vector can assume NON ZERO BASED
     * if x = [0:1:3], size = 4
     */
    virtual unsigned int  ComputeJointIndex  (VariablesInfo* pVars) const;

    /**
        * Transoforms from an index to a vector 
        * @param size the number of elements of the resulting vector
        * @param base the max magnitude that the variables in vector can assume NON ZERO BASED
        * if x = [0:1:3], size = 4
        */
    virtual vector <unsigned> GetFromIndex(unsigned int index, unsigned int size, unsigned base) const;

    /**
        * Transoforms from an index to a vector 
        * @param size the number of elements of the resulting vector
        * @param base the max magnitude that the variables in vector can assume NON ZERO BASED
        * if x = [0:1:3], size = 4
        */
    virtual vector <unsigned> GetFromStateIndex(unsigned int index, unsigned int size, unsigned base) const;

    /**
        * Transoforms from an index to a vector 
        * @param pVars is the structure where the vars are stored
        * if x = [0:1:3], size = 4
        */
    virtual vector <unsigned> GetFromIndex(unsigned int index, VariablesInfo* pVars) const;

	virtual char GetGame() const;
	

protected:

    /**
     * Generates the tranistion model for the GW
     */
    virtual void GenerateTransitionModel ( );

    /**
        * computes the next state according to the given dynamics
        * dynamics: the first agent to move is chosen randomly
        * 
        */
    virtual vector<unsigned> ComputeNextState( vector <unsigned> state, unsigned jointAction, float* prob);

    /**
     * computes the next state
     */
    virtual vector <bool> ComputeNextState( vector <unsigned> state, unsigned jointAction, unsigned *state);

    virtual vector <unsigned> ComputeNextState( vector <unsigned int> state, vector <unsigned int> jointAction) const;


    /**
       * Moves  the agent to the requested postition if the movement is available 
       * according to the game's dynamics (Littman's soccer domain).
       */
    /*virtual vector < vector<unsigned> >  ChangePosition (unsigned int i, unsigned int action_value,
    	    vector <unsigned int> cur_row,  vector <unsigned int> cur_col,
    	    string *row, string *col );*/

    /**
     * Pointer to the transition model
     */
    TransitionModel transitionModel;

    /**
     * The grid world game
     */
    char mGame;


    /**
     * The number of rows in the grid world
     */
    unsigned int mNumAgents;
    
    float nullPay;

    /**
     * The number of rows in the grid world
     */
    unsigned int mNumRows;

    /**
     * The number of columns in the grid world
     */
    unsigned int mNumCols;

    /**
    * The number of states
    */
    unsigned int mNumStates;

    /**
    * The number of Joint actions
    */
    unsigned mNumJointActions;

    /**
     * The goals for agent 0
     */
    map <pair<unsigned,unsigned>,float>  mGoal0;

    /**
    * The goals for agent 1
    */
    map <pair<unsigned,unsigned>,float>  mGoal1;

    /**
     * The goals for agent 1
     */
    map <pair<unsigned,unsigned>,float>  mGoal2;

    /**
    * The number of the first row for each agent
    */
    map <pair<unsigned,unsigned>, float> mEndCell1;

    /**
     * The number of the first row for each agent
     */
    map <pair<unsigned,unsigned>, float> mEndCell2;

    /**
    * The number of the first row for each agent
    */
    vector<unsigned> mFirstRow;

    /**
     * The number of the last row for each agent
     */
    vector<unsigned> mLastRow;

    /**
     * The number of the first col for each agent
     */
    vector<unsigned> mFirstCol;

    /**
     * The number of the last column for each agent
     */
    vector<unsigned> mLastCol;

    /**
     * The initial row for each agent
     */
    vector<unsigned> mAgentRow;

    /**
     * The initial column for each agent
     */
    vector<unsigned> mAgentCol;


private:
};

#endif // GRIDWORLD_H
