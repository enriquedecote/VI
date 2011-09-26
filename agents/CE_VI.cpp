#include <iostream>
#include <algorithm>
#include <iomanip>
#include <sstream>
#include <string>
#include <fstream>
#include <math.h>
#include <cmath>
#include <stdlib.h>
#include <assert.h>
#include "glpk.h"
#include "../include/CE_VI.h"

using namespace std;

CE_VI::CE_VI(string name,
             unsigned int pos,
             unsigned int numAgents,
             VariablesInfo*const admissibleActions,
             float discountFactor,
             float maxReward,
             bool isVerbose,
             char algorithm,
             char opt,
             float w,
             Environment* environment) : VI(name,
                                                    pos,
                                                    numAgents,
                                                    admissibleActions,
                                                    discountFactor,
                                                    maxReward,
                                                    isVerbose,
                                                    environment)

{
    mNumAgents = numAgents;
    mDiscountFactor = mdiscountFactor;
    mpEnvironment = (GridWorld*)environment;
    mNumActions = admissibleActions->size();
    mAlgorithm = algorithm;
    mOpt = opt;
    mWeight = w;
    mDefendAgent = 1;
    cout << "admissibleActions: " << mNumActions << endl;
    mNumJointActions = (unsigned) pow((float)mNumActions, (float)numAgents);
    mNumStates = mpEnvironment->Size();
    for (unsigned i = 0; i< mNumStates; i++)
    {
        mJointVTable[i].first = 0.0;
        mJointVTable[i].second = 0.0;
    }
    if(mpEnvironment->GetGame()=='P')
        mInitState = 291;
    else if(mpEnvironment->GetGame()=='X')
        mInitState = 91;
    else if(mpEnvironment->GetGame()=='A')
    	mInitState = 76;//for a 10x1 grid
    else
        mInitState = 78;
}



CE_VI::~CE_VI ()
{
    cout << "CE_VI::~CE_VI" << endl;
}

void CE_VI::SetTransitionModel (const TransitionModel*const pTransitionModel)
{
    mpTransitionModel = (TransitionModel*)pTransitionModel;
}

void CE_VI::Reset()
{
	mJointQTable.clear();
	mJointVTable.clear();	
}

void CE_VI::ParetOptimalSearch()
{
	double a0 = 0;
	double a1 = 0;
    double b0,b1;
    double v0 =0.0;
    double v1 =0.0;
	mWeight = 0.0;
	
	
}

void CE_VI::BuildConvexHull()
{
    debug.open( "../results/VI/debug.log", ios::app );
    //debug  << PrintInfo();
	double a0 = 0;
	double a1 = 0;
    double b0,b1;
    double v0 =0.0;
    double v1 =0.0;
	mWeight = 0.0;
	
	mDefendAgent = 0;
    ComputeVI('M');
    v0 = mJointVTable[mInitState].first;
    Reset();
    
    mDefendAgent = 1;
    ComputeVI('M');
    v1 = mJointVTable[mInitState].second;
    Reset();
    
    debug << "v = (" << v0 << "," << v1 << ")" << endl;

    while(mWeight < 1)
    {
    	Reset();
        ComputeVI('T');
		b0 = mJointVTable[mInitState].first;
		b1 = mJointVTable[mInitState].second;
		mWeight += 0.01;
		if(a0 != b0 || a1 != b1)
			debug << mWeight << ":" << b0 << "\t" << b1 << endl;
		a0 = b0;
		a1 = b1;
    }
	
}

void CE_VI::Folk()
{
	//double weight = 0.0;
    double delta = 1.0;
    unsigned p = 0;
    bool stop = false;
    double error = 1/(double)64;
    double v0 =0.0;
    double v1 =0.0;
    double c0;
    double c1;

	mDefendAgent = 0;
    ComputeVI('M');
    v0 = mJointVTable[mInitState].first;
    Reset();
    
    mDefendAgent = 1;
    ComputeVI('M');
    v1 = mJointVTable[mInitState].second;

    
    mWeight = 0.0;

    while((float)delta >= (float)error && !stop)
    {
    	Reset();
        delta = pow(2,-(double)p);
        ComputeVI('T');
		c0 = mJointVTable[mInitState].first;
		c1 = mJointVTable[mInitState].second;

        //cout <<c0 << "," <<c1<<endl;
        if((float)c0>(float)v0 && (float)c1>(float)v1)//quadrant I: a Nash is found!
        {
                debug << "Nash solution found with weight("<< mWeight<<") ";
                debug <<v0<<","<<v1<< "->:" <<c0<<","<< c1<<endl;
            stop = true;
        }
        else if((float)c0==(float)v0 && (float)c1==(float)v1)//minimax point
        {
                debug << "Minimax solution found ("<< mWeight<<") ";
                debug <<v0<<","<<v1<< "->:" <<c0<<","<< c1<<endl;
        }
        else if((float)c0<=(float)v0 && (float)c1>=(float)v1)//quadrant II
        {
                debug << "Solution in quadrant II ("<< mWeight<<") ";
                debug <<v0<<","<<v1<< "->:" <<c0<<","<< c1<<endl;
            if((mWeight+delta)<=1)
            	mWeight += delta;
        }
        else if((float)c0>=(float)v0 && (float)c1<=(float)v1)//quadrant IV
        {
                debug << "Solution in quadrant IV("<< mWeight<<") ";
                debug <<v0<<","<<v1<< "->:" <<c0<<","<< c1<<endl;
            if(mWeight>=delta)
            	mWeight -= delta;
        }
        else if((float)c0 < (float)v0 && (float)c1< (float)v1)//quadrant III IT'S A BUG
        {
            cout << "BUG: RESULT OF MDP("<< mWeight<< ") IS IN QUADRANT III"<<endl;
            cout << "defend:" << v0<<","<<v1<<endl;
            cout <<"strat:" <<c0 <<","<< c1<<endl;
            exit(1);
        }
        p++;
    }
}


void CE_VI::UpdateValue (float* rNewValue,  float payoff,  float bestValue)
{
    //if (learningRate <= 0.0)
    //	learningRate = mLearningRate;
    *rNewValue = 0;//(1-learningRate)*(*rNewValue) + learningRate*(payoff + mDiscountFactor*bestValue);
}

void CE_VI::Execute()
{
	unsigned T=0;
	ofstream fileProb( "../results/VI/probs.log", ios::app );
    fileProb << PrintInfo();
    ofstream fileQ( "../results/VI/qtable.log", ios::app );
    fileQ  << PrintInfo();
    ofstream fileV( "../results/VI/vtable.log", ios::app );
    fileV  << PrintInfo();
    //ofstream debug( "../results/VI/debug.log", ios::app );
    debug.open( "../results/VI/debug.log", ios::app );
    debug  << PrintInfo();
    
    
    if(mAlgorithm == 'T')
    {
    	Folk();
    }
    else
    	T=(unsigned)ComputeVI(mAlgorithm);
    	
    fileQ << "Qtable:" << endl << PrintQTable();
    fileV <<  "Vtable:"<<endl<<PrintVTable();
    fileProb << "Probs:" << endl << PrintJointStrat();
    fileQ.close();
    fileV.close();
    fileProb.close();
    
	 
	cout << "iterations:"<<T;
    cout << endl << "++++++++++++++++++++++++++++++"<<endl;
    cout << "Vtable initial state values found!" << endl;
    cout << "V1("<<mInitState<<"):"<< mJointVTable[mInitState].first<< endl;
    cout << "V2("<<mInitState<<"):"<< mJointVTable[mInitState].second<< endl;
    cout << "++++++++++++++++++++++++++++++"<<endl<<endl;

	string tmp;
    tmp.append(WriteStrategy(mInitState));
    cout << tmp;
    
    debug << endl << "++++++++++++++++++++++++++++++"<<endl;
    debug << "V1("<<mInitState<<"):"<< mJointVTable[mInitState].first<< endl;
    debug << "V2("<<mInitState<<"):"<< mJointVTable[mInitState].second<< endl;
    debug << tmp;
    debug.close();
}

float CE_VI::ComputeVI (char algorithm)
{
    float error1 = 0;
    float error2 = 0;
    float stop = 0.00005;
    unsigned T =0;
    pair <float,float> r;
    float r1=0;
    float r2=0;
    pair <float,float> sumNextState(0.0,0.0);
    pair <float,float> nextQ(0.0,0.0);

    do
    {
        //debug << endl << endl<<"**************START OF TRIAL**************"<<endl;
        // (1)
        for (unsigned i = 0; i < mNumStates; i++)// for all states
        {
            //cout << "State:" << i;
            //debug <<endl<<"**********************************STATE:" << i<< endl;
            map <unsigned, pair<float,float> > payoff;
            for (unsigned j=0; j < mNumJointActions; j++)//for all actions
            {

                //debug << " ->action:" << j<<endl;
                //r.first = mpEnvironment->Reward(0,i,j);
                //r.second = mpEnvironment->Reward(1,i,j);
                r1 = mpEnvironment->Reward(0,i,j);
                r2 = mpEnvironment->Reward(1,i,j);
                //if((j== 0|| j== 1|| j== 2|| j== 3 || j== 8|| j== 9|| j== 10|| j== 11)&& (r2!=0))
                	//debug << i<<","<<j<<":"<<r1<<","<<r2<<endl;
                //float sum = 0;
                for(unsigned x = 0; x<mNumStates; x++)//next state
                {
                    sumNextState.first += mpTransitionModel->GetProb(i,j,x)*mJointVTable[x].first;
                    sumNextState.second += mpTransitionModel->GetProb(i,j,x)*mJointVTable[x].second;
                    //sum += mpTransitionModel->GetProb(i,j,x);
                    //if(sum > 1)
                    //debug << "|"<<"("<< i <<","<<j<<"):"<<sum << endl;
                    //if(i == 229)
                    //debug << " Rewards (" << r.first<<","<<r.second<<")"<<endl;
                    //vector <unsigned> temp = mpEnvironment->GetFromIndex(i,4,3);
                    /*if( i== 32 && mpTransitionModel->GetProb(i,j,x)!=0)
                {
                        //assert(r.first==0&&r.second==0);
                        debug <<"("<< i <<","<<j<<")->"<<x<<":"<<mpTransitionModel->GetProb(i,j,x)<<"|";
                        debug << mJointVTable[x].first<<","<<mJointVTable[x].second;
                        //debug << ": "<< mJointVTable[x].first;
                        //debug << "(" << mpTransitionModel->GetProb(i,j,x)<< ")"<<endl;
                        //cout << ": " <<temp.at(0)<< "," <<temp.at(1)<< "," <<temp.at(2)<< "," <<temp.at(3)<< endl;
                        //cout << " ->action:" << j;
                        //vector <unsigned> temp2 = mpEnvironment->GetFromIndex(j,2,4);
                        //cout << ":" << temp2.at(0) << ","<< temp2.at(1) << endl;
                        debug << "(" << mDiscountFactor*sumNextState.first << "," << mDiscountFactor*sumNextState.second << ")"<<endl;
                }*/
                    // }
                    //cout << "(" << sumNextState.first << "," << sumNextState.second << ")" << endl;
                }

                nextQ.first = r1 + mDiscountFactor*sumNextState.first ;
                nextQ.second = r2 + mDiscountFactor*sumNextState.second;
                //if(i==91)
                //    debug << endl<<"nextQ.first: "<<nextQ.first <<";  nextQ.second: "<<nextQ.second;
                payoff[j] = nextQ;
                sumNextState.first = 0;
                sumNextState.second = 0;

            }//end all actions
            mJointQTable[i] = payoff;

        }

        // (2)
        //cout << " "<<mpTransitionModel->ToString(1);
        for (unsigned i = 0; i < mNumStates; i++)// for all states
        {
            //debug<<endl<<"**********************************STATE:" << i<< endl;
            switch (algorithm)
            {
            case 'C':
                switch (mOpt)
                {
                case 'u':
                    uCE(i);
                    break;
                case 'e':
                    eCE(i);
                    break;
                case 'r':
                    rCE(i);
                    break;
                }
                break;
            case 'F':
                Friend(i);
                break;
            case 'M':
                Defend(i,mDefendAgent);
                break;
            case 'T':
                TB(i,mWeight);
                break;
            default:
                break;
            }
            /*float sum =0;
            for (unsigned var = 0; var < mNumJointActions; var++) 
            sum += mJointStrat[i][var];
            if(sum !=1 &&sum != 0)
        {
            cout << "ERROR! "<<sum<< " IS NOT A FEASIBLE PROB. DISTRIBUTION"<< endl; 
            assert(sum == 1);
        }*/

        }

        // (3)
        vector <unsigned> jointMoves;
        pair <float, float> sumActions;
        sumActions.first = 0.0;
        sumActions.second = 0.0;
        //debug<< endl<<"*****V vals updates***"<< endl;
        for (unsigned i = 0; i < mNumStates; i++)// for all states
        {
            //debug << endl<<endl<<"**********************************STATE:" << i<< endl;
            jointMoves = mpEnvironment->AvailJointMoves(i);
            for (unsigned j = 0; j<mNumJointActions;j++)
            {
                sumActions.first += mJointStrat[i][j]*mJointQTable[i][j].first;
                sumActions.second += mJointStrat[i][j]*mJointQTable[i][j].second;
                //if(i==83)
                //   debug <<"p("<<i<<","<<j<<"):"<<mJointStrat[i][j]<<"|Q:"<<mJointQTable[i][j].first<<","<<mJointQTable[i][j].second<<endl;
                /*if (sumActions.first > 100 ||sumActions.second > 100)
            {
                    vector <unsigned> state = mpEnvironment->GetFromIndex(i,4,3);
                    vector <unsigned> move = mpEnvironment->GetFromIndex(j,2,4);
                    debug <<"state " << i << ": " <<state[0]<<","<<state[1]<<","<<state[2]<<","<<state[3]<<endl;
                    debug <<"move " << j << ": " <<move[0]<<","<<move[1]<<endl;
                    debug << "V("<<i<<","<<j<<")= "<<sumActions.first<<","<<sumActions.second<<endl;
            }*/
            }
            if(fabs(mJointVTable[i].first - sumActions.first) > error1)
                error1 = fabs(mJointVTable[i].first - sumActions.first);
            if(fabs(mJointVTable[i].second - sumActions.second))
                error2 = fabs(mJointVTable[i].second - sumActions.second);
            mJointVTable[i].first = sumActions.first;
            mJointVTable[i].second = sumActions.second;
            //if(i==32)
             // debug << "V:"<<mJointVTable[i].first<<","<<mJointVTable[i].second<<endl;
            sumActions.first = 0;
            sumActions.second = 0;
            //if(i==91)
            //   debug <<"V("<<i<<")"<<mJointVTable[i].first<<","<<mJointVTable[i].second<<endl;
        }
        T++;
    }
    while(error1>stop && error2>stop && T<200);
    //while(T<200);

	return T;
    //return error1+error2;
}

void CE_VI::TB(unsigned state, float weight)
{
    MDP(state,weight);
}

void CE_VI::TB(unsigned state)
{
    double weight = 0.0;
    double delta = 1.0;
    unsigned p = 0;
    bool stop = false;
    double error = 1/(double)64;
    double defend0 =0.0;
    double defend1 =0.0;
    double sumActions0;
    double sumActions1;


    if(state==mInitState)
    {
        for (unsigned j = 0; j < mNumJointActions; ++j)
            debug <<"|"<<j<<":"<<mJointQTable[state][j].first<<","<<mJointQTable[state][j].second;
        debug <<endl;
    }

    Defend(state,0);
    for (unsigned j = 0; j<mNumJointActions;j++)
        defend0 += mJointStrat[state][j]*mJointQTable[state][j].first;

    Defend(state,1);
    for (unsigned j = 0; j<mNumJointActions;j++)
        defend1 += mJointStrat[state][j]*mJointQTable[state][j].second;

    while((float)delta >= (float)error && !stop)
    {
        delta = pow(2,-(double)p);
        MDP(state,weight);
        sumActions0 = 0.0;
        sumActions1 = 0.0;
        for (unsigned j = 0; j<mNumJointActions;j++)
        {
            sumActions0 += mJointStrat[state][j]*mJointQTable[state][j].first;
            sumActions1 += mJointStrat[state][j]*mJointQTable[state][j].second;
        }

        //cout <<sumActions0 << "," <<sumActions1<<endl;
        if((float)sumActions0>(float)defend0 && (float)sumActions1>(float)defend1)//quadrant I: a Nash is found!
        {
            if(state==mInitState)
            {
                debug << "Nash solution found with weight("<< weight<<") ";
                debug <<defend0<<","<<defend1<< "->:" <<sumActions0<<","<< sumActions1<<endl;
            }
            stop = true;
        }
        else if((float)sumActions0==(float)defend0 && (float)sumActions1==(float)defend1)
        {
            if(state==mInitState)
            {
                debug << "Minimax solution found ("<< weight<<") ";
                debug <<defend0<<","<<defend1<< "->:" <<sumActions0<<","<< sumActions1<<endl;
            }
        }
        else if((float)sumActions0<=(float)defend0 && (float)sumActions1>=(float)defend1)//quadrant II
        {
            if(state==mInitState)
            {
                debug << "Solution in quadrant II ("<< weight<<") ";
                debug <<defend0<<","<<defend1<< "->:" <<sumActions0<<","<< sumActions1<<endl;
            }
            if((weight+delta)<=1)
            	weight += delta;
        }
        else if((float)sumActions0>=(float)defend0 && (float)sumActions1<=(float)defend1)//quadrant IV
        {
            if(state==mInitState)
            {
                debug << "Solution in quadrant IV("<< weight<<") ";
                debug <<defend0<<","<<defend1<< "->:" <<sumActions0<<","<< sumActions1<<endl;
            }
            if(weight>=delta)
            	weight -= delta;
        }
        else if((float)sumActions0 < (float)defend0 && (float)sumActions1< (float)defend1)//quadrant III IT'S A BUG
        {
            cout << "BUG: RESULT OF MDP("<< weight<< ") IS IN QUADRANT III"<<endl;
            cout << "defend:" << defend0<<","<<defend1<<endl;
            cout <<"strategy:" <<sumActions0 <<","<< sumActions1<<endl;
            cout << "State:" << state<<endl;
            exit(1);
        }
        p++;
    }

}

void CE_VI::MDP(unsigned state, double weight)
{
    vector <unsigned> jointMoves;
    vector <unsigned> bestMoves;
    unsigned jointMove = 0;
    double bestActionVal = -100;
    double tempVal = -100;

    //jointMoves.clear();
    //jointMoves = mpEnvironment->AvailJointMoves(mpEnvironment->GetFromStateIndex(state,4,3));

    for(unsigned i=0;i<mNumJointActions; i++)
    {

        //cout << mJointQTable[state][i].first << "," << mJointQTable[state][i].second<<endl;

        jointMove = i;
        //jointMoves.pop_back();
        tempVal = weight*mJointQTable[state][jointMove].first + (1-weight)*mJointQTable[state][jointMove].second;
        if(tempVal > bestActionVal)
        {
            bestMoves.clear();
            assert(bestMoves.empty());
            bestMoves.push_back(jointMove);
            bestActionVal = tempVal;
            //if(state==78)
            // cout << "bestAction: "<<jointMove<<": "<<bestActionVal<< endl;
        }
        else
        {
            if(tempVal == bestActionVal)
                bestMoves.push_back(jointMove);
        }
        // if(state==78)
        //  cout << "Num. best moves: "<< bestMoves.size()<< endl;
    }

    //make the probability distribution
    for(unsigned j=0; j< mNumJointActions; j++)
        mJointStrat[state][j] = 0;

    //unsigned size = bestMoves.size();
    //while(!bestMoves.empty())
    //{
    //mJointStrat[state][bestMoves.back()] = 1/(float)size;
    mJointStrat[state][bestMoves.back()] = 1;//picks the first from those maximizing joint actions
    //cout << endl<< "Joint action:"<<bestMoves.back();
    //bestMoves.pop_back();
    //}
}

void CE_VI::eCE(unsigned i)
{
    double sum = 0.0;
    double temp_sum = 1000;
    vector <double> pi;
    vector <double> temp;
    for (unsigned n = 0;  n < 2; n++)
    {
        sum = 0.0;
        temp = CE(i,n);
        //debug << "AGENT:" << n << endl;
        //for (unsigned j = 0; j < mNumJointActions; ++j)
        //    debug << "|"<<j<<":"<<temp[j]<< "|";
        if(n ==0)
        {
            for (unsigned j = 0; j < mNumJointActions; ++j)
            {
                sum += temp[j]*mJointQTable[i][j].first;
                // debug << "Q("<<i<<","<<j<<")"<<mJointQTable[i][j].first<<":"<<temp[j]<<endl;
            }
        }
        else
        {
            for (unsigned j = 0; j < mNumJointActions; ++j)
            {
                sum += temp[j]*mJointQTable[i][j].second;
                //  debug << "Q("<<i<<","<<j<<")"<<mJointQTable[i][j].second<<":"<<temp[j]<<endl;
            }
        }
        //debug << "SUM:"<< sum<< endl;
        if(sum < temp_sum)
        {
            temp_sum = sum;
            pi = temp;
        }
        else if(sum == temp_sum)
            pi = temp;
    }
    for (unsigned j = 0; j < mNumJointActions; ++j)
    {
        mJointStrat[i][j] = pi[j];
        //debug << "->|"<<j<<":"<<pi[j]<< "|";
    }
}

void CE_VI::rCE(unsigned i)
{
    double sum = 0.0;
    double temp_sum = -1000;
    vector <double> pi;
    vector <double> temp;
    for (unsigned n = 0;  n < 2; n++)
    {
        sum = 0.0;
        temp = CE(i,n);
        //debug << "AGENT:" << n << endl;
        //for (unsigned j = 0; j < mNumJointActions; ++j)
        //debug << "|"<<j<<":"<<temp[j]<< "|";
        if(n ==0)
        {
            for (unsigned j = 0; j < mNumJointActions; ++j)
            {
                sum += temp[j]*mJointQTable[i][j].first;
                //debug << "Q("<<i<<","<<j<<")"<<mJointQTable[i][j].first<<":"<<temp[j]<<endl;
            }
        }
        else
        {
            for (unsigned j = 0; j < mNumJointActions; ++j)
            {
                sum += temp[j]*mJointQTable[i][j].second;
                //debug << "Q("<<i<<","<<j<<")"<<mJointQTable[i][j].second<<":"<<temp[j]<<endl;
            }
        }
        //debug << "SUM:"<< sum<< endl;
        if(sum > temp_sum)
        {
            temp_sum = sum;
            pi = temp;
        }
        else if(sum == temp_sum)
            pi = temp;
    }
    for (unsigned j = 0; j < mNumJointActions; ++j)
        mJointStrat[i][j] = pi[j];
}

void CE_VI::Friend(unsigned state)
{
    vector <unsigned> jointMoves;
    map <unsigned, double> bestMoves1;
    map <unsigned, double> bestMoves2;
    double bestActionVal1 = -100;
    double bestActionVal2 = -100;
    double tempVal = -100;
    unsigned jointMove = 0;
    unsigned action = 0;
    map<unsigned, double>::iterator it1;
    map<unsigned, double>::iterator it2;

    //if(state==91)
    //	cout << "STATE " << state << endl;
    //jointMoves.clear();
    //jointMoves = mpEnvironment->AvailJointMoves(mpEnvironment->GetFromStateIndex(state,4,3));
    for(unsigned i= 0; i< mNumJointActions;i++)
    {
        jointMove = i;
        for(unsigned j =0; j<2;j++)
        {
            //if(state==38)
            //cout <<"jointAction: " << jointMove<< endl;
            if(j==0)
            {
                tempVal = mJointQTable[state][jointMove].first;
                // if(state==38)
                // 	cout <<"1: REWARD: "<<tempVal<<endl;
                if(tempVal > bestActionVal1)
                {
                    bestMoves1.clear();
                    assert(bestMoves1.empty());
                    action = (unsigned)mpEnvironment->GetFromIndex(jointMove,2,4)[0];
                    bestActionVal1 = tempVal;
                    bestMoves1[action] = 0.0;
                    // if(state==38)
                    // 	cout << "1: bestAction: "<<action<<": "<<bestActionVal1<< endl;
                }
                else
                {
                    if(tempVal == bestActionVal1)
                    {
                        action = (unsigned)mpEnvironment->GetFromIndex(jointMove,2,4)[0];
                        //if(state==38)
                        //	cout << "1: bestAction: "<<action<<": "<<bestActionVal1<< endl;
                        it1 = bestMoves1.find(action);
                        if(it1== bestMoves1.end())
                            bestMoves1[action]= bestActionVal1;
                    }
                }
            }//agent 2
            else
            {
                tempVal = mJointQTable[state][jointMove].second;
                //if(state==38)
                //	cout <<"2: REWARD: "<<tempVal<<endl;
                //tempVal = mJointQTable[state][jointMove].second;
                if(tempVal > bestActionVal2)
                {
                    bestMoves2.clear();
                    assert(bestMoves2.empty());
                    action = mpEnvironment->GetFromIndex(jointMove,2,4)[1];
                    bestActionVal2 = tempVal;
                    bestMoves2[action] = 0.0;
                    // if(state==38)
                    // 	cout << "2: bestAction: "<<action<<": "<<bestActionVal2<< endl;
                }
                else
                {
                    if(tempVal == bestActionVal2)
                    {
                        action = mpEnvironment->GetFromIndex(jointMove,2,4)[1];
                        //  if(state==38)
                        //  	cout << "2: bestAction: "<<action<<": "<<bestActionVal2<< endl;
                        it2 = bestMoves2.find(action);
                        if(it2== bestMoves2.end())
                            bestMoves2[action] = bestActionVal2;
                    }
                }
            }
        }//for each agent
    }//for each joint action

    /* if(state==38)
{
     	cout << "1: Num. best moves: "<< bestMoves1.size()<< endl;
     	cout << "2: Num. best moves: "<< bestMoves2.size()<< endl;
}*/
    //make the probability distribution
    for(it1 = bestMoves1.begin(); it1!= bestMoves1.end();it1++)
        bestMoves1[it1->first] = 1/(float)bestMoves1.size();
    for(it2 = bestMoves2.begin(); it2!= bestMoves2.end();it2++)
        bestMoves2[it2->first] = 1/(float)bestMoves2.size();


    for(unsigned i = 0; i<mNumJointActions; i++)
    {
        mJointStrat[state][i]= 0.0;
    }//end for i

    //make the joint strategy prob distribution
    for(it1 = bestMoves1.begin(); it1!= bestMoves1.end();it1++)
    {
        for(it2 = bestMoves2.begin(); it2!= bestMoves2.end();it2++)
        {
            jointMoves.clear();
            assert(jointMoves.empty());
            jointMoves.push_back(it1->first);
            jointMoves.push_back(it2->first);
            mJointStrat[state][mpEnvironment->ComputeJointIndex(jointMoves,4)]= it1->second*it2->second;
            /*if(state==52)
        {
            	cout << "JointMove "<< mpEnvironment->ComputeJointIndex(jointMoves,4);
            	cout << " ("<< it1->first<<","<<it2->first<<"): "<<it1->second*it2->second << endl;
        }*/
        }
    }
}

void CE_VI::uCE(unsigned state)
{
    glp_prob *lp;
    double Qsum = 0;
    unsigned elem = 1;
    bool isNonZero = false;
    vector <unsigned> jointMoves;
    unsigned jointMove;
    vector <unsigned> lieMoves;
    vector <unsigned> jointMovesLie;
    vector <double> strat;
    vector <unsigned> ones;//for the prob dist constraint
    unsigned jointMoveLie;
    unsigned index = 1;
    //unsigned const numConstrains = mNumActions*(mNumActions-1)*mNumAgents;
    string str;
    vector<int> ind;
    vector<double> val;

    //debug << endl<<endl<<"**********************************STATE:" << state<< endl;
    lp = glp_create_prob();
    glp_set_prob_name(lp, "CE");
    glp_set_obj_dir(lp, GLP_MAX);

    //here we set the limits of the independent variables (all joint probabilities),
    //as well as their coeficients in the objective function
    glp_add_cols(lp, mNumJointActions);
    for(unsigned i=0; i<mNumJointActions; i++)
    {
        str.append("pi_");
        str.append(1,i+97);
        glp_set_col_name(lp, i+1, str.data());
        str.clear();
        glp_set_col_bnds(lp, i+1, GLP_DB, 0.0, 1.0);//the [0,1] bounds on each prob
        //this is what realy changes with each flavor of CE
        glp_set_obj_coef(lp, i+1, mJointQTable[state][i].first+mJointQTable[state][i].second);
    }

    //here we set the coeficients of all constraint ia[] controlls the rows (contraint index)
    //ja[]controls the cols (each joint prob), ar[] is the value for that coeficient

    for(unsigned n=0; n<mNumAgents;n++)
    {
        //debug << "*agent:" << n<< endl;
        for(unsigned i=0; i<mNumActions;i++)//my action
        {
            //debug<<"**action:" << i<< endl;
            for(unsigned k=0;k < mNumActions;k++)
            {
                if(k != i)
                    lieMoves.push_back(k);
            }

            while(!lieMoves.empty())//my lie
            {
                //debug<< "***lie:" << lieMoves.back()<<endl;

                for(unsigned j=0; j<mNumActions;j++)//opponent's move
                {
                    jointMoves.clear();
                    jointMovesLie.clear();

                    if(n == 0)
                    {
                        jointMoves.push_back(i);
                        jointMoves.push_back(j);
                        jointMovesLie.push_back(lieMoves.back());
                        jointMovesLie.push_back(j);

                        jointMove = mpEnvironment->ComputeJointIndex(jointMoves,4);
                        jointMoveLie = mpEnvironment->ComputeJointIndex(jointMovesLie,4);
                        Qsum = mJointQTable[state][jointMove].first - mJointQTable[state][jointMoveLie].first; /* a[1,1] =  1 */
                        if(Qsum != 0)
                        {
                            isNonZero = true;
                            ind.push_back((int)jointMove+1), val.push_back(Qsum);
                            elem++;
                            //debug << "Q"<<n<<"("<<state<<","<<jointMove<<"):"<<mJointQTable[state][jointMove].first;
                            //debug << " - " << "Q"<<n<<"("<<state<<","<<jointMoveLie<<"):"<<mJointQTable[state][jointMoveLie].first<<endl;
                            //debug << "NONZERO:" << index<< endl;
                        }
                    }
                    else if(n == 1)
                    {
                        jointMoves.push_back(j);
                        jointMoves.push_back(i);
                        jointMovesLie.push_back(j);
                        jointMovesLie.push_back(lieMoves.back());

                        jointMove = mpEnvironment->ComputeJointIndex(jointMoves,4);
                        jointMoveLie = mpEnvironment->ComputeJointIndex(jointMovesLie,4);
                        Qsum =  mJointQTable[state][jointMove].second - mJointQTable[state][jointMoveLie].second; /* a[1,1] =  1 */
                        if(Qsum != 0)
                        {
                            isNonZero = true;
                            ind.push_back((int)jointMove+1), val.push_back(Qsum);
                            elem++;
                            //debug << "Q"<<n<<"("<<state<<","<<jointMove<<"):"<<mJointQTable[state][jointMove].second;
                            //debug<< " - " << "Q"<<n<<"("<<state<<","<<jointMoveLie<<"):"<<mJointQTable[state][jointMoveLie].second<<endl;
                            //debug << "NONZERO:" << index<< endl;
                        }
                    }
                    //debug << "Q"<<n<<"("<<state<<","<<jointMove<<"):"<<mJointQTable[state][jointMove].first;
                    //debug<< " - " << "Q"<<n<<"("<<state<<","<<jointMoveLie<<"):"<<mJointQTable[state][jointMoveLie].first<<endl;
                }//for all opponent actions
                if(isNonZero)
                {
                    glp_add_rows(lp, 1);
                    str = "r"+index;
                    glp_set_row_name(lp, index, str.data());
                    glp_set_row_bnds(lp, index, GLP_LO, 0.0, 0.0);
                    int ind1[ind.size()+1];
                    //debug <<"->row " << index <<": ";
                    double val1[ind.size()+1];
                    int var = 1;
                    int size = ind.size();
                    while (!ind.empty())
                    {
                        ind1[var] = ind.back();
                        //debug << ind1[var]<<",";
                        ind.pop_back();
                        val1[var] = val.back();
                        //debug << val1[var]<<";";
                        val.pop_back();
                        var++;
                    }
                    glp_set_mat_row(lp,index, size,ind1,val1);
                    ind.clear(), val.clear();
                    index++;
                }
                lieMoves.pop_back();
                isNonZero = false;
                elem = 1;
            }//for each lie
        }//for each of my actions
    }

    //for the prob distr constraint
    int ind1[mNumJointActions+1];
    double val1[mNumJointActions+1];
    for(unsigned i=1; i< mNumJointActions+1;i++)
    {
        ind1[i] = i;
        val1[i] = 1;
    }
    int row = glp_add_rows(lp, 1);
    glp_set_row_name(lp, row, "probDist");
    glp_set_row_bnds(lp, row, GLP_FX, 1, 1);
    glp_set_mat_row(lp,row, mNumJointActions,ind1,val1);
    index++;

    lpx_simplex(lp);
    //Z = glp_get_obj_val(lp);
    //cout << "Z = " << Z << endl;

    //if(state==16)
    //{
    /*double  val2[mNumJointActions];
    int ind2[mNumJointActions];
    debug << "Rows:" << glp_get_num_rows(lp)<<endl;
    debug << "Cols: "<< glp_get_num_cols(lp)<<endl;
    for(unsigned j=1; j< glp_get_num_rows(lp)+1;j++)
{
        glp_get_mat_row(lp,j, ind2,val2);
        debug << endl<<"Constraint[" <<j<<"]:";
        for(unsigned i=1; i<5;i++)
        {
            debug << ind2[i]<<":"<<val2[i]<<",";
        }
}*/
    //}

    for(unsigned i=0; i<mNumJointActions;i++)
    {
        mJointStrat[state][i] = lpx_get_col_prim(lp, i+1);
        //if(state == 41)
        //debug << i<< "("<<mJointQTable[state][i].first<<","<<mJointQTable[state][i].second<<"):"<<lpx_get_col_prim(lp, i+1)<<" ";
    }


    lpx_delete_prob(lp);
}


vector <double> CE_VI::CE(unsigned state, unsigned agent)
{
    glp_prob *lp;
    double Qsum = 0;
    unsigned elem = 1;
    bool isNonZero = false;
    vector <unsigned> jointMoves;
    unsigned jointMove;
    vector <unsigned> lieMoves;
    vector <unsigned> jointMovesLie;
    vector <double> strat;
    vector <unsigned> ones;//for the prob dist constraint
    unsigned jointMoveLie;
    unsigned index = 1;
    //unsigned const numConstrains = mNumActions*(mNumActions-1)*mNumAgents;
    string str;
    vector<int> ind;
    vector<double> val;

    //debug << endl<<endl<<"**********************************STATE:" << state<< endl;
    lp = glp_create_prob();
    glp_set_prob_name(lp, "CE");
    glp_set_obj_dir(lp, GLP_MAX);

    //here we set the limits of the independent variables (all joint probabilities),
    //as well as their coeficients in the objective function
    glp_add_cols(lp, mNumJointActions);
    for(unsigned i=0; i<mNumJointActions; i++)
    {
        str.append("pi_");
        str.append(1,i+97);
        glp_set_col_name(lp, i+1, str.data());
        str.clear();
        glp_set_col_bnds(lp, i+1, GLP_DB, 0.0, 1.0);//the [0,1] bounds on each prob
        //this is what realy changes with each flavor of CE
        if (agent == 0)
            glp_set_obj_coef(lp, i+1, mJointQTable[state][i].first);
        else if (agent == 1)
            glp_set_obj_coef(lp, i+1, mJointQTable[state][i].second);
    }

    //here we set the coeficients of all constraint ia[] controlls the rows (contraint index)
    //ja[]controls the cols (each joint prob), ar[] is the value for that coeficient

    for(unsigned n=0; n<mNumAgents;n++)
    {
        //debug << "*agent:" << n<< endl;
        for(unsigned i=0; i<mNumActions;i++)//my action
        {
            //debug<<"**action:" << i<< endl;
            for(unsigned k=0;k < mNumActions;k++)
            {
                if(k != i)
                    lieMoves.push_back(k);
            }

            while(!lieMoves.empty())//my lie
            {
                //debug<< "***lie:" << lieMoves.back()<<endl;

                for(unsigned j=0; j<mNumActions;j++)//opponent's move
                {
                    jointMoves.clear();
                    jointMovesLie.clear();

                    if(n == 0)
                    {
                        jointMoves.push_back(i);
                        jointMoves.push_back(j);
                        jointMovesLie.push_back(lieMoves.back());
                        jointMovesLie.push_back(j);

                        jointMove = mpEnvironment->ComputeJointIndex(jointMoves,4);
                        jointMoveLie = mpEnvironment->ComputeJointIndex(jointMovesLie,4);
                        Qsum = mJointQTable[state][jointMove].first - mJointQTable[state][jointMoveLie].first; /* a[1,1] =  1 */
                        if(Qsum != 0)
                        {
                            isNonZero = true;
                            //ia[elem] = index, ja[elem] = jointMove+1, ar[elem] =  Qsum;
                            ind.push_back((int)jointMove+1), val.push_back(Qsum);
                            elem++;
                            //debug << "Q"<<n<<"("<<state<<","<<jointMove<<"):"<<mJointQTable[state][jointMove].first;
                            //debug << " - " << "Q"<<n<<"("<<state<<","<<jointMoveLie<<"):"<<mJointQTable[state][jointMoveLie].first<<endl;
                            //debug << "NONZERO:" << index<< endl;
                        }
                    }
                    else if(n == 1)
                    {
                        jointMoves.push_back(j);
                        jointMoves.push_back(i);
                        jointMovesLie.push_back(j);
                        jointMovesLie.push_back(lieMoves.back());

                        jointMove = mpEnvironment->ComputeJointIndex(jointMoves,4);
                        jointMoveLie = mpEnvironment->ComputeJointIndex(jointMovesLie,4);
                        Qsum =  mJointQTable[state][jointMove].second - mJointQTable[state][jointMoveLie].second; /* a[1,1] =  1 */
                        if(Qsum != 0)
                        {
                            isNonZero = true;
                            //ia[elem] = index, ja[elem] = jointMove+1, ar[elem] = Qsum;
                            ind.push_back((int)jointMove+1), val.push_back(Qsum);
                            elem++;
                            //debug << "Q"<<n<<"("<<state<<","<<jointMove<<"):"<<mJointQTable[state][jointMove].second;
                            //debug<< " - " << "Q"<<n<<"("<<state<<","<<jointMoveLie<<"):"<<mJointQTable[state][jointMoveLie].second<<endl;
                            //debug << "NONZERO:" << index<< endl;
                        }
                    }
                    //debug << "Q"<<n<<"("<<state<<","<<jointMove<<"):"<<mJointQTable[state][jointMove].first;
                    //debug<< " - " << "Q"<<n<<"("<<state<<","<<jointMoveLie<<"):"<<mJointQTable[state][jointMoveLie].first<<endl;
                }//for all opponent actions
                if(isNonZero)
                {
                    glp_add_rows(lp, 1);
                    str = "r"+index;
                    glp_set_row_name(lp, index, str.data());
                    glp_set_row_bnds(lp, index, GLP_LO, 0.0, 0.0);
                    int ind1[ind.size()+1];
                    //debug <<"->row " << index <<": ";
                    double val1[ind.size()+1];
                    int var = 1;
                    int size = ind.size();
                    while (!ind.empty())
                    {
                        ind1[var] = ind.back();
                        //debug << ind1[var]<<",";
                        ind.pop_back();
                        val1[var] = val.back();
                        //debug << val1[var]<<";";
                        val.pop_back();
                        var++;
                    }
                    glp_set_mat_row(lp,index, size,ind1,val1);
                    ind.clear(), val.clear();
                    index++;
                }
                lieMoves.pop_back();
                isNonZero = false;
                elem = 1;
            }//for each lie
        }//for each of my actions
    }

    //for the prob distr constraint
    int ind1[mNumJointActions+1];
    double val1[mNumJointActions+1];
    for(unsigned i=1; i< mNumJointActions+1;i++)
    {
        ind1[i] = i;
        val1[i] = 1;
    }
    int row = glp_add_rows(lp, 1);
    glp_set_row_name(lp, row, "probDist");
    glp_set_row_bnds(lp, row, GLP_FX, 1, 1);
    glp_set_mat_row(lp,row, mNumJointActions,ind1,val1);
    index++;

    lpx_simplex(lp);
    //Z = glp_get_obj_val(lp);
    //cout << "Z = " << Z << endl;

    //if(state==16)
    //{
    /*double  val2[mNumJointActions];
    int ind2[mNumJointActions];
    debug << "Rows:" << glp_get_num_rows(lp)<<endl;
    debug << "Cols: "<< glp_get_num_cols(lp)<<endl;
    for(unsigned j=1; j< glp_get_num_rows(lp)+1;j++)
{
        glp_get_mat_row(lp,j, ind2,val2);
        debug << endl<<"Constraint[" <<j<<"]:";
        for(unsigned i=1; i<5;i++)
        {
            debug << ind2[i]<<":"<<val2[i]<<",";
        }
}*/
    //}

    for(unsigned i=0; i<mNumJointActions;i++)
    {
        strat.push_back(lpx_get_col_prim(lp, i+1));
        //if(state == 41)
        //debug << i<< "("<<mJointQTable[state][i].first<<","<<mJointQTable[state][i].second<<"):"<<lpx_get_col_prim(lp, i+1)<<" ";
    }


    lpx_delete_prob(lp);
    return strat;
}

double CE_VI::Min(vector <double> moves)
{
    double temp = 10000;
    while(!moves.empty())
    {
        //temp = moves.back()<? temp;
        temp = min(moves.back(),temp);
        moves.pop_back();
    }
    return temp;
}

vector <double> CE_VI::MiniMax(unsigned state, unsigned agent, int direction, double* sum)
{
    glp_prob *lp;
    bool isNonZero = false;
    vector < vector<double> > tableQ;
    vector <unsigned> temp;
    unsigned jointMove;
    vector <double> rowQ;
    vector <unsigned> jointMoves1;
    vector <double> strat;
    vector <unsigned> ones;//for the prob dist constraint
    unsigned index = 1;
    //unsigned const numConstrains = mNumActions*(mNumActions-1)*mNumAgents;
    string str;
    vector<int> ind;
    vector<double> val;

    //debug << endl<<endl<<"**********************************STATE:" << state<< endl;
    lp = glp_create_prob();
    glp_set_prob_name(lp, "minimax");
    if(direction < 0)
        glp_set_obj_dir(lp, GLP_MAX);
    if(direction > 0)
        glp_set_obj_dir(lp, GLP_MAX);
    //if(state==78)
    //    debug << "State:"<<state<< endl;
    for(unsigned i=0; i<mNumActions; i++)
    {
        for(unsigned j=0; j<mNumActions; j++)
        {
            if(agent==0)
            {
                temp.push_back(i), temp.push_back(j);
                if(direction>0)
                    rowQ.push_back(direction*mJointQTable[state][mpEnvironment->ComputeJointIndex(temp,4)].first);
                else
                    rowQ.push_back(direction*mJointQTable[state][mpEnvironment->ComputeJointIndex(temp,4)].second);
            }
            else
            {
                temp.push_back(j), temp.push_back(i);
                if(direction>0)
                    rowQ.push_back(direction*mJointQTable[state][mpEnvironment->ComputeJointIndex(temp,4)].second);
                else
                    rowQ.push_back(direction*mJointQTable[state][mpEnvironment->ComputeJointIndex(temp,4)].first);

            }
            temp.clear();
        }
        //if (state==35)
        //    debug << rowQ[0]<< ","<< rowQ[1]<< ","<< rowQ[2]<< ","<< rowQ[3]<< endl;
        tableQ.push_back(rowQ);
        rowQ.clear();
    }

    //here we set the limits of the independent variables (all joint probabilities),
    //as well as their coeficients in the objective function
    glp_add_cols(lp, mNumActions);
    for(unsigned i=0; i<mNumActions; i++)
    {
        str.append("pi_");
        str.append(1,i+97);
        glp_set_col_name(lp, i+1, str.data());
        str.clear();
        glp_set_col_bnds(lp, i+1, GLP_DB, 0.0, 1.0);//the [0,1] bounds on each prob

        glp_set_obj_coef(lp, i+1, Min(tableQ[i]));
        /*if(state==35)
    {
            debug << tableQ[i][0]<<","<<tableQ[i][1]<<","<<tableQ[i][2]<<","<<tableQ[i][3]<<endl;
            debug << "coef: "<< Min(tableQ[i])<<endl;
    }*/
    }
    //the V value for minimax
    glp_add_cols(lp, 1);
    str.append("pi_");
    str.append(1,5+97);
    glp_set_col_name(lp, 5, str.data());
    str.clear();
    glp_set_col_bnds(lp, 5, GLP_FR, 0.0, 1.0);
    glp_set_obj_coef(lp, 5, 0);

    //here we set the coeficients of all constraint ia[] controlls the rows (contraint index)
    //ja[]controls the cols (each joint prob), ar[] is the value for that coeficient

    //debug << "*agent:" << n<< endl;
    for(unsigned i=0; i<mNumActions;i++)//my action
    {

        for(unsigned j=0; j<mNumActions;j++)//opponent's move
        {
            //jointMoves.clear();
            jointMoves1.clear();

            if(agent == 0)
            {

                jointMoves1.push_back(i);
                jointMoves1.push_back(j);

                jointMove = mpEnvironment->ComputeJointIndex(jointMoves1,4);
                if(direction>0)
                {
                    if(mJointQTable[state][jointMove].first != 0)
                    {
                        isNonZero = true;
                        ind.push_back(j+1), val.push_back(direction*mJointQTable[state][jointMove].first);
                    }
                }
                else
                {
                    if(mJointQTable[state][jointMove].second != 0)
                    {
                        isNonZero = true;
                        ind.push_back(j+1), val.push_back(direction*mJointQTable[state][jointMove].second);
                    }

                }
            }
            else if(agent == 1)
            {
                jointMoves1.push_back(j);
                jointMoves1.push_back(i);

                jointMove = mpEnvironment->ComputeJointIndex(jointMoves1,4);
                if(direction>0)
                {
                    if(mJointQTable[state][jointMove].second != 0)
                    {
                        isNonZero = true;
                        ind.push_back(j+1), val.push_back(direction*mJointQTable[state][jointMove].second);
                    }
                }
                else
                {
                	if(mJointQTable[state][jointMove].first != 0)
                    {
                        isNonZero = true;
                        ind.push_back(j+1), val.push_back(direction*mJointQTable[state][jointMove].first);
                    }
                
                }
            }
            //debug << "Q"<<n<<"("<<state<<","<<jointMove<<"):"<<mJointQTable[state][jointMove].first;
            //debug<< " - " << "Q"<<n<<"("<<state<<","<<jointMoveLie<<"):"<<mJointQTable[state][jointMoveLie].first<<endl;
        }//for all opponent actions
        if(isNonZero)
        {
            ind.push_back(5);
            val.push_back(-1);
            glp_add_rows(lp, 1);
            str = "r"+index;
            glp_set_row_name(lp, index, str.data());
            if(direction < 0)
                glp_set_row_bnds(lp, index, GLP_UP, 0.0, 0.0);
            else
                glp_set_row_bnds(lp, index, GLP_LO, 0.0, 0.0);
            int ind1[ind.size()+1];
            //debug <<"->row " << index <<": ";
            double val1[ind.size()+1];
            int var = 1;
            int size = ind.size();
            while (!ind.empty())
            {
                ind1[var] = ind.back();
                //debug << ind1[var]<<",";
                ind.pop_back();
                val1[var] = val.back();
                //debug << val1[var]<<";";
                val.pop_back();
                var++;
            }
            glp_set_mat_row(lp,index, size,ind1,val1);
            ind.clear(), val.clear();
            index++;
        }
        isNonZero = false;
    }//for each of my actions

    //for the prob distr constraint
    int ind1[mNumActions+1];
    double val1[mNumActions+1];
    for(unsigned i=1; i< mNumActions+1;i++)
    {
        ind1[i] = i;
        val1[i] = 1;
    }
    int row = glp_add_rows(lp, 1);
    glp_set_row_name(lp, row, "probDist");
    glp_set_row_bnds(lp, row, GLP_FX, 1, 1);
    glp_set_mat_row(lp,row, mNumActions,ind1,val1);
    index++;

    lpx_simplex(lp);
    //*sum = glp_get_obj_val(lp);
    *sum = lpx_get_col_prim(lp, 5);
    //cout << "Z = " << Z << endl;
    /*if(state ==78)
{
    	
        debug << "COEF, VAL:"<< endl;
        for(unsigned i=0; i<mNumActions;i++)
        {
            debug << glp_get_obj_coef(lp,i+1)<<","<<lpx_get_col_prim(lp, i+1)<<"|";
        }
}*/

    if(index>1)
    {
        for(unsigned i = 0; i<mNumActions; i++)
            strat.push_back(lpx_get_col_prim(lp, i+1));
    }
    else
    {
        for(unsigned i = 0; i<mNumActions; i++)
        {
            strat.push_back(1/(float)mNumActions);
        }//end for i
    }

    /*if(state==91)
{
        debug << "STRAT("<<agent<<"):";
        for(unsigned i = 0; i<mNumActions; i++)
        {
            debug << strat[i]<<",";
        }//end for i
        debug << endl;
}*/


    lpx_delete_prob(lp);
    return strat;
}



void CE_VI::Defend(unsigned state, int agent)
{
    vector <double> temp;
    vector <double> strat1;
    vector <double> strat2;
    double min1 = 10000;
    double min2 = 10000;
    double sum;
    vector <unsigned> moves;
    unsigned joint;

    for(unsigned i=0; i<mNumAgents;i++)
    {
        //if(state==mInitState)
        //   debug << "AGENT:" << i<<endl;
        if((unsigned)agent == i)
        {
            if(i==0)
                strat1 = MiniMax(state,i,MAX,&sum);
            else
                strat2 = MiniMax(state,i,MAX,&sum);
        }

        else
        {
            if(i==0)
                strat1 = MiniMax(state,i,MIN,&sum);
            else
                strat2 = MiniMax(state,i,MIN,&sum);
        }

    }
    //strat1.clear(), strat2.clear(), temp.clear();
    /*if(state==mInitState)
    {
        debug <<"AGENT:" << agent<<endl;
        debug <<"Strat: ("<< strat1[0]<<","<<strat2[0]<<"),("<<strat1[1]<<","<< strat2[1];
        debug << "),("<< strat1[2]<<","<<strat2[2]<<"),("<< strat1[3]<<","<<strat2[3]<<")"<<endl;
    }*/

    min1 = 10000;
    min2 = 10000;
    for(unsigned i=0; i<mNumActions;i++)
    {
        for(unsigned j=0; j<mNumActions;j++)//for each opponent action
        {
            if(agent==0)
            {
                moves.push_back(i), moves.push_back(j);
                joint = mpEnvironment->ComputeJointIndex(moves,4);
                mJointStrat[state][joint] = strat1[i]*strat2[j];
                //if(state==35)
                //    debug<< "P["<<i<<","<<j <<"]:"<<strat1[i]<<","<<strat2[j]<<endl;
                moves.clear();
            }
            else
            {
                moves.push_back(j), moves.push_back(i);
                joint = mpEnvironment->ComputeJointIndex(moves,4);
                mJointStrat[state][joint] = strat1[j]*strat2[i];
                //if(state==35)
                //    debug<< "P["<<i<<","<<j <<"]:"<<strat1[i]<<","<<strat2[j]<<endl;
                moves.clear();
            }
        }
    }

    /*if(state==mInitState)
    {
        for(unsigned x =0; x<mNumJointActions; x++)
            debug << "Q("<<state<<","<<x<<")"<<mJointQTable[state][x].first<<","<<mJointQTable[state][x].second<<":"<<mJointStrat[state][x]<<endl;
    }*/
}

void CE_VI::DebugStateIndex()
{
    cout << "**DEBUGING INDEX-VECTOR TRANSFORMATION**"<<endl;
    for (unsigned i = 0; i < mNumStates; i++)// for all states
    {
        cout << "State " << i<<": ";
        vector <unsigned> state = mpEnvironment->GetFromStateIndex(i,4,2);
        cout << state[0] << ","<< state[1] << ","<< state[2] << ","<< state[3];
        cout << " -> "<< mpEnvironment->ComputeJointIndex(state,3)<<endl;
    }
}

string CE_VI::PrintQTable()
{
    ostringstream str;
    str << setprecision(6);
    str << "mJointQTable (size):" << mJointQTable.size() << endl;

    map<unsigned, map<unsigned, pair <float,float> > >::const_iterator joint_it;
    //map<float, map<Variable*, unsigned int> >::const_iterator joint_exp_it = mJointExperience.begin();
    for (joint_it = mJointQTable.begin(); joint_it != mJointQTable.end(); joint_it++)
    {
        str << "Q[" << joint_it->first << "]:";
        map<unsigned, pair<float, float> >::const_iterator action_it;
        //map<Variable*, unsigned int>::const_iterator exp_it = joint_exp_it->second.begin();
        for (action_it = joint_it->second.begin(); action_it != joint_it->second.end(); action_it++)
        {
            str << setw(1) << setprecision(4) << action_it->first << "(" << action_it->second.first << ","<<action_it->second.second << ")|";
        }
        str << endl;
    }
    return str.str();
}

string CE_VI::PrintVTable()
{
    vector <unsigned> state;
    ostringstream str;
    str << setprecision(6);
    str << "mJointVTable (size):" << mJointVTable.size() << endl;

    map<unsigned, pair <float,float> >::const_iterator joint_it;
    //map<float, map<Variable*, unsigned int> >::const_iterator joint_exp_it = mJointExperience.begin();
    for (joint_it = mJointVTable.begin(); joint_it != mJointVTable.end(); joint_it++)
    {
        str << "V[" << joint_it->first;
        state.clear();
        state = mpEnvironment->GetFromStateIndex(joint_it->first,4,3);
        str << ":" << state[0]<<","<< state[1]<<","<< state[2]<<","<< state[3]<<"]";
        str << setw(1) << setprecision(4) << "(" << joint_it->second.first << ","<<joint_it->second.second << ")|";
        str << endl;
    }
    return str.str();
}

string CE_VI::PrintJointStrat()
{
    ostringstream str;
    str << setprecision(6);
    str << "mJointStrat (size):" << mJointStrat.size() << endl;

    map <unsigned, map <unsigned, float> >::const_iterator joint_it;
    //map<float, map<Variable*, unsigned int> >::const_iterator joint_exp_it = mJointExperience.begin();
    for (joint_it = mJointStrat.begin(); joint_it != mJointStrat.end(); joint_it++)
    {
        str << "p[" << joint_it->first << "]:";
        map<unsigned, float>::const_iterator action_it;
        //map<Variable*, unsigned int>::const_iterator exp_it = joint_exp_it->second.begin();
        for (action_it = joint_it->second.begin(); action_it != joint_it->second.end(); action_it++)
        {
            str << setw(1) << setprecision(4) << action_it->first << "(" <<action_it->second<< ")|";
        }
        str << endl;
    }
    return str.str();
}

string CE_VI::DebugDynamics(vector <unsigned> state, vector <unsigned> actions)
{
    ostringstream str;
    str << "state: " << state[0] << ","<< state[1] << ","<< state[2] << ","<< state[3] <<endl;
    str << "actions: " <<actions[0]<< ","<<actions[1]<<  endl;
    return str.str();
}

string CE_VI::WriteStrategy(unsigned state)
{
    ostringstream str;
    unsigned action = 0;
    unsigned next = 0;
    unsigned moves = 0;

    str << state<<":";

    do
    {

        action = 0;
        while(mJointStrat[state][action]< 0.001)
            action++;
        str << action << "->";

        next = 0;
        while(mpTransitionModel->GetProb(state,action,next)==0 && next < mNumStates)
            next++;
        state = next;
        str << state<< ":";
        moves++;
    }
    while(next < mNumStates && moves < 30);

    str << endl;
    return str.str();
}

string CE_VI::PrintInfo()
{
    ostringstream str;
    str << "************NEW EXPERIMENT*************" << endl;
    str << mpEnvironment->GetInfo(0);
    str << "algorithm:"<<mAlgorithm << endl;
    str << "gamma:" << mDiscountFactor << endl;
    return str.str();
}
