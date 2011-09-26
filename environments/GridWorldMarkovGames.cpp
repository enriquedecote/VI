// SYSTEM INCLUDES
#include <sstream>
#include <assert.h>

// LOCAL INCLUDES
#include "GridWorldMarkovGames.h"
#include "TransitionModel.h"

extern "C" GridWorldMarkovGames* GridWorldMarkovGamesRef()
{
    return new GridWorldMarkovGames();
}

GridWorldMarkovGames::GridWorldMarkovGames()
{}

GridWorldMarkovGames::~GridWorldMarkovGames()
{}

void  GridWorldMarkovGames::Init ()
{
    if(Holes_elem!=NULL)
    {
        DOMNodeList* Holes_list = Holes_elem->getElementsByTagName(C2X("Hole"));
        for (unsigned int i = 0; i < (Holes_list->getLength()); i++)
        {
            DOMElement* Hole_elem = (DOMElement*) Holes_list->item(i);
            string row = X2C(Hole_elem->getAttributeNode(C2X("Row")) -> getValue());
            string col = X2C(Hole_elem->getAttributeNode(C2X("Col")) -> getValue());

            pair<int,int> p;
            p.first  = atoi(row.data());
            p.second = atoi(col.data());
            mHoles[p] = true;
            cout<<"Hole in x:"<<p.first<<"\ty:"<<p.second<<endl;
        }
    }
    mNumAgents = mpStateInfo->size()/2;
    
    //cout << "  number of agents: " << mNumAgents/2 << endl;
    for (unsigned i = 0; i < mNumAgents; i++)
    {
        string *row = new string(ROW);
        string *col = new string(COL);
        row->append(Utils::xSeparator);
        col->append(Utils::xSeparator);
        ostringstream buffer;
        buffer << i;
        const Variable*const row_var = mpStateInfo->GetVariable(row->append(buffer.str()));
        const Variable*const col_var = mpStateInfo->GetVariable(col->append(buffer.str()));
        mFirstRow.push_back((unsigned int) row_var->GetMinValue());
        mLastRow.push_back((unsigned int) row_var->GetMaxValue());
        mFirstCol.push_back((unsigned int) col_var->GetMinValue());
        mLastCol.push_back((unsigned int) col_var->GetMaxValue());
        mAgentRow.push_back((unsigned int) row_var->GetVarValue());
        mAgentCol.push_back((unsigned int) col_var->GetVarValue());
        /*cout << "  agent " << i+1 << endl;
        cout << "    first row = " << mFirstRow[i] << endl;
        cout << "    last row = " << mLastRow[i] << endl;
        cout << "    first col = " << mFirstCol[i] << endl;
        cout << "    last col = " << mLastCol[i] << endl;*/
        delete row;
        delete col;
    }

    mNumRows = mLastRow[0] - mFirstRow[0] + 1;
    mNumCols = mLastCol[0] - mFirstCol[0] + 1;
    
    //transitionModel = new SASTransitionModel();
    GenerateTransitionModel();
    

}

void  GridWorldMarkovGames::Reset ( )
{
  // GridWorldWall::Reset();
    for (unsigned i = 0; i < mNumAgents; i++)
    {
        bool ret;
        string *row = new string(ROW);
        string *col = new string(COL);
        row->append(Utils::xSeparator);
        col->append(Utils::xSeparator);
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

void  GridWorldMarkovGames::ResetRandom ( )
{
    GridWorldWall::ResetRandom();
}

void  GridWorldMarkovGames::ResetForTest ( )
{
    // position the first agent...
    unsigned int row = (unsigned int) rand() % 9;
    unsigned int col = (unsigned int) rand() % 9;
    pair<int,int> p;
    p.first=row;
    p.second=col;

    mpStateInfo -> SetVarValue(ROW,0);
    mpStateInfo -> SetVarValue(COL, 0);
}

const SASTransitionModel*const  GridWorldMarkovGames::GetTransitionModel ()
{
  //cout << "Environment TansitionModel: " << &transitionModel << endl;
  return &transitionModel;
}

void  GridWorldMarkovGames::ExecuteActions (const VariablesInfo*const pActions)
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
	      row->append(Utils::xSeparator);
	      col->append(Utils::xSeparator);
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
	      row_aux->append(Utils::xSeparator);
	      col_aux->append(Utils::xSeparator);
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

      vector< vector<unsigned> > change = ChangePosition(i,action_value,cur_row,cur_col, row, col);
      cur_row.clear();
      cur_col.clear();
      cur_row.assign(change[0].begin(),change[0].end());
      cur_col.assign(change[1].begin(),change[1].end());
	  
      //cout << "after( " << cur_row[i] << " , " << cur_col[i]  <<" )" << endl;
      //cout << "ENVIRONMENT AFTER AGENT " << i << endl << mpStateInfo->ToString(2) << endl;
    }
}

vector < vector<unsigned> > GridWorldMarkovGames::ChangePosition (unsigned int i, unsigned int action_value, 
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
}

unsigned int GridWorldMarkovGames::ComputeNextState( vector <unsigned int> state, unsigned int jointAction)
{
  vector <unsigned int> row;
  row.push_back(state[2]);
  row.push_back(state[3]);
  vector <unsigned int> col;
  col.push_back(state[0]);
  col.push_back(state[1]);
  vector <unsigned int> next_state;

  cout <<"**COMPUTE NEXT STATE**" << endl;
 
  //get the actions of each agent from the available joint actions
  vector <unsigned int> actions = GetFromIndex(jointAction, 2, 4);
  for (unsigned i=0; i<mNumAgents; i++)
    {
      switch (actions[i])
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
  unsigned next_index = ComputeJointIndex(next_state, 3);
  cout << "Prev state:" << state[0]<<","<<state[1]<<","<<state[2]<<","<<state[3]<<endl;
  cout << "actions:" << jointAction<< ": "<<actions[0]<<","<<actions[1]<<"->"<< col[0]<<","<<col[1]<<","<<row[0]<<","<<row[1]
       << ": " << next_index << endl;
  return next_index;
}

vector <unsigned> GridWorldMarkovGames::ComputeNextState( vector <unsigned int> state, vector <unsigned int> jointAction)
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

vector <unsigned>  GridWorldMarkovGames::AvailJointMoves( vector <unsigned int> state)
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
  
  cout<< "**AVAIL JOINT MOVES**"<< endl;
  cout << "current state: "<< state[0] << "," <<state[1] << "," <<state[2] << "," << state[3]<< endl;
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
	      cout << "Avail joint action:" << temp << ": " << j << "," << i <<endl;
	      jointMoves.push_back(temp);
	    }
	  jointMove.clear();
	}
    }
  return jointMoves;
}

bool  GridWorldMarkovGames::AvailMove(unsigned int i, unsigned int action_value,
		       vector <unsigned int> cur_row,  vector <unsigned int> cur_col)
{
  pair<int,int> p;
  switch (action_value)
    {
    case D :
      p.first  = cur_row[i]+1;
      p.second = cur_col[i];
      if (cur_row[i] < mLastRow[i] && mHorWall.count(p) < 1.0 && mVerWall.count(p) < 1.0)
	{
	  if (i==0)
	    {	      
	      if (!(cur_row[i]+1 == cur_row[i+1] && cur_col[i] == cur_col[i+1]))
		{
		  return true;
		}
	      else return false;
	    }
	  else 
	    {	      
	      if (!(cur_row[i]+1 == cur_row[i-1] && cur_col[i] == cur_col[i-1]))
		{
		  return true;
		}
	      else return false;
	    }
	}
      else return false;
      break;

    case R :
      if (cur_col[i] < mLastCol[i] && mVerWall.count(p) < 1.0 && mHorWall.count(p) < 1.0)
	{
	  if (i==0)
	    {	      
	      if (!(cur_col[i]+1 == cur_col[i+1] && cur_row[i] == cur_row[i+1]))
		{
		  return true;
		}
	      else return false;
	    }
	  else 
	    {	      
	      if (!(cur_col[i]+1 == cur_col[i-1] && cur_row[i] == cur_row[i-1]))
		{
		  return true;
		}
	      else return false;
	    }
	}
      else return false;
      break;

    case U:
       if (cur_row[i] > mFirstRow[i] && mHorWall.count(p) < 1.0 && mVerWall.count(p) < 1.0)
	{
	  if (i==0)
	    {	      
	      if (!(cur_row[i]-1 == cur_row[i+1] && cur_col[i] == cur_col[i+1]))
		{
		  return true;
		}
	      else return false;
	    }
	  else 
	    {	      
	      if (!(cur_row[i]-1 == cur_col[i-1] && cur_col[i] == cur_col[i-1]))
		{ 
		  return true;
		}
	      else return false;
	    }
	}
       else return false;
       break;

    case L :
      if (cur_col[i] > mFirstCol[i] && mVerWall.count(p) < 1.0 && mHorWall.count(p) < 1.00)
 	{
	  if (i==0)
	    {	      
	      if (!(cur_col[i]-1 == cur_col[i+1] && cur_row[i] == cur_row[i+1]))
		{
		  return true;
		}
	      else return false;
	    }
	  else 
	    {	      
	      if (!(cur_col[i]-1 == cur_col[i-1] && cur_row[i] == cur_row[i-1]))
		{
		  return true;
		}
	      else return false;
	    }
	}
      else return false;
      break;
    }
}

bool  GridWorldMarkovGames::AvailMove( vector <unsigned int> state)
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
	  if ( mHorWall.count(p) < 1.0 && mVerWall.count(p) < 1.0)
	    truth.push_back(true);
	}
      else
	truth.push_back(false);
    }
  if(truth.at(0)==truth.at(1) && truth.at(0)== true)
    return true;
  else return false;
}

vector <unsigned> GridWorldMarkovGames::GetFromIndex(unsigned int index, unsigned int size, unsigned base)
{
  vector <unsigned> vec;
  unsigned int cur_joint_action = index;
  unsigned reminder;
  for (unsigned int i = 0; i < size; i++)
    {
      reminder = cur_joint_action % base;
      vec.push_back( reminder);
      cur_joint_action = cur_joint_action/base;
    }
  return vec;
}

unsigned int  GridWorldMarkovGames::ComputeJointIndex  (vector <unsigned> vec, unsigned int size)
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

void  GridWorldMarkovGames::GenerateTransitionModel ( )
{
  //unsigned int stateSize = mNumRows*mNumCols;
  vector <unsigned> state(4,0);
  // vector <unsigned> actions;
  unsigned s1,s2;
  
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
		      state.push_back(z);
		      state.push_back(y);
		      state.push_back(x);
		      state.push_back(w);
		      s1 = ComputeJointIndex(state, 4);
		      vector <unsigned> actions = AvailJointMoves(state);
		      for (unsigned a = 0; a<actions.size(); a++) //cycle through all avalilable joint actions
			{
			  s2 = ComputeNextState(state, actions[a]);
			  transitionModel.SetProbability(s1,actions[a],s2,1.0);
			}
		    }
		  state.clear();
		}
	    }
	}
    }
  
}

string  GridWorldMarkovGames::ToString (unsigned int n) const
{
#if TOSTRING
    // cout << "GridWorldMarkovGames::ToString(" << n << ")" << endl;
    ostringstream str;
    str << Utils::Spaces(n) << "GRID WORLD ENVIRONMENT" << endl;
    str << Utils::Spaces(n) << "  Rows = " << mNumRows << "; Cols = " << mNumCols << endl;
    str << Utils::Spaces(n) << "  Error Probabilities: (" << mErrDown << ", "
    << mErrRight << ", " << mErrUp << ", " << mErrLeft << ")" << endl;

    map<pair<int,int>, bool>::const_iterator wall_it;

    str << Utils::Spaces(n) << "  Horizontal Walls: " << endl;
    str << Utils::Spaces(n) << "  ";
    for (wall_it = mHorWall.begin(); wall_it != mHorWall.end(); wall_it++)
    {
        pair<int,int> p = wall_it->first;
        str << "(" << p.first << ", " << p.second << ") ";
    }
    str << endl;

    str << Utils::Spaces(n) << "  Vertical Walls: " << endl;
    str << Utils::Spaces(n) << "  ";
    for (wall_it = mVerWall.begin(); wall_it != mVerWall.end(); wall_it++)
    {
        pair<int,int> p = wall_it->first;
        str << "(" << p.first << ", " << p.second << ") ";
    }
    str << endl;

    map<pair<int,int>, bool>::const_iterator holes_it;
    str << Utils::Spaces(n) << "  Holes: " << endl;
    str << Utils::Spaces(n) << "  ";
    for (holes_it = mHoles.begin(); holes_it != mHoles.end(); holes_it++)
    {
        pair<int,int> p = holes_it->first;
        str << "(" << p.first << ", " << p.second << ") ";
    }
    str << endl;

    str << Environment::ToString(n+2);
    return str.str();
#else

    return "";
#endif
}

string  GridWorldMarkovGames::ToShortString (unsigned int n) const
{
#if TOSTRING
    // cout << "GridWorldMarkovGames::ToShortString(" << n << ")" << endl;
    ostringstream str;
    str << Environment::ToShortString(n);
    return str.str();
#else

    return "";
#endif
}

