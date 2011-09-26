#ifndef TRANSITION_MODEL_H
#define TRANSITION_MODEL_H

// SYSTEM INCLUDES
#include <string>
//#include <xercesc/dom/DOM.hpp>
#include <map>

#include "VariablesInfo.h"
#include "Variable.h"
#include "TransitionModel.h"

/**
 * @brief Implements the TransitionModel abstract class as a map whose first key
 * is the starting state. Is an implementation of the classic model
 * State->Action->State
 */
class TransitionModel
{

public:
    /**
     * Empty constructor.
     */
    TransitionModel ( );

    
    /**
     * Virtual Destructor
     */
    virtual ~TransitionModel ( );
 
    /**
     * Return the probability of the requested transition.
     * @param prevState the previous state (in feature space)
     * @param action    the executed action
     * @param curState  the current state (in feature space)
     * @return the probability of the transition P(s,a,s'). If the transition
     *         does not exist a probability of 0.0 is returned.
     */
    virtual float  GetProb(unsigned int prevState,
                           float action,
                           unsigned int curState) const;
 
    /**
     * Return the probability to get to curState from prevState according
     * to the specified policy (i.e., probability distribution over actions).
     * The state transition probability P(s,s') is computed as
     * \f[
     * P(s, s^{I}) = \sum_{a_{i} \in A(s)} {\pi(s, a_{i}) \cdot P(s, a_{i}, s^{I})}
     * \f]
     * @param prevState the previous state (in feature space)
     * @param curState  the current state (in feature space)
     * @param policy    the probability distribution over actions, the size
     *                  of the vector must be the same as the number of
     *                  the actions available in prevState
     */
    
    virtual void  SetProbability(unsigned int prevState,
                                 float action,
                                 unsigned int curState,
                                 float prob);
                                 
     /**
     * Return the size of the state space
     */
    
    virtual unsigned  Size();
    /**
     * Return the list of all the transitions in the transition model and
     * their probabilities.
     * @param n the number of spaces needed to indent the output
     * @return the string containing all the information about the transition model
     */
    virtual string  ToString (unsigned int i = 0) const;

    /**
     * Return the list of all the transitions in the transition model and
     * their probabilities.
     * @param n the number of spaces needed to indent the output
     * @return the string containing all the information about the transition model
     */
    virtual string  ToShortString (unsigned int i = 0) const;

protected:

    /**
     * The transtion model function T: SxAxS --> R, i.e. given the previous
     * state, the action and the current state returns the probability of
     * such a transition.
     */
    map<unsigned int, map<float, map<unsigned int, float> > > mProb;

private:

};

#endif // TRANSITION_MODEL_H
