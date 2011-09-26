#ifndef ENVIRONMENT_H
#define ENVIRONMENT_H

// SYSTEM INCLUDES
#include <string>

// PROJECT INCLUDES
#include <mystl.h>

// LOCAL INCLUDES
#include "VariablesInfo.h"

/**
 * Class Environment
 * This class simulates the dynamic of an MDP.
 */
class Environment
{
public:
    /**
     * Empty Constructor.
     */
    Environment ( );

    /**
     * Virtual Destructor.
     */
    virtual ~Environment ( );

    /**
     * Initialize the environment on the basis of what specified in the 
     * xml fragment.
     * @param pEnv the pointer to the xml fragment
     * @param classes the list of all the classes available
     */
    virtual void  Init (VariablesInfo* pActions);

    /**
     * Reset the environment to its initial condition.
     */
    virtual void  Reset ( ) = 0;

    /**
     * Reset the environment to a random condition.
     */
    virtual void  ResetRandom ( ) = 0;

    /**
     * Execute the specified actions.
     * @param pActions the structure that contains all the details about
     *                 the actions to be executed
     */
    virtual void  ExecuteActions (VariablesInfo* pActions) = 0;

    /**
     * Return the data structure that describes the state space of the 
     * environment.
     * NOTE: it is possible to modify the value of the state variables but
     * the pointer is owned by the Environment itself and it is deleted when
     * the destructor of Environemnt class is called.
     * @return the pointer to the state space description
     */
    VariablesInfo* GetStateDescription ( );

    /**
     * Return all the information about the environment
     * @param i the number of spaces needed to indent the output
     * @return the string containing all the information about the environment
     */
    virtual string  ToString (unsigned int i = 0) const;

    /**
     * Return some information about the environment
     * @param i the number of spaces needed to indent the output
     * @return the string containing some information about the environment
     */
    virtual string  ToShortString (unsigned int i = 0) const;

protected:
    /**
        * The data structure that describes the state space of the environment
        */
    VariablesInfo* mpStateInfo;
private:
};

#endif // ENVIRONMENT_H
