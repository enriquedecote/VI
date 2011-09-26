// SYSTEM INCLUDES
#include <iostream>
#include <sstream>
#include <assert.h>

// LOCAL INCLUDES
#include "GridWorldWall.h"

/*extern "C" GridWorldWall* GridWorldWallRef()
{
    return new GridWorldWall();
}*/


GridWorldWall::GridWorldWall(char game)
{
    pair <unsigned, unsigned> coord;// <col,row>
    mNumAgents = 2;
    mNumJointActions = 16;
    mGame = game;
    nullPay = -1;
    switch(game)
    {
    case 'C':
        wallProb = 0.5;
        mNumRows = 3;
        mNumCols = 3;
        coord.first = 1;//col
        coord.second = 0;//row
        SetGoal(0,coord,1);//this is only for letting both agents step into the same cell
        SetGoal(1,coord,100);
        SetGoal(2,coord,100);
        mEndCell1[coord] = 1.0;
        mEndCell2[coord] = 1.0;
        SetHWall(2,1,2,2,1);
        SetHWall(0,2,0,1,1);
        break;
    case 'G':
        wallProb = 0;
        mNumRows = 3;
        mNumCols = 3;
        coord.first = 2;//col
        coord.second = 0;//row
        SetGoal(1,coord,100);
        SetGoal(0,coord,1);
        mEndCell1[coord] = 1.0;
        coord.first = 0;
        coord.second = 0;
        SetGoal(2,coord,100);
        SetGoal(0,coord,1);
        mEndCell2[coord] = 1.0;
        break;
    case 'P':
        wallProb = 0;
        mNumRows = 2;
        mNumCols = 9;
        coord.first = 0;//col
        coord.second = 1;//row
        SetGoal(1,coord,100);
        mEndCell1[coord] = 1.0;
        coord.first = 8;//col
        coord.second = 1;//row
        SetGoal(2,coord,100);
        mEndCell2[coord] = 1.0;
        coord.first = 4;//col
        coord.second = 0;//row
        SetGoal(0,coord,100);
        SetGoal(1,coord,100);
        SetGoal(2,coord,100);
        mEndCell1[coord] = 1.0;
        mEndCell2[coord] = 1.0;
        //walls
        SetHWall(3,0,3,1,1);
        SetHWall(5,0,5,1,1);
        SetVWall(3,0,4,0,1);
        SetVWall(4,0,5,0,1);
        break;
    case 'X':
        wallProb = 0;
        mNumRows = 2;
        mNumCols = 5;
        coord.first = 3;//col
        coord.second = 0;//row
        SetGoal(1,coord,100);
        //SetGoal(0,coord,1);
        mEndCell1[coord] = 1.0;
        coord.first = 1;//col
        coord.second = 0;//row
        SetGoal(2,coord,100);
        //SetGoal(0,coord,1);
        mEndCell2[coord] = 1.0;
        //walls
        SetVWall(0,0,1,0,1);
        SetVWall(1,0,2,0,1);
        SetVWall(2,0,3,0,1);
        SetVWall(3,0,4,0,1);
        break;
    case 'A':
        wallProb = 0;
        mNumRows = 1;
        mNumCols = 10;
        coord.first = 0;//col
        coord.second = 0;//row
        SetGoal(1,coord,100);
        SetGoal(0,coord,1);
        mEndCell1[coord] = 1.0;
        coord.first = 2;
        coord.second = 0;
        SetGoal(2,coord,100);
        SetGoal(0,coord,1);
        mEndCell2[coord] = 1.0;
        coord.first = 8;//col
        coord.second = 0;//row
        SetGoal(1,coord,100);
        SetGoal(0,coord,1);
        mEndCell1[coord] = 1.0;
        nullPay = -10;
        break;
    }
}

GridWorldWall::~GridWorldWall()
{}

void  GridWorldWall::SetHWall(unsigned col1, unsigned row1, unsigned col2, unsigned row2, unsigned lenght)
{
    for(unsigned int j = 0; j < lenght; j++)
    {
        pair<int,int> p1;
        p1.first  = col1+j;
        p1.second = row1;
        pair<int,int> p2;
        p2.first  = col2+j;
        p2.second = row2;
        mWall[p1][p2] = 2;
        mWall[p2][p1] = 2;
        cout << "HorWall between: " << "("<< col1<<","<<row1<<") - ("<< col2<<","<<row2<<")"<< endl;
    }
}

void  GridWorldWall::SetVWall(unsigned col1, unsigned row1, unsigned col2, unsigned row2, unsigned lenght)
{
    for(unsigned int j = 0; j < lenght; j++)
    {
        pair<int,int> p1;
        p1.first  = col1;
        p1.second = row1+j;
        pair<int,int> p2;
        p2.first  = col2;
        p2.second = row2+j;
        mWall[p1][p2] = 2;
        mWall[p2][p1] = 2;
        cout << "VertWall between: " << "("<< col1<<","<<row1<<") - ("<< col2<<","<<row2<<")"<< endl;
    }
}

void  GridWorldWall::GenerateTransitionModel ( )
{
    //unsigned int stateSize = mNumRows*mNumCols;
    vector <unsigned> state1(4,0);
    vector <unsigned> state2;
    pair<unsigned,unsigned> coord1;
    pair<unsigned,unsigned> coord2;
    vector <unsigned> actions;
    float prob = 1;
    float prob1 = 0.0;
    VariablesInfo* pState = new VariablesInfo();
    pState->AddVariable(new Variable("Col_0", 0.0, (float)mNumCols-1));
    pState->AddVariable(new Variable("Col_1", 0.0, (float)mNumCols-1));
    pState->AddVariable(new Variable("Row_0", 0.0, (float)mNumRows-1));
    pState->AddVariable(new Variable("Row_1", 0.0, (float)mNumRows-1));
    // vector <unsigned> actions;
    unsigned s1;
    vector <unsigned> s2;
    vector <unsigned> states;
    vector <unsigned> stateVar;
    //cout <<"mNumRows:"<<mNumRows<<endl;
    //cout <<"mNumCols:"<<mNumCols<<endl;
    //cycle through the joint action space: |S|=81
    for (unsigned w = 0; w<mNumRows; w++)//Row_1
    {
        for (unsigned x = 0; x<mNumRows; x++)//Row_0
        {
            for (unsigned y = 0; y<mNumCols; y++)//Col_1
            {
                for (unsigned z = 0; z<mNumCols; z++)//Col_0
                {
                    if(!(z==y && x==w))
                    {
                        pState->SetVarValue("Col_0",(float)z);
                        pState->SetVarValue("Col_1",(float)y);
                        pState->SetVarValue("Row_0",(float)x);
                        pState->SetVarValue("Row_1",(float)w);
                        s1 = ComputeJointIndex(pState);

                        state1.clear();
                        assert(state1.empty());
                        state1.push_back(z);
                        state1.push_back(y);
                        state1.push_back(x);
                        state1.push_back(w);

                        actions.clear();
                        assert(actions.empty());
                        //actions= AvailJointMoves(state1);
                        //if (s1==52)
                        //cout << "STATE:"<< s1 << endl;
                        for (unsigned a = 0; a<mNumJointActions; a++) //cycle through all avalilable joint actions
                        {
                            s2 = ComputeNextState(state1, a, &prob1);
                            //if(s1==97)
                            //  cout << "action:"<<a<<"("<<prob1<<") size:"<<s2.size()<<"->"<<s2[0]<<","<<s2[1]<<endl;
                            if(wallProb==0)
                                s2 = PurgeStatesWalls(state1,s2, &prob1);
                            for (unsigned var = 0; var < s2.size(); var++)
                            {
                                //  if(s1==97)
                                //     cout << "s2"<<"("<<prob1<<"):"<<s2[0]<<","<<s2[1]<<endl;
                                //watching for the walls (probs from s1 to s2) may change
                                states.clear();
                                states.push_back(s2.at(var));
                                prob = prob1;
                                state2 = GetFromStateIndex(s2.at(var),4,3);
                                coord1.first = state1[0];
                                coord1.second = state1[2];
                                coord2.first = state2[0];
                                coord2.second = state2[2];
                                unsigned walls = 0;
                                if(wallProb==0)
                                {
                                    if(mWall[coord1][coord2]==2)//there's a wall in between s1-s2 for agent 1
                                    {
                                        states.clear();
                                        walls++;
                                        //if(!(state1[0]==state2[1] && state1[2]==state2[3])||mGoal1[coord2]<1)
                                        if(s2.size()>1)
                                        {
                                            if(!(state1[0]==state2[1] && state1[2]==state2[3]))
                                            {
                                                //prob *= wallProb;
                                                //the agent may not move due to the wall
                                                pState->SetVarValue("Col_0",state1[0]);
                                                pState->SetVarValue("Col_1",state2[1]);
                                                pState->SetVarValue("Row_0",state1[2]);
                                                pState->SetVarValue("Row_1",state2[3]);
                                                states.push_back(ComputeJointIndex(pState));
                                            }
                                            else
                                                states.push_back(s1);
                                        }
                                    }

                                    coord1.first = state1[1];
                                    coord1.second = state1[3];
                                    coord2.first = state2[1];
                                    coord2.second = state2[3];
                                    if(mWall[coord1][coord2]==2)//there's a wall in between s1-s2 for agent 2
                                    {
                                        if(walls == 0)
                                            states.clear();
                                        walls++;
                                        //if(!(state2[0]==state1[1] && state2[2]==state1[3])||mGoal2[coord2]<1)
                                        if(!(state2[0]==state1[1] && state2[2]==state1[3]))
                                        {
                                            //prob *= wallProb;
                                            //the agent may not move due to the wall
                                            pState->SetVarValue("Col_0",state2[0]);
                                            pState->SetVarValue("Col_1",state1[1]);
                                            pState->SetVarValue("Row_0",state2[2]);
                                            pState->SetVarValue("Row_1",state1[3]);
                                            states.push_back(ComputeJointIndex(pState));
                                        }
                                        else
                                            states.push_back(s1);
                                    }
                                    if(walls ==2)
                                    {
                                        states.clear();
                                        states.push_back(s1);
                                    }
                                    for (unsigned i= 0; i < states.size(); i++)
                                    {
                                        transitionModel.SetProbability(s1,a,states.at(i),prob);
                                        //cout << "STATE:" << s1<< endl;
                                        //if(s1==43)
                                        //cout << "Action " << a << "("<< prob << ")-> " << states.at(i)<<	endl;
                                    }
                                }
                                else
                                {

                                    if(mWall[coord1][coord2]==2)//there's a wall in between s1-s2 for agent 1
                                    {
                                        walls++;
                                        if(!(state1[0]==state2[1] && state1[2]==state2[3])||mGoal1[coord2]<1)
                                        {
                                            prob *= wallProb;
                                            //the agent may not move due to the wall
                                            pState->SetVarValue("Col_0",state1[0]);
                                            pState->SetVarValue("Col_1",state2[1]);
                                            pState->SetVarValue("Row_0",state1[2]);
                                            pState->SetVarValue("Row_1",state2[3]);
                                            states.push_back(ComputeJointIndex(pState));
                                        }
                                    }

                                    coord1.first = state1[1];
                                    coord1.second = state1[3];
                                    coord2.first = state2[1];
                                    coord2.second = state2[3];
                                    if(mWall[coord1][coord2]==2)//there's a wall in between s1-s2 for agent 2
                                    {
                                        walls++;
                                        if(!(state2[0]==state1[1] && state2[2]==state1[3])||mGoal2[coord2]<1)
                                        {
                                            prob *= wallProb;
                                            //the agent may not move due to the wall
                                            pState->SetVarValue("Col_0",state2[0]);
                                            pState->SetVarValue("Col_1",state1[1]);
                                            pState->SetVarValue("Row_0",state2[2]);
                                            pState->SetVarValue("Row_1",state1[3]);
                                            states.push_back(ComputeJointIndex(pState));
                                        }
                                    }
                                    if(walls ==2)
                                        states.push_back(s1);
                                    for (unsigned i= 0; i < states.size(); i++)
                                    {
                                        transitionModel.SetProbability(s1,a,states.at(i),prob);
                                        //cout << "STATE:" << s1<< endl;
                                        //if(s1==229)
                                        //cout << "Action " << a << "("<< prob << ")-> " << states.at(i)<<	endl;
                                    }
                                }//else: semi walls
                            }
                            s2.clear();
                        }
                    }
                    state1.clear();
                    state2.clear();
                }
            }
        }
    }
    delete pState;
}

vector <unsigned> GridWorldWall::PurgeStatesWalls(vector <unsigned>state1, vector <unsigned>st2, float *prob)
{
    vector <unsigned> state2;
    //  vector <unsigned> nowalls;
    vector <unsigned> states;
    VariablesInfo* pState = new VariablesInfo();
    pState->AddVariable(new Variable("Col_0", 0.0, (float)mNumCols-1));
    pState->AddVariable(new Variable("Col_1", 0.0, (float)mNumCols-1));
    pState->AddVariable(new Variable("Row_0", 0.0, (float)mNumRows-1));
    pState->AddVariable(new Variable("Row_1", 0.0, (float)mNumRows-1));

    pair<unsigned,unsigned> coord1_1;
    pair<unsigned,unsigned> coord1_2;
    pair<unsigned,unsigned> coord2_1;
    pair<unsigned,unsigned> coord2_2;

    vector <unsigned> s2 = st2;
    //if(state1[0]==3&&state1[1]==2&&state1[2]==1&&state1[3]==0)
    //cout << s2[0]<<","<<s2[1]<<" size. "<< s2.size()<<endl;
    for(unsigned i = 0; i < s2.size();i++)
    {
        state2 = GetFromStateIndex(s2[i],4,3);
        coord1_1.first = state1[0];
        coord1_1.second = state1[2];
        coord1_2.first = state2[0];
        coord1_2.second = state2[2];
        coord2_1.first = state1[1];
        coord2_1.second = state1[3];
        coord2_2.first = state2[1];
        coord2_2.second = state2[3];
        //if(state1[0]==3&&state1[1]==2&&state1[2]==1&&state1[3]==0)
        //	cout << "data:"<<s2[i]<<endl;
        if(mWall[coord1_1][coord1_2]!=2 && mWall[coord2_1][coord2_2]!=2)//no walls
            states.push_back(s2[i]);
        else
        {
            if(mWall[coord1_1][coord1_2]==2 && mWall[coord2_1][coord2_2]==2)
            {
                *prob = 1;
            }
            else
            {
                if(mWall[coord1_1][coord1_2]==2 && s2.size()<2)//agent 1's wall
                {
                    pState->SetVarValue("Col_0",state1[0]);
                    pState->SetVarValue("Col_1",state2[1]);
                    pState->SetVarValue("Row_0",state1[2]);
                    pState->SetVarValue("Row_1",state2[3]);
                    states.push_back(ComputeJointIndex(pState));

                }
                else if(mWall[coord2_1][coord2_2]==2 && s2.size()<2)//agent 2's wall
                {
                    pState->SetVarValue("Col_0",state2[0]);
                    pState->SetVarValue("Col_1",state1[1]);
                    pState->SetVarValue("Row_0",state2[2]);
                    pState->SetVarValue("Row_1",state1[3]);
                    states.push_back(ComputeJointIndex(pState));
                }
            }
        }
    }
    //if(state1[0]==2&&state1[1]==0&&state1[2]==0&&state1[3]==0)
    //   cout<< "states.size()"<<states.size()<< "  st2.size()"<<st2.size()<<endl;
    if(states.size()!=st2.size() && states.size()==1)
        *prob = 1;
    if (states.size()==2)
        if(states[0]==states[1])
            *prob = 1;

    return states;
}

float GridWorldWall::Reward(unsigned agent, unsigned s, unsigned actions)
{
    vector <unsigned> state = GetFromStateIndex(s, 4, 3);
    map <pair<unsigned,unsigned>,float>::iterator it;

    //check if agent is not on the goal
    pair<unsigned, unsigned> c1;
    pair<unsigned, unsigned> c2;

    c1.first = state[0], c1.second=state[2];
    c2.first = state[1], c2.second=state[3];
    //if(mGoal2[c]!=0 ||mGoal0[c]!=0)
    if(mEndCell1[c1]==1.0||mEndCell2[c2]==1.0)
        return 0.0;

    if(mGame != 'T')
    {
        if (!(state[0] == state[1] && state[2] == state[3]))//if agents are not on the same cell
        {
            unsigned nextState;
            vector <bool> stay = ComputeNextState(state, actions, &nextState);
            vector <unsigned> nextStateVec = GetFromStateIndex(nextState, 4, 3);
            if(agent == 0)
            {
                pair <unsigned, unsigned> coord;
                coord.first = nextStateVec[0];//col
                coord.second = nextStateVec[2];//row
                if(!(nextStateVec[0] == state[1] && nextStateVec[2]==state[3]))//check if opponent was not on the goal
                {
                    if(mWall[c1][coord]!=2)
                    {
                        if(stay[agent])//agent didn't move, don't punish
                            return 0.0;
                        it = mGoal1.find(coord);
                        if(it != mGoal1.end()&& it->second !=0)
                            return mGoal1[coord];
                        else
                            return nullPay;
                    }
                    else
                        return nullPay;
                }
                else if((mGoal2[coord]<1) && !(nextStateVec[1]==state[0]&&nextStateVec[3]==state[2]) && mGame == 'A')
                {
                    if(mWall[c1][coord]!=2)
                    {
                        if(stay[agent])//agent didn't move, don't punish
                            return 0.0;
                        it = mGoal1.find(coord);
                        if(it != mGoal1.end()&& it->second !=0)
                            return mGoal1[coord];
                        else
                            return nullPay;
                    }
                }
                else
                    return nullPay;
            }

            else if (agent == 1)
            {
                pair <unsigned, unsigned> coord;
                coord.first = nextStateVec[1];//col
                coord.second = nextStateVec[3];//row
                pair <unsigned, unsigned> coord1;
                coord1.first = state[0];//opp col
                coord1.second = state[2];//opp row
                if(!(nextStateVec[1] == state[0] && nextStateVec[3]==state[2]))//check if opponent was not on the goal
                {
                    if(mWall[c2][coord]!=2)
                    {
                        if(stay[agent])//agent didn't move, don't punish
                            return 0.0;
                        it = mGoal2.find(coord);
                        if(it != mGoal2.end()&& it->second !=0)
                            return mGoal2[coord];
                        else
                            return nullPay;
                    }
                    else
                        return nullPay;
                }
                else if((mGoal1[coord]<1) && !(nextStateVec[0]==state[1]&&nextStateVec[2]==state[3]) && mGame == 'A')
                {
                    if(mWall[c2][coord]!=2)
                    {
                        if(stay[agent])//agent didn't move, don't punish
                            return 0.0;
                        it = mGoal2.find(coord);
                        if(it != mGoal2.end()&& it->second !=0)
                            return mGoal2[coord];
                        else
                            return nullPay;
                    }
                    else return nullPay;
                }
                else
                    return nullPay;
            }
            else
                return nullPay;
        }
        else
            return nullPay;
    }
    else
    {
        if (!(state[0] == state[1] && state[2] == state[3]))
        {
            vector <unsigned> nextStateVec = ComputeNextState(state, GetFromIndex(actions, 2, 4));
            if(nextStateVec[0] == nextStateVec[1] && nextStateVec[2] == nextStateVec[3])
            {
                pair <unsigned, unsigned> coord;
                coord.first = nextStateVec[0];//col
                coord.second = nextStateVec[2];//row
                return mGoal0[coord];
            }

            else
                return nullPay;
        }
        else
            return nullPay;
    }
}

