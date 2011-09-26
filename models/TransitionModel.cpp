// SYSTEM INCLUDES
#include <sstream>
#include <iostream>
#include <assert.h>
#include <time.h>
#include <iomanip>
#include <cmath>

// LOCAL INCLUDES
#include "TransitionModel.h"

TransitionModel::TransitionModel ()
{}


TransitionModel::~TransitionModel ()
{

}

unsigned TransitionModel::Size()
{
	return mProb.size();
}

float  TransitionModel::GetProb(unsigned int prevState,
                                   float action,
                                   unsigned int curState) const
{
    map<unsigned int, map<float, map<unsigned int, float> > >::const_iterator prev_state_it;
    prev_state_it = mProb.find(prevState);
    if (prev_state_it == mProb.end())
    {
        return 0.0;
    }

    map<float, map<unsigned int, float> >::const_iterator act_it;
    act_it = prev_state_it->second.find(action);
    if (act_it == prev_state_it->second.end())
    {
        return 0.0;
    }

    map<unsigned int, float>::const_iterator cur_state_it;
    cur_state_it = act_it->second.find(curState);
    if (cur_state_it == act_it->second.end())
    {
        return 0.0;
    }

    return cur_state_it->second;
}

void  TransitionModel::SetProbability(unsigned int prevState,
        float action,
        unsigned int curState,
        float prob)
{
    mProb[prevState][action][curState] = prob;
}




string TransitionModel::ToString (unsigned int i) const
{
//#if TOSTRING
    ostringstream str;
    //str << TransitionModel::ToString(i) << endl;
    map<unsigned int, map<float, map<unsigned int, float> > >::const_iterator state_it;
    for (state_it = mProb.begin(); state_it != mProb.end() && mProb.size() != 0; state_it++)
    {
        map<float, map<unsigned int, float> >::const_iterator act_it;
        for (act_it = state_it->second.begin(); act_it != state_it->second.end(); act_it++)
        {
            map<unsigned int, float>::const_iterator next_state_it;
            for (next_state_it = act_it->second.begin(); next_state_it != act_it->second.end(); next_state_it++)
            {
                str << "(" << state_it->first << "," << act_it->first << "," << next_state_it->first << ") = " << next_state_it->second << endl;
            }
        }
    }
    return str.str();
//#else

//    return "";
//#endif
}

string  TransitionModel::ToShortString (unsigned int i) const
{
#if TOSTRING
    ostringstream str;

    map<unsigned int, map<float, map<unsigned int, float> > >::const_iterator state_it;
    for (state_it = mProb.begin(); state_it != mProb.end() && mProb.size() != 0; state_it++)
    {
        map<float, map<unsigned int, float> >::const_iterator act_it;
        for (act_it = state_it->second.begin(); act_it != state_it->second.end(); act_it++)
        {
            map<unsigned int, float>::const_iterator next_state_it;
            for (next_state_it = act_it->second.begin(); next_state_it != act_it->second.end(); next_state_it++)
            {
                str << state_it->first << "\t" << act_it->first << "\t" << next_state_it->first << "\t" << next_state_it->second << endl;
            }
        }
    }
    return str.str();
#else

    return "";
#endif
}

