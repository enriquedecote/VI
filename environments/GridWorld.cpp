#include <iostream>
#include <sstream>
#include <string>
#include <math.h>
#include <assert.h>

// LOCAL INCLUDES
#include "GridWorld.h"

GridWorld::GridWorld ()
{}

GridWorld::GridWorld (char game)
{
    pair <unsigned, unsigned> coord;// <col,row>
    mNumAgents = 2;
    mGame = game;
    mNumJointActions = 16;
    switch(game)
    {
    case 'G':
        mNumRows = 3;
        mNumCols = 3;
        coord.first = 2;//col
        coord.second = 0;//row
        SetGoal(1,coord,100);
        mEndCell1[coord] = 1.0;
        coord.first = 0;
        coord.second = 0;
        SetGoal(2,coord,100);
        mEndCell2[coord] = 1.0;
        break;
    case 'T':
        mNumRows = 3;
        mNumCols = 3;
        coord.first = 0;
        coord.second = 0;
        SetGoal(0,coord,100);
        mEndCell1[coord] = 1.0;
        mEndCell2[coord] = 1.0;
        cout << "Environment: Team game MG" << endl;
        cout << "Num. of goals: 1" << endl;
        cout << "Grid size: " << mNumRows << " x " << mNumCols << endl;
        break;
    }
}

GridWorld::~GridWorld ( )
{}

void  GridWorld::Init (VariablesInfo* pActions)
{
    mpStateInfo = pActions->Clone();
    mNumStates = (unsigned)pow((float)(mNumRows*mNumCols),(float)mNumAgents);
    //cout << "NumStates: " << mNumStates<< endl;
    for (unsigned i = 0; i < mNumAgents; i++)
    {
        /*ostringstream row;
        ostringstream col;
        row << ROW;
        col << COL;
        row << "_" << i;
        col << "_" << i;

        const Variable*const row_var = mpStateInfo->GetVariable(row.str());
        const Variable*const col_var = mpStateInfo->GetVariable(col.str());
        mFirstRow.push_back((unsigned int) row_var->GetMinValue());
        mLastRow.push_back((unsigned int) row_var->GetMaxValue());
        mFirstCol.push_back((unsigned int) col_var->GetMinValue());
        mLastCol.push_back((unsigned int) col_var->GetMaxValue());
        mAgentRow.push_back((unsigned int) row_var->GetVarValue());
        mAgentCol.push_back((unsigned int) col_var->GetVarValue());
        cout << "  agent " << i+1 << endl;*/
        mFirstRow.push_back(0);
        mLastRow.push_back(mNumRows-1);
        mFirstCol.push_back(0);
        mLastCol.push_back(mNumCols-1);
        /*cout << "    first row = " << mFirstRow[i] << endl;
        cout << "    last row = " << mLastRow[i] << endl;
        cout << "    first col = " << mFirstCol[i] << endl;
        cout << "    last col = " << mLastCol[i] << endl;*/

    }

    //transitionModel = new SASTransitionModel();
    GenerateTransitionModel();
    cout << "Transition Model created!" << endl;

}

void GridWorld::AppendInit(VariablesInfo* pState)
{
    //cout << "GridWorld::AppendInit" << endl;
    //cout << "size: "<< pState->size() << endl;
    /*string token = "token_";
    pair <unsigned,unsigned> coord;
    for (unsigned i = 0; i < mNumAgents; i++)
       {
       	std::ostringstream counter;
       	counter << i;

           const Variable*const row_var = pState->GetVariable(token.append("Row").append(counter.str()));
           token = "token_";
           const Variable*const col_var = pState->GetVariable(token.append("Col").append(counter.str()));
           token = "token_";
           coord.first = (unsigned int) row_var->GetVarValue();
           coord.second = (unsigned int) col_var->GetVarValue();
           if (i == 0)
           	mTokens0.push_back(coord);
           else if(i == 1)
           	mTokens1.push_back(coord);*/
    /*cout << "  agent " << i+1 << endl;
    cout << "    first row = " << mFirstRow[i] << endl;
    cout << "    last row = " << mLastRow[i] << endl;
    cout << "    first col = " << mFirstCol[i] << endl;
    cout << "    last col = " << mLastCol[i] << endl;*/

    // }
}

void  GridWorld::Reset ( )
{
    for (unsigned i = 0; i < mNumAgents; i++)
    {
        bool ret;
        string *row = new string(ROW);
        string *col = new string(COL);
        row->append("_");
        col->append("_");
        ostringstream buffer;
        buffer << i;
        row->append(buffer.str());
        col->append(buffer.str());
        //cout << "  agente " << i+1 << " --> riga = " << mAgentRow[i] << ", colonna = " << mAgentCol[i] << endl;
        //ret = mpStateInfo->SetVarValue(*row,mFirstRow[i]);
        //delete row;
        //assert(ret);
        //ret = mpStateInfo->SetVarValue(*col,mFirstCol[i]);
        //delete col;
        //assert(ret);
        ret = mpStateInfo->SetVarValue(*row,mAgentRow[i]);
        delete row;
        assert(ret);
        ret = mpStateInfo->SetVarValue(*col,mAgentCol[i]);
        delete col;
        assert(ret);
    }
}

void  GridWorld::ResetRandom ( )
{
    unsigned int start_row = rand() % mNumRows;
    unsigned int start_col = rand() % mNumCols;
    //cout << "Starting from row " << start_row << ", col " << start_col << endl;
    mpStateInfo -> SetVarValue(ROW,start_row);
    mpStateInfo -> SetVarValue(COL,start_col);
}

void  GridWorld::GenerateTransitionModel ( )
{
    //unsigned int stateSize = mNumRows*mNumCols;
    vector <unsigned> state(4,0);
    vector <unsigned> actions;
    float prob = 0.0;
    VariablesInfo* pState = new VariablesInfo();
    pState->AddVariable(new Variable("Col_0", 0.0, (float)mNumCols-1));
    pState->AddVariable(new Variable("Col_1", 0.0, (float)mNumCols-1));
    pState->AddVariable(new Variable("Row_0", 0.0, (float)mNumRows-1));
    pState->AddVariable(new Variable("Row_1", 0.0, (float)mNumRows-1));
    // vector <unsigned> actions;
    unsigned s1;
    vector <unsigned> s2;
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

                        state.clear();
                        assert(state.empty());
                        state.push_back(z);
                        state.push_back(y);
                        state.push_back(x);
                        state.push_back(w);

                        actions.clear();
                        assert(actions.empty());
                        //actions= AvailJointMoves(state);
                        //if (s1==52)
                        //	cout << "STATE 52"<<endl;
                        for (unsigned a = 0; a<mNumJointActions; a++) //cycle through all avalilable joint actions
                        {
                            s2 = ComputeNextState(state, a, &prob);
                            for (unsigned var = 0; var < s2.size(); var++)
                            {
                                transitionModel.SetProbability(s1,a,s2.at(var),prob);
                                cout << "STATE:" << s1<<endl;
                                cout << "action "<<a<<"("<<prob<<")"<<" -> "<<s2.at(var)<<endl;
                                /*if(s1==24)
                            {
                                	int x;
                                	cin >> x;
                            }*/
                            }
                            // 	cout << "action "<<actions[a]<<" -> "<<s2<<endl;
                        }
                    }
                    state.clear();
                }
            }
        }
    }
    delete pState;

}

unsigned int  GridWorld::Size  () const
{
    return mNumStates;
}

unsigned int  GridWorld::ComputeJointIndex  (vector <unsigned> vec, unsigned int size) const
{
    unsigned int base = 1;
    unsigned int index = 0;

    for (unsigned int i = 0; i < vec.size(); i++)
    {
        // the value of the variable is used to define its "positional"
        // value wrt the admissible values

        index += vec[i]*base;
        base *= size;
    }

    return index;
}

unsigned int  GridWorld::ComputeJointIndex  (VariablesInfo* pVars) const
{
    unsigned int base = 1;
    unsigned int index = 0;

    for (unsigned int i = 0; i < pVars->size(); i++)
    {
        // the value of the variable is used to define its "positional"
        // value wrt the admissible values
        index += (unsigned int) pVars->GetVariable(i)->GetVarValue()*base;
        base *= (unsigned int) pVars->GetVariable(i)->GetMaxValue()+1;
    }

    return index;
}

vector <unsigned>  GridWorld::AvailJointMoves( vector <unsigned int> state)
{
    vector <unsigned int> cur_row;
    cur_row.push_back(state[2]);
    cur_row.push_back(state[3]);
    vector <unsigned int> cur_col;
    cur_col.push_back(state[0]);
    cur_col.push_back(state[1]);
    vector <unsigned int> state_option;
    vector <unsigned int> next_col;
    vector <unsigned int> jointMove(2,0);
    vector <unsigned int> jointMoves;
    unsigned temp;

    //cout<< "**AVAIL JOINT MOVES**"<< endl;
    //if(state[0]==0&&state[1]==1&&state[2]==1&&state[3]==0)
    //cout << "current state: "<< state[0] << "," <<state[1] << "," <<state[2] << "," << state[3]<< endl;
    for (unsigned i=0; i < 4; i++)//agent 1
    {
        for (unsigned j=0; j < 4; j++)//agent 0
        {
            jointMove.push_back(j);
            jointMove.push_back(i);
            state_option = ComputeNextState(state,jointMove);
            //if(state[0]==0&&state[1]==1&&state[2]==1&&state[3]==0)
            //cout << "state option: " << state_option[0]<< "," << state_option[1]<< "," << state_option[2]<< "," << state_option[3]<< endl;
            if ( AvailMove(state_option))
            {
                //jointMove.push_back(i);
                temp = ComputeJointIndex(jointMove, 4);
                //if(state[0]==0&&state[1]==1&&state[2]==1&&state[3]==0)
                //cout << "Avail joint action:" << temp << ": " << j << "," << i <<endl;
                jointMoves.push_back(temp);
            }
            jointMove.clear();
        }
    }
    return jointMoves;
}

vector <unsigned>  GridWorld::AvailJointMoves( unsigned int s) const
{
    vector <unsigned int> state = GetFromStateIndex(s,4,3);
    vector <unsigned int> cur_row;
    cur_row.push_back(state[2]);
    cur_row.push_back(state[3]);
    vector <unsigned int> cur_col;
    cur_col.push_back(state[0]);
    cur_col.push_back(state[1]);
    vector <unsigned int> state_option;
    vector <unsigned int> next_col;
    vector <unsigned int> jointMove(2,0);
    vector <unsigned int> jointMoves;
    unsigned temp;

    //cout<< "**AVAIL JOINT MOVES**"<< endl;
    //cout << "current state: "<< state[0] << "," <<state[1] << "," <<state[2] << "," << state[3]<< endl;
    for (unsigned i=0; i < 4; i++)//agent 1
    {
        for (unsigned j=0; j < 4; j++)//agent 0
        {
            jointMove.push_back(j);
            jointMove.push_back(i);
            state_option = ComputeNextState(state,jointMove);
            //  cout << "state option: " << state_option[0]<< "," << state_option[1]<< "," << state_option[2]<< "," << state_option[3]<< endl;
            if ( AvailMove(state_option))
            {
                jointMove.push_back(i);
                temp = ComputeJointIndex(jointMove, 4);
                //cout << "Avail joint action:" << temp << ": " << j << "," << i <<endl;
                jointMoves.push_back(temp);
            }
            jointMove.clear();
        }
    }
    return jointMoves;
}

const TransitionModel*const  GridWorld::GetTransitionModel ()
{
    //cout << "Environment TansitionModel: " << &transitionModel << endl;
    return &transitionModel;
}

vector<unsigned> GridWorld::ComputeNextState( vector <unsigned> state, unsigned jointAction, float* prob)
{
    VariablesInfo* pState = new VariablesInfo();
    pState->AddVariable(new Variable("Col_0", 0.0, (float)mNumCols-1));
    pState->AddVariable(new Variable("Col_1", 0.0, (float)mNumCols-1));
    pState->AddVariable(new Variable("Row_0", 0.0, (float)mNumRows-1));
    pState->AddVariable(new Variable("Row_1", 0.0, (float)mNumRows-1));
    vector <unsigned> col;
    vector <unsigned> row;
    pair <unsigned,unsigned> coord;
    col.push_back(state[0]);
    col.push_back(state[1]);
    row.push_back(state[2]);
    row.push_back(state[3]);
    vector <unsigned> next_col(col);
    vector <unsigned> next_row(row);


    vector <unsigned int> next_state;

    //if(state[0]==1&&state[1]==2&&state[2]==2&&state[3]==1)
    //	cout <<"**COMPUTE NEXT STATE**" << endl;

    //get the actions of each agent from the available joint actions
    vector <unsigned int> actions = GetFromIndex(jointAction, 2, 4);

    coord.first = col[0], coord.second = row[0];
    if(mEndCell1[coord] == 1.0)
    {
        next_state.push_back(78);
        *prob = 0.0;
        return next_state;
    }
    coord.first = col[1], coord.second = row[1];
    if(mEndCell2[coord] == 1.0)
    {
        next_state.push_back(78);
        *prob = 0.0;
        return next_state;
    }

    //GET A RANDOM PLAYER
    unsigned agent = rand() %2;
    //cout << "First agent:" << agent<< endl;
    unsigned i = 0;
    do
    {
        if(AvailMove(agent,actions[agent],row,col) || mGame == 'T')
        {
        	//if(state[0]==2&&state[1]==4&&state[2]==1&&state[3]==1)
        	//	cout << "agent:"<< agent<< " action:"<<actions[agent]<<endl;
            switch (actions[agent])
            {
            case D :
                next_row[agent]++;
                break;

            case R:
                next_col[agent]++;
                break;

            case U:
                next_row[agent]--;
                break;

            case L:
                next_col[agent]--;
                break;
            }
        }
        else
        {
            switch(mGame)
            {
            case 'G':
                if(agent ==0)
                {
                    next_col[agent] = 0;
                    next_row[agent] = 2;
                }
                else
                {
                    next_col[agent] = 2;
                    next_row[agent] = 2;
                }
                break;
            }
        }
        i++;
        if(agent == 0)
            agent++;
        else
            agent--;
    }
    while (i < 2);
    //there's a joint move that would make them both end up in the same cell

    //check if it's not a common goal
    bool isGoal = false;
    for (int var = 0; var < 1; ++var)
    {
        coord.first = next_col[var], coord.second = next_row[var];
        if(mGoal0[coord]==1)
            isGoal = true;
    }

    if (next_row[0] == next_row[1] && next_col[0] == next_col[1] && mGame != 'T'&&!isGoal)
    {
        bool agentMoved = false;
        agent = 0;
        while(agent < 2)
        {
            next_row.clear(), next_col.clear();
            next_row = row;
            next_col = col;
            /*if (next_col[0]==3&&next_col[1]==2&&next_row[0]==1&&next_row[1]==0&&jointAction==3)
        {
            	cout<< "agent:" << agent<< endl;
            	cout << "action:"<<actions[agent]<<endl;
        }*/
            if(AvailMove(agent,actions[agent],row,col)&& NoOppInCell(agent,actions[agent],next_row,next_col))
            {
                /*if (next_col[0]==3&&next_col[1]==2&&next_row[0]==1&&next_row[1]==0&&jointAction==3)
                {
                    cout<< "--agent:" << agent<< endl;
                    cout << "--action:"<<actions[agent]<<endl;
                }*/
                agentMoved = true;
                switch (actions[agent])
                {
                case D :
                    next_row[agent]++;
                    break;

                case R:
                    next_col[agent]++;
                    break;

                case U:
                    next_row[agent]--;
                    break;

                case L:
                    next_col[agent]--;
                    break;
                }
            }
            agent++;
            pState->SetVarValue("Col_0",(float)next_col.at(0));
            pState->SetVarValue("Col_1",(float)next_col.at(1));
            pState->SetVarValue("Row_0",(float)next_row.at(0));
            pState->SetVarValue("Row_1",(float)next_row.at(1));
            next_state.push_back(ComputeJointIndex(pState));
        }
        if(agentMoved)
            *prob = 0.5;
    }//end if same ending cell
    else
    {
        pState->SetVarValue("Col_0",(float)next_col.at(0));
        pState->SetVarValue("Col_1",(float)next_col.at(1));
        pState->SetVarValue("Row_0",(float)next_row.at(0));
        pState->SetVarValue("Row_1",(float)next_row.at(1));
        next_state.push_back(ComputeJointIndex(pState));
        *prob = 1;
    }

    //this checks that the agents don't "traspass" each other
    if(next_col[0]==col[1] && next_row[0]==row[1] && next_col[1]==col[0] && next_row[1]==row[0])
    {
        next_state.clear();
        pState->SetVarValue("Col_0",(float)state.at(0));
        pState->SetVarValue("Col_1",(float)state.at(1));
        pState->SetVarValue("Row_0",(float)state.at(2));
        pState->SetVarValue("Row_1",(float)state.at(3));
        next_state.push_back(ComputeJointIndex(pState));
        *prob =1;
    }
    /*if(state[0]==2&&state[1]==0&&state[2]==0&&state[3]==0)
{
     cout << "Prev state:" << state[0]<<","<<state[1]<<","<<state[2]<<","<<state[3]<<endl;
     cout << "actions:" << jointAction<< ": "<<actions[0]<<","<<actions[1]<<"->"<< next_col[0]<<","<<next_col[1]<<","<<next_row[0]<<","<<next_row[1]<< ": " << endl;
}*/
    /*for (unsigned i = 0; i < next_state.size(); i++) {
    	cout << "Prev state:" << state[0]<<","<<state[1]<<","<<state[2]<<","<<state[3]<<endl;
        cout << "actions:" << jointAction<< "->"<< next_state[i] << endl;
}*/

    delete pState;
    return next_state;
}

/*vector<unsigned> GridWorld::ComputeNextState( vector <unsigned> state, unsigned jointAction, float* prob)
{
    VariablesInfo* pState = new VariablesInfo();
    pState->AddVariable(new Variable("Col_0", 0.0, (float)mNumCols-1));
    pState->AddVariable(new Variable("Col_1", 0.0, (float)mNumCols-1));
    pState->AddVariable(new Variable("Row_0", 0.0, (float)mNumRows-1));
    pState->AddVariable(new Variable("Row_1", 0.0, (float)mNumRows-1));
    vector <unsigned> col;
    vector <unsigned> row;
    pair <unsigned,unsigned> coord;
    col.push_back(state[0]);
    col.push_back(state[1]);
    row.push_back(state[2]);
    row.push_back(state[3]);
    vector <unsigned> next_col(col);
    vector <unsigned> next_row(row);
 
 
    vector <unsigned int> next_state;
 
    //if(state[0]==1&&state[1]==2&&state[2]==2&&state[3]==1)
    //	cout <<"**COMPUTE NEXT STATE**" << endl;
 
    //get the actions of each agent from the available joint actions
    vector <unsigned int> actions = GetFromIndex(jointAction, 2, 4);
 
    coord.first = col[0], coord.second = row[0];
    if(mEndCell1[coord] == 1.0)
    {
        next_state.push_back(78);
        *prob = 0.0;
        return next_state;
    }
    coord.first = col[1], coord.second = row[1];
    if(mEndCell2[coord] == 1.0)
    {
        next_state.push_back(78);
        *prob = 0.0;
        return next_state;
    }
 
    //GET A RANDOM PLAYER
    unsigned agent = rand() %2;
    //cout << "First agent:" << agent<< endl;
    unsigned i = 0;
    do
    {
        if(AvailMove(agent,actions[agent],row,col) || mGame == 'T')
        {
            switch (actions[agent])
            {
            case D :
                next_row[agent]++;
                break;
 
            case R:
                next_col[agent]++;
                break;
 
            case U:
                next_row[agent]--;
                break;
 
            case L:
                next_col[agent]--;
                break;
            }
        }
        else
        {
        	switch(mGame)
        	{
        		case 'G':
        			if(agent ==0)
        			{
        				next_col[agent] = 0;
        				next_row[agent] = 2;
        			}
        			else
        			{
        				next_col[agent] = 2;
        				next_row[agent] = 2;
        			}
        			break;
        	}
        }
        i++;
        if(agent == 0)
            agent++;
        else
            agent--;
    }
    while (i < 2);
 
 
    //there's a joint move that would make them both end up in the same cell
 
    //check if it's not a goal
    bool isGoal = false;
    for (int var = 0; var < 1; ++var)
    {
        coord.first = next_col[var], coord.second = next_row[var];
        if(mGoal0[coord]==1)
            isGoal = true;
    }
 
    if (next_row[0] == next_row[1] && next_col[0] == next_col[1] && mGame != 'T'&&!isGoal)
    {
        agent = 0;
        do
        {
            next_row = row;
            next_col = col;
            if(AvailMove(agent,actions[agent],row,col))
            {
                switch (actions[agent])
                {
                case D :
                    next_row[agent]++;
                    break;
 
                case R:
                    next_col[agent]++;
                    break;
 
                case U:
                    next_row[agent]--;
                    break;
 
                case L:
                    next_col[agent]--;
                    break;
                }
            }
            agent++;
            pState->SetVarValue("Col_0",(float)next_col.at(0));
            pState->SetVarValue("Col_1",(float)next_col.at(1));
            pState->SetVarValue("Row_0",(float)next_row.at(0));
            pState->SetVarValue("Row_1",(float)next_row.at(1));
            next_state.push_back(ComputeJointIndex(pState));
        }
        while (agent < 2);
        *prob = 0.5;
    }//end if same ending cell
    else
    {
        pState->SetVarValue("Col_0",(float)next_col.at(0));
        pState->SetVarValue("Col_1",(float)next_col.at(1));
        pState->SetVarValue("Row_0",(float)next_row.at(0));
        pState->SetVarValue("Row_1",(float)next_row.at(1));
        next_state.push_back(ComputeJointIndex(pState));
        *prob = 1;
    }
 
    //this checks that the agents don't "traspass" each other
    if(next_col[0]==col[1] && next_row[0]==row[1] && next_col[1]==col[0] && next_row[1]==row[0])
    {
        next_state.clear();
        pState->SetVarValue("Col_0",(float)state.at(0));
        pState->SetVarValue("Col_1",(float)state.at(1));
        pState->SetVarValue("Row_0",(float)state.at(2));
        pState->SetVarValue("Row_1",(float)state.at(3));
        next_state.push_back(ComputeJointIndex(pState));
        *prob =1;
    }
    delete pState;
    return next_state;
}*/

vector <bool> GridWorld::ComputeNextState( vector <unsigned> state, unsigned jointAction, unsigned *stateIndex)
{
    VariablesInfo* pState = new VariablesInfo();
    Variable* col0 = new Variable("Col_0", 0.0, (float)mNumCols-1);
    Variable* col1 = new Variable("Col_1", 0.0, (float)mNumCols-1);
    Variable* row0 = new Variable("Row_0", 0.0, (float)mNumRows-1);
    Variable* row1 = new Variable("Row_1", 0.0, (float)mNumRows-1);
    pState->AddVariable(col0);
    pState->AddVariable(col1);
    pState->AddVariable(row0);
    pState->AddVariable(row1);
    vector <unsigned int> row;
    row.push_back(state[2]);
    row.push_back(state[3]);
    vector <unsigned int> col;
    col.push_back(state[0]);
    col.push_back(state[1]);
    vector <unsigned int> next_state;
    vector <bool> stay(false,2);

    //if(state[0]==1&&state[1]==2&&state[2]==2&&state[3]==1)
    //	cout <<"**COMPUTE NEXT STATE**" << endl;

    //get the actions of each agent from the available joint actions
    vector <unsigned int> actions = GetFromIndex(jointAction, 2, 4);

    //GET A RANDOM PLAYER
    unsigned agent = rand() %2;
    //cout << "First agent:" << agent<< endl;
    unsigned i = 0;
    do
    {
        if(AvailMove(agent,actions[agent],row,col)||mGame == 'T')
        {
            switch (actions[agent])
            {
            case D :
                row[agent]++;
                break;

            case R:
                col[agent]++;
                break;

            case U:
                row[agent]--;
                break;

            case L:
                col[agent]--;
                break;
            }
            stay[agent] = false;
        }
        else
        	stay[agent] = true;
        i++;
        if(agent == 0)
            agent++;
        else
            agent--;
    }
    while (i < 2);
    pState->SetVarValue("Col_0",(float)col.at(0));
    pState->SetVarValue("Col_1",(float)col.at(1));
    pState->SetVarValue("Row_0",(float)row.at(0));
    pState->SetVarValue("Row_1",(float)row.at(1));
    unsigned next_index = ComputeJointIndex(pState);
    /*if(state[0]==1&&state[1]==3&&state[2]==1&&state[3]==1)
{
     cout << "Prev state:" << state[0]<<","<<state[1]<<","<<state[2]<<","<<state[3]<<endl;
     cout << "actions:" << jointAction<< ": "<<actions[0]<<","<<actions[1]<<"->"<< col[0]<<","<<col[1]<<","<<row[0]<<","<<row[1]
          << ": " << next_index << endl;
          cout << "truth:"<< stay[0]<<","<<stay[1]<<endl;
}*/
	delete col0;
	delete col1;
	delete row0;
	delete row1;
    delete pState;
    *stateIndex = next_index;
    return stay;
}

vector <unsigned> GridWorld::ComputeNextState( vector <unsigned int> state, vector <unsigned int> jointAction) const
{
    vector <unsigned int> row;
    row.push_back(state[2]);
    row.push_back(state[3]);
    vector <unsigned int> col;
    col.push_back(state[0]);
    col.push_back(state[1]);
    vector <unsigned int> next_state;

    //cout <<"**COMPUTE NEXT STATE**" << endl;

    //get the actions of each agent from the available joint actions
    for (unsigned i=0; i<mNumAgents; i++)
    {
        switch (jointAction[i])
        {
        case D :
            row[i]++;
            break;

        case R:
            col[i]++;
            break;

        case U:
            row[i]--;
            break;

        case L:
            col[i]--;
            break;
        }
    }
    next_state.push_back(col.at(0));
    next_state.push_back(col.at(1));
    next_state.push_back(row.at(0));
    next_state.push_back(row.at(1));
    // cout << "Prev state:" << state[0]<<","<<state[1]<<","<<state[2]<<","<<state[3]<<endl;
    // cout << "actions:" <<jointAction[0]<<","<<jointAction[1]<<"->"<< col[0]<<","<<col[1]<<","<<row[0]<<","<<row[1]
    //  << ": " <<  endl;
    return next_state;
}

vector <unsigned> GridWorld::GetFromStateIndex(unsigned int index, unsigned int size, unsigned base) const
{
    vector <unsigned> vec;
    unsigned int cur_joint_action = index;
    unsigned reminder;
    for (unsigned int i = 0; i < size; i++)
    {
        if (i == 0 || i == 1)//it's a column
        {
            reminder = cur_joint_action % mNumCols;
            vec.push_back( reminder);
            cur_joint_action = cur_joint_action/mNumCols;
        }
        else
        {
            reminder = cur_joint_action % mNumRows;
            vec.push_back( reminder);
            cur_joint_action = cur_joint_action/mNumRows;
        }
    }
    return vec;
}

vector <unsigned> GridWorld::GetFromIndex(unsigned int index, unsigned int size, unsigned base) const
{
    vector <unsigned> vec;
    unsigned int cur_joint_action = index;
    unsigned reminder;
    for (unsigned int i = 0; i < size; i++)
    {
        if (i == 0 || i == 1)//it's a column
        {
            reminder = cur_joint_action % base;
            vec.push_back( reminder);
            cur_joint_action = cur_joint_action/base;
        }
        else
        {
            reminder = cur_joint_action % base;
            vec.push_back( reminder);
            cur_joint_action = cur_joint_action/base;
        }
    }
    return vec;
}

vector <unsigned> GridWorld::GetFromIndex(unsigned int index, VariablesInfo* pVars) const
{
    vector <unsigned> vec;
    unsigned int cur_joint_action = index;
    unsigned reminder;
    for (unsigned int i = 0; i < pVars->size(); i++)
    {
        reminder = cur_joint_action % (unsigned int) (pVars->GetVariable(i)->GetMaxValue()+1);
        vec.push_back( reminder);
        cur_joint_action = cur_joint_action/(unsigned int) (pVars->GetVariable(i)->GetMaxValue()+1);
    }
    return vec;
}

bool  GridWorld::AvailMove(unsigned int i, unsigned action_value,
                           vector <unsigned int> cur_row,  vector <unsigned int> cur_col) const
{
    pair<int,int> p;

    switch (action_value)
    {
    case D :
        p.first  = cur_row[i]+1;
        p.second = cur_col[i];
        if (cur_row[i] < mLastRow[i] )
        {
            /*if (i==0)
        {
                if (!(cur_row[i]+1 == cur_row[i+1] && cur_col[i] == cur_col[i+1]))
                {
                    return true;
                }
                else
                    return false;
        }
            else
        {
                if (!(cur_row[i]+1 == cur_row[i-1] && cur_col[i] == cur_col[i-1]))
                {
                    return true;
                }
                else
                    return false;
        }*/
        	return true;

        }
        else
            return false;
        break;

    case R :
        if (cur_col[i] < mLastCol[i] )
        {
            /*if (i==0)
        {
                if (!(cur_col[i]+1 == cur_col[i+1] && cur_row[i] == cur_row[i+1]))
                {
                    return true;
                }
                else
                    return false;
        }
            else
        {
                if (!(cur_col[i]+1 == cur_col[i-1] && cur_row[i] == cur_row[i-1]))
                {
                    return true;
                }
                else
                    return false;
        }*/
            return true;
        }
        else
            return false;
        break;

    case U:
        if (cur_row[i] > mFirstRow[i] )
        {
            /* if (i==0)
             {
                 if (!(cur_row[i]-1 == cur_row[i+1] && cur_col[i] == cur_col[i+1]))
                 {
                     return true;
                 }
                 else
                     return false;
             }
             else
             {
                 if (!(cur_row[i]-1 == cur_col[i-1] && cur_col[i] == cur_col[i-1]))
                 {
                     return true;
                 }
                 else
                     return false;
             }*/
            return true;
        }
        else
            return false;
        break;

    case L :
        if (cur_col[i] > mFirstCol[i] )
        {
            /*if (i==0)
        {
                if (!(cur_col[i]-1 == cur_col[i+1] && cur_row[i] == cur_row[i+1]))
                {
                    return true;
                }
                else
                    return false;
        }
            else
        {
                if (!(cur_col[i]-1 == cur_col[i-1] && cur_row[i] == cur_row[i-1]))
                {
                    return true;
                }
                else
                    return false;
        }
            */
            return true;
        }
        else
            return false;
        break;
    }
}

bool  GridWorld::NoOppInCell(unsigned int i, unsigned action_value,
                             vector <unsigned int> cur_row,  vector <unsigned int> cur_col) const
{
    pair<int,int> p;
    unsigned r1,r2,c1,c2;
    r1 = cur_row[0], r2= cur_row[1], c1= cur_col[0], c2 = cur_col[1];

    switch (action_value)
    {
    case D :
        p.first  = cur_row[i]+1;
        p.second = cur_col[i];
        if (cur_row[i] < mLastRow[i] )
        {
            if (i==0)
            {
                if (!(cur_row[i]+1 == cur_row[i+1] && cur_col[i] == cur_col[i+1]))
                {
                    return true;
                }
                else
                    return false;
            }
            else
            {
                if (!(cur_row[i]+1 == cur_row[i-1] && cur_col[i] == cur_col[i-1]))
                {
                    
                    return true;
                }
                else
                    return false;
            }
            return true;
        }
        else
            return false;
        break;

    case R :
        if (cur_col[i] < mLastCol[i] )
        {
            if (i==0)
            {
                if (!(cur_col[i]+1 == cur_col[i+1] && cur_row[i] == cur_row[i+1]))
                {
                    return true;
                }
                else
                    return false;
            }
            else
            {
                if (!(cur_col[i]+1 == cur_col[i-1] && cur_row[i] == cur_row[i-1]))
                {
                    return true;
                }
                else
                    return false;
            }
            return true;
        }
        else
            return false;
        break;

    case U:
        if (cur_row[i] > mFirstRow[i] )
        {
            if (i==0)
            {
                if (!(cur_row[i]-1 == cur_row[i+1] && cur_col[i] == cur_col[i+1]))
                {
                    return true;
                }
                else
                    return false;
            }
            else
            {
                if (!(cur_row[i]-1 == cur_row[i-1] && cur_col[i] == cur_col[i-1]))
                {
                    return true;
                }
                else
                    return false;
            }
            return true;
        }
        else
            return false;
        break;

    case L :
        if (cur_col[i] > mFirstCol[i] )
        {
            if (i==0)
            {
                if (!(cur_col[i]-1 == cur_col[i+1] && cur_row[i] == cur_row[i+1]))
                {
                    return true;
                }
                else
                    return false;
            }
            else
            {
                if (!(cur_col[i]-1 == cur_col[i-1] && cur_row[i] == cur_row[i-1]))
                {
                    return true;
                }
                else
                    return false;
            }

            return true;
        }
        else
            return false;
        break;
    }
}

bool  GridWorld::AvailMove( vector <unsigned int> state) const
{
    vector <unsigned int> cur_row;
    cur_row.push_back(state[2]);
    cur_row.push_back(state[3]);
    vector <unsigned int> cur_col;
    cur_col.push_back(state[0]);
    cur_col.push_back(state[1]);

    pair<int,int> p;
    vector<bool> truth;

    for ( unsigned i=0; i<mNumAgents; i++)
    {

        if ( !((cur_row[i] > mLastRow[i] || cur_row[i] <  mFirstRow[i])
                || (cur_col[i]  > mLastCol[i] || cur_col[i]  < mFirstCol[i])))
        {
            p.first  = cur_row[i];
            p.second = cur_col[i];
            //if ( mHorWall.count(p) < 1.0 && mVerWall.count(p) < 1.0)
            truth.push_back(true);
        }
        else
            truth.push_back(false);
    }
    if(truth.at(0)==truth.at(1) && truth.at(0)== true)
        return true;
    else
        return false;
}

float GridWorld::Reward(unsigned agent, unsigned s, unsigned actions)
{
    vector <unsigned> state = GetFromStateIndex(s, 4, 3);

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
            ComputeNextState(state, actions, &nextState);
            vector <unsigned> nextStateVec = GetFromStateIndex(nextState, 4, 3);
            if(agent == 0)
            {
                pair <unsigned, unsigned> coord;
                coord.first = nextStateVec[0];//col
                coord.second = nextStateVec[2];//row
                if(!(nextStateVec[0] == state[1] && nextStateVec[2]==state[3]))//check if opponent was not on the goal
                {
                    return mGoal1[coord];
                }
                else
                {
                    if(mGoal2[coord] > 1)
                        return 0.0;
                    else
                        return mGoal1[coord];
                }
            }

            else if (agent == 1)
            {
                pair <unsigned, unsigned> coord;
                coord.first = nextStateVec[1];//col
                coord.second = nextStateVec[3];//row
                if(!(nextStateVec[1] == state[0] && nextStateVec[3]==state[2]))//check if opponent was not on the goal
                {
                    return mGoal2[coord];
                }
                else
                {
                    if(mGoal1[coord] > 1)
                        return 0.0;
                    else
                        return mGoal1[coord];
                }
            }
            else
                return 0.0;
        }
        else
            return 0.0;
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
                return 0.0;
        }
        else
            return 0.0;
    }
}

void GridWorld::SetGoal(unsigned goal, pair<unsigned,unsigned> coord, float reward)
{
    switch (goal)
    {
    case 0:
        mGoal0[coord] = reward;
        break;
    case 1:
        mGoal1[coord] = reward;
        break;
    case 2:
        mGoal2[coord] = reward;
        break;

    default:
        cout << "GridWorld::SetGoal Error while assigning goals" << endl;
        break;
    }
}

void  GridWorld::ExecuteActions (VariablesInfo* pActions)
{
    vector <unsigned int> cur_row;
    vector <unsigned int> cur_col;
    //cout << "ENVIRONMENT BEFORE = " << endl << mpStateInfo->ToString(2) << endl;
    assert(pActions->size() == mNumAgents);

    for (unsigned i = 0; i < mNumAgents; i++)
    {
        unsigned int action_value = (unsigned int) pActions->GetVariable(i)->GetVarValue();
        string *row = new string(ROW);
        string *col = new string(COL);
        for (unsigned k = 0; k < mNumAgents; k++)
        {
            float temp = 0.0;
            if (i==k)
            {
                row->append("_");
                col->append("_");
                ostringstream buffer;
                buffer << i;
                row->append(buffer.str());
                col->append(buffer.str());
                mpStateInfo->GetVarValue(*row, temp);
                cur_row.push_back((unsigned int) temp);
                mpStateInfo->GetVarValue(*col, temp);
                cur_col.push_back((unsigned int) temp);
            }
            else
            {
                string *row_aux = new string(ROW);
                string *col_aux = new string(COL);
                row_aux->append("_");
                col_aux->append("_");
                ostringstream buffer;
                buffer << k;
                row_aux->append(buffer.str());
                col_aux->append(buffer.str());
                mpStateInfo->GetVarValue(*row_aux, temp);
                cur_row.push_back((unsigned int) temp);
                mpStateInfo->GetVarValue(*col_aux, temp);
                cur_col.push_back((unsigned int) temp);
            }
        }
        //cout << "ACTION AGENT " << i <<": " << action_value  << endl;
        //cout << "before( " << cur_row[i] << " , " << cur_col[i]  <<" )" << endl;

        //vector< vector<unsigned> > change = ChangePosition(i,action_value,cur_row,cur_col, row, col);
        cur_row.clear();
        cur_col.clear();
        //cur_row.assign(change[0].begin(),change[0].end());
        //cur_col.assign(change[1].begin(),change[1].end());

        //cout << "after( " << cur_row[i] << " , " << cur_col[i]  <<" )" << endl;
        //cout << "ENVIRONMENT AFTER AGENT " << i << endl << mpStateInfo->ToString(2) << endl;
    }
}

/*vector < vector<unsigned> > GridWorld::ChangePosition (unsigned int i, unsigned int action_value,
        vector <unsigned int> cur_row,  vector <unsigned int> cur_col,
        string *row, string *col)
{
    if (AvailMove(i,action_value,cur_row,cur_col))
    {
        switch (action_value)
        {
        case D :
            mpStateInfo->SetVarValue(*row, cur_row[i]+1);
            cur_row[i]++;
            break;
 
        case R:
            mpStateInfo->SetVarValue(*col, cur_col[i]+1);
            cur_col[i]++;
            break;
 
        case U:
            mpStateInfo->SetVarValue(*row, cur_row[i]-1);
            cur_row[i]--;
            break;
 
        case L:
            mpStateInfo->SetVarValue(*col, cur_col[i]-1);
            cur_col[i]--;
            break;
        }
    }
    vector <vector<unsigned> > temp;
    temp.push_back(cur_row);
    temp.push_back(cur_col);
    return temp;
}*/

char GridWorld::GetGame() const
{
    return mGame;
}

string  GridWorld::ToString (unsigned int n) const
{
#if TOSTRING
    // cout << "GridWorld::ToString(" << n << ")" << endl;
    ostringstream str;
    str << Utils::Spaces(n) << "Rows = " << mNumRows << "; Cols = " << mNumCols << endl;
    str << Utils::Spaces(n) << "Error Probabilities: (" << mErrDown << ", "
    << mErrRight << ", " << mErrUp << ", " << mErrLeft << ")" << endl;
    str << Environment::ToString(n+2);
    return str.str();
#else

    return "";
#endif
}

string  GridWorld::ToShortString (unsigned int n) const
{
#if TOSTRING
    // cout << "GridWorld::ToShortString(" << n << ")" << endl;
    ostringstream str;
    str << Environment::ToShortString(n);
    return str.str();
#else

    return "";
#endif
}

string  GridWorld::GetInfo (unsigned int n) const
{
    ostringstream str;
    str  << "Grid:" << mGame<< endl;
    return str.str();

}

