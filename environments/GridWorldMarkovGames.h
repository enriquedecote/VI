#ifndef GRID_WORLD_MARKOVGAMES_H_
#define GRID_WORLD_MARKOVGAMES_H_

// LOCAL INCLUDES
#include "GridWorldWall.h"
#include "TransitionModel.h"

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
 * world.
 */
class GridWorldMarkovGames : public GridWorldWall
{
public:
    /**
     * Empty Constructor.
     */
    GridWorldMarkovGames ( );

    /**
     * Virtual Destructor.
     */
    virtual ~GridWorldMarkovGames ( );

    /**
     * Initialize the environment on the basis of what specified in the 
     * xml fragment.
     * @param pDomState the pointer to the xml fragment
     */
    virtual void  Init (const DOMElement*const pEnv, map<ClassType,Classes> classes);

    /**
     * Moves  the agent to the requested postition if the movement is available 
     * according to the game's dynamics (Littman's soccer domain).
     */
    virtual vector < vector<unsigned> >  ChangePosition (unsigned int i, unsigned int action_value, 
					    vector <unsigned int> cur_row,  vector <unsigned int> cur_col,
					    string *row, string *col );

    /**
     * Get the pointer of the transition model
     */
    virtual const SASTransitionModel*const GetTransitionModel(); 
    
    /**
     * Checks if a move U,D,R,L is avalilable according to the environment present state
     */
    virtual bool AvailMove(unsigned int i, unsigned int action_value,
		       vector <unsigned int> cur_row,  vector <unsigned int> cur_col);

    /**
     * Checks if a move U,D,R,L is avalilable according to the environment present state
     */
    virtual bool AvailMove( vector <unsigned int> state);
    
    /**
     * Computes the joint index
     * @param size the max magnitude that the variables in vector can assume NON ZERO BASED
     */
    virtual unsigned int  ComputeJointIndex  (vector <unsigned> vector, unsigned int size);

    /**
     * Transoforms from an index to a vector 
     * @param size the number of elements of the resulting vector
     * @param base the max magnitude that the variables in vector can assume
     */
    virtual vector <unsigned> GetFromIndex(unsigned int index, unsigned int size, unsigned base);
  
    /**
     * Reset the environment to its initial condition.
     */
    virtual void  Reset ( );

    /**
     * Reset the environment to a random condition.
     */
    virtual void  ResetRandom ( );

    /**
     * Reset the environment for executing the test.
     */
    virtual void  ResetForTest ( );

    /**
     * Execute the specified actions.
     * @param pActions the structure that contains all the details about
     *                 the actions to be executed
     */
    virtual void  ExecuteActions (const VariablesInfo*const pActions);

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

protected:

    /**
     * Generates the tranistion model for the GW
     */
    virtual void GenerateTransitionModel ( );  

    /**
     * constructs a vector of available joint moves
     */
    virtual vector <unsigned>  AvailJointMoves( vector <unsigned int> state);

    /**
     * computes the next state
     */
    virtual unsigned int ComputeNextState( vector <unsigned int> state, unsigned int jointAction);

    /**
     * computes the next state
     */
    virtual vector <unsigned> ComputeNextState( vector <unsigned int> state, vector <unsigned> jointAction);

    /**
     * Pointer to the transition model
     */
    SASTransitionModel transitionModel;

    map<pair<int,int>,bool> mHoles;

    /**
     * The number of rows in the grid world
     */
    unsigned int mNumRows;

    /**
        * The number of columns in the grid world
        */
    unsigned int mNumCols;

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
    /**
     * The number of agents in this environment
     */
    unsigned int mNumAgents;
};

POLIMI_RL_TOOLKIT_NAMESPACE_END

#endif /*GRIDWORLD_MARKOVGAMES_H_*/
