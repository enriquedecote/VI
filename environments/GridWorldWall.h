#ifndef GRID_WORLD_WALL_H_
#define GRID_WORLD_WALL_H_

// LOCAL INCLUDES
#include "GridWorld.h"

// Variables
#define ROW "Row"
#define COL "Col"
//#define MAX 1
//#define MIN 0

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
class GridWorldWall : public GridWorld
{
public:
    /**
     * Empty Constructor.
     */
    GridWorldWall (char game);

    /**
     * Virtual Destructor.
     */
    virtual ~GridWorldWall ( );

    /**
     * A horizontal wall is a transition probability rom one cell (col1,row1)
     * to another cell (col2,row2), and can be extended to some length to the right
     */
    virtual void  SetHWall(unsigned col1, unsigned row1, unsigned col2, unsigned row2, unsigned lenght);
    
    /**
     * A vertical wall is a transition probability rom one cell (col1,row1)
     * to another cell (col2,row2), and can be extended to some length to the right
     */
    virtual void  SetVWall(unsigned col1, unsigned row1, unsigned col2, unsigned row2, unsigned lenght);


    /**
     * The list of all the positions occupied by a wall
     */
    map<pair<unsigned,unsigned>, map< pair<unsigned,unsigned>, unsigned> > mWall;
    
    /**
    * The reward function: given a state and a joint action, returns the t+1 reward
    */
    virtual float Reward(unsigned agent, unsigned state, unsigned actions);
    
protected:

    /**
     * Generates the tranistion model for the GW
     */
    virtual void GenerateTransitionModel ( );
    
    virtual vector <unsigned> PurgeStatesWalls(vector <unsigned>state1, vector <unsigned>s2, float *prob);
	
	float wallProb;

};


#endif /*GRI_DWORLD_WALL_H_*/
