#include <iostream>
#include <sstream>
#include <ctime>
#include <sys/time.h>
#include <time.h>
#include <getopt.h>
#include "include/Environment.h"
#include "environments/Bar.h"
#include "environments/GridWorld.h"
#include "environments/GridWorldWall.h"
#include "include/Variable.h"
#include "include/VariablesInfo.h"
#include "include/VI.h"
#include "include/CE_VI.h"
#include "include/constants.h"

int GetTime() {
	struct timeval tv;
	struct timezone tz;
	gettimeofday(&tv,&tz);
	return(tv.tv_sec*1000000+tv.tv_usec);
}

void AddAgentNames(vector<string>* agent_names) {
	/*agent_names->push_back("Leto");
	agent_names->push_back("Marcj");
	agent_names->push_back("Tattanke");
	agent_names->push_back("AndyBon");
	agent_names->push_back("Goodrich");
	agent_names->push_back("Stimpson");
	agent_names->push_back("Crandall");
	agent_names->push_back("Littman");
	agent_names->push_back("Stone");
	agent_names->push_back("Bowling");*/
	agent_names->push_back("Agent1");
	agent_names->push_back("Agent2");
	agent_names->push_back("Agent3");
	agent_names->push_back("Agent4");
	agent_names->push_back("Agent5");
	agent_names->push_back("Agent6");
	agent_names->push_back("Agent7");
	agent_names->push_back("Agent8");
	agent_names->push_back("Agent9");
	agent_names->push_back("Agent10");
}

void AddAgentActions(int num_actions, VariablesInfo* admissible_actions) {
	Variable* temp;
	string action_name("Action_");
	temp = new Variable(action_name.append(1,'R'),1.0,GRID_SIZE);		// Right ( value = 0)
	if (temp == NULL) {
		cerr << "main:AddAgentActions (R action) was NULL!" << endl;
		exit(NO_MEM_AVAILABLE);
	}
	temp->SetVarValue(0);
	admissible_actions->AddVariable(temp->Clone());
	delete temp;
	action_name.clear();
	action_name = "Action_";
	temp = new Variable(action_name.append(1,'L'),1.0,GRID_SIZE);		// Left (value = 1)
	if (temp == NULL) {
		cerr << "main:AddAgentActions (L action) was NULL!" << endl;
		exit(NO_MEM_AVAILABLE);
	}
	temp->SetVarValue(1);
	admissible_actions->AddVariable(temp->Clone());
	delete temp;
	action_name.clear();
	action_name = "Action_";
	temp = new Variable(action_name.append(1,'U'),1.0,GRID_SIZE);		// Up (value = 2)
	if (temp == NULL) {
		cerr << "main:AddAgentActions (U action) was NULL!" << endl;
		exit(NO_MEM_AVAILABLE);
	}
	temp->SetVarValue(2);
	admissible_actions->AddVariable(temp->Clone());
	delete temp;
	action_name.clear();
	action_name = "Action_";
	temp = new Variable(action_name.append(1,'D'),1.0,GRID_SIZE);		// Down (value = 3)
	if (temp == NULL) {
		cerr << "main:AddAgentActions (D action) was NULL!" << endl;
		exit(NO_MEM_AVAILABLE);
	}
	temp->SetVarValue(3);
	admissible_actions->AddVariable(temp->Clone());
	delete temp;
}

// Add agents (for the grid environment)
VariablesInfo* SetInitialState(unsigned NUM_AGENTS) {
	string name;
//	bool grid[GRID_SIZE][GRID_SIZE];
	unsigned i;
//	unsigned j;
/*	for (i = 0; i < GRID_SIZE; i++)
		for (j = 0; j < GRID_SIZE; j++)
			grid[i][j] = false;*/
	unsigned c;
	unsigned r;
	VariablesInfo* initial_state = new VariablesInfo();
	
//	vector<string>::iterator it = agent_names->begin();
	for (i = 0; i < NUM_AGENTS; i++) {
		std::ostringstream stream;
		stream<<i;
		//c = rand()%GRID_SIZE; // columns
		//r = rand()%GRID_SIZE; // rows
		if(i == 0){
			r = 2;
			c = 0; 
		}
		else{
			r = 2;
			c = 2;
		}
//		grid[r][c] = true;
		name = "Row_";
		Variable* state = new Variable(name.append(stream.str()),0.0,GRID_SIZE-1);
		state->SetVarValue(r);			// Random row
		initial_state->AddVariable(state->Clone());
		delete state;
		name = "Col_";
		state = new Variable(name.append(stream.str()),0.0,GRID_SIZE-1);
		state->SetVarValue(c);			// Random column
		initial_state->AddVariable(state->Clone());
		delete state;
	}
	return initial_state;
}

// Add tokens (for the grid environment)
VariablesInfo* AppendToInitialState(VariablesInfo* AgentPositions, unsigned NUM_AGENTS) {
	// now it receives the environment description which contains ONLY the position of all agents
	string counter;
	string token;
	unsigned c;
	unsigned r;
	unsigned i = 0;
	unsigned j = 0;
	bool grid[GRID_SIZE][GRID_SIZE];
	for (i = 0; i < GRID_SIZE; i++)
		for (j = 0; j < GRID_SIZE; j++)
			grid[i][j] = false;
	Sgi::hash_map<int, Variable*> state;
	AgentPositions->GetVariableList(state);
	Sgi::hash_map<int, Variable*>::iterator it = state.begin();
	while (it != state.end()) {
		r = (int) (it->second->GetVarValue()); // row
		it++;
		c = (int) (it->second->GetVarValue()); // column
		grid[r][c] = true;
		it++;
	}
	VariablesInfo* initial_state = new VariablesInfo();
	for (i = 0; i < NUM_TOKENS_A; i++) {
		token = "token_";
		counter = i;
		//r = rand()%GRID_SIZE; // row
		//c = rand()%GRID_SIZE; // column
		r = 0;
		c = 2;
		/*	while (grid[r][c]) {
			r = rand()%GRID_SIZE;
			c = rand()%GRID_SIZE;
			}*/
		grid[r][c] = true;
		token.append("Row").append("0").append(string(counter));
		Variable* state = new Variable(token,0.0,GRID_SIZE);
		state->SetVarValue(r);		// row
		initial_state->AddVariable(state->Clone());
		delete state;
		token = "token_";
		token.append("Col").append("0").append(string(counter));
		state = new Variable(token,0.0,GRID_SIZE);
		state->SetVarValue(c);		// column
		initial_state->AddVariable(state->Clone());
		delete state;
		//cout << "appended token " << i << " for agent A" << endl;
	}

	for (i = 0; i < NUM_TOKENS_B; i++) {
		token = "token_";
		counter = i;
		//r = rand()%GRID_SIZE; // row
		//c = rand()%GRID_SIZE; // column
		r = 0;
		c = 0;
		/*	while (grid[r][c]) {
			r = rand()%GRID_SIZE;
			c = rand()%GRID_SIZE;
			}*/
		grid[r][c] = true;
		token.append("Row").append("1").append(string(counter));
		Variable* state = new Variable(token,0.0,GRID_SIZE);
		state->SetVarValue(r);		// row
		initial_state->AddVariable(state->Clone());
		delete state;
		token = "token_";
		token.append("Col").append("1").append(string(counter));
		state = new Variable(token,0.0,GRID_SIZE);
		state->SetVarValue(c);		//column
		initial_state->AddVariable(state->Clone());
		delete state;
		//cout << "appended token " << i << " for agent B" << endl;
	}
	return initial_state;
}

void PrintUsage(string beforeMessage, int exitValue) {
	cout << endl << "** " << beforeMessage << " **" << endl;
	cout << "Parameters allowed (those marked with * are mandatory):" << endl;
	cout << "\t-h (--help) -->: print this message);" << endl;
	cout << "\t(*) -a (--algorithm) --> the algorithm used:" <<endl;
	cout << "\t\tC --> CE-VI" <<endl;
	cout << "\t\tF --> Friend-VI" <<endl;
	cout << "\t\tM --> MiniMax" <<endl;
	cout << "\t\tT --> FNS (folk Nash solver)" <<endl;
	cout << "\t\t      Max-EF (maximum efficient folk)" <<endl;
	cout << "\t\t      Max-ERE (maximum efficient repeated equilibrium)" <<endl;
	cout << "\t(*) -o (--option) --> the specific flavor of CE (u,e,r)" <<endl;
	cout << "\t(*) -w (--weight) --> the weight [0,1] assigned to agent 1" <<endl;
	cout << "\t(*) -g (--gamma) --> gamma" <<endl;
	cout << "\t(*) -e (--environment) --> environment to be used:" <<endl;
	cout << "\t\tG --> coordination (3x3)" <<endl;
	cout << "\t\tT --> team (3x3)" <<endl;
	cout << "\t\tC --> chicken (3x3) with walls" <<endl;
	cout << "\t\tP --> prisoner's dilemma (9x2) with walls" <<endl;
	cout << "\t\tX --> compromise game" <<endl;
	cout << "\t\tA --> asymmetric game" <<endl;
	cout << "Now I will exit" << endl;
	exit (exitValue);
}

void test_environment(Environment* e) {
	unsigned tokens = 0;
	VariablesInfo* state = e->GetStateDescription();
	Sgi::hash_map<int, Variable*> actions;
	state->GetVariableList(actions);
	Sgi::hash_map<int, Variable*>::iterator it;
	it = actions.begin();
	cout << endl << "main: Test environment" << endl;
	while (it != actions.end()) {
		if (it->second->GetVarName().find("Token_") != string::npos)
			tokens++;
		cout << "\t\t*" << (it->first)+1 << " -->\t" << it->second->GetVarName() << " ha valore " << it->second->GetVarValue() <<endl;
		it++;
	}
	cout << endl;
	cout << "main:" << endl << "\t--> no. of tokens for A: " << NUM_TOKENS_A << "; for B: " << NUM_TOKENS_B << endl << "\t--> no. of tokens NOT taken = " << tokens/2 << endl;
	delete state;
}

int main(int argc, char* argv[]) {
	clock_t begin_main = GetTime();
	clock_t end_main;
	bool is_verbose = false;
#ifdef OUTPUT
    is_verbose = true;
#endif
 	float discount_factor = 0.0;  /* gamma */
	float max_reward = 0.0;
	float weight = 0.0;
	char algorithm = 0;
	char opt = 0;
	unsigned int NUM_ACTIONS = 4;
	unsigned int NUM_AGENTS = 2;
	GridWorld *environment = NULL;
	VariablesInfo* stateDescription = NULL;
	int next_option = 0;
	int num_options = 0;
	/* A string listing valid short options letters:
		h = help
		a = number of actions
		t = number of agents
		m = algorithm (C=CoLF, K = CKCoLF)
	 	g = gamma
		p = number of steps for each trial
		e = environment (G = grid for now)
	*/
	const char* const short_options = "h:a:o:w:g:e:";
	/* An array describing valid long options (see above). */
	const struct option long_options[] = {
		{"help", 0, NULL, 'h'},
		{"algorithm", 1, NULL, 'a'},
		{"option", 1, NULL, 'o'},
		{"weight", 1, NULL, 'w'},
	 	{"gamma", 1, NULL, 'g'},
		{"environment", 1, NULL, 'e'},
		{NULL, 0, NULL, 0}   /* Required at end of array.  */
	};
	srand(time(NULL));
	vector<VI*> agents;
	vector<string> agent_names;
	do {
		next_option = getopt_long (argc, argv, short_options, long_options, NULL);
		switch (next_option) {
			case 'h': {		/* -h or --help */
				PrintUsage("Help message",EXIT_OK);
			}
			case 'a': {		/* -m or --algorithm --> learning algorithm for self play (C, K)  */
			  
			  if (*optarg == 'C' || *optarg == 'F'||*optarg == 'M'||*optarg == 'T')		  
			    algorithm = *optarg;
			  else
			    PrintUsage("Invalid algorithm, only \'C\' or \'F\' or \'M\' or \'T\' are allowed!",BAD_USAGE);
			  //cout << "Algorithm: " << algorithm << endl;
			  num_options++;
			  break;
			}
			case 'o': {	
			  if (*optarg == 'u' || *optarg == 'e'||*optarg == 'r')
			    opt = *optarg;
				else
			    PrintUsage("Invalid algorithm option, only \'u\' or \'e\' or \'r\' are allowed!",BAD_USAGE);
			  //cout << "Algorithm: " << algorithm << endl;
			  num_options++;
			  break;
			}
			case 'w':
				weight = atof(optarg);
		 	 	if (weight < 0 || weight > 1)
					PrintUsage("Invalid weight, it must be between 0 and 1", BAD_USAGE);
				//cout << "Gamma: " << discount_factor <<endl;;
				num_options++;
				break;
				
			case 'g': {		/* -g or --gamma --> discount factor */
				discount_factor = atof(optarg);
				if (discount_factor < 0 || discount_factor > 1)
					PrintUsage("Invalid discount_factor, it must be between 0 and 1", BAD_USAGE);
				//cout << "Gamma: " << discount_factor <<endl;;
				num_options++;
				break;
			}

			case 'e': {		/* -e or --environment --> environment choosen (G) */
				if ( *optarg == 'T') {
					if (NUM_AGENTS >= 8)
						PrintUsage("Too many agents for the grid environment; maximum allowed = 7\n", BAD_USAGE);
					environment = new GridWorld(*optarg);
				}
				else if(*optarg == 'C'||*optarg == 'P'||*optarg == 'X'||*optarg == 'G'||*optarg == 'A')
					environment = new GridWorldWall(*optarg);
				else
					PrintUsage("Invalid environment", BAD_USAGE);
				//cout << "Environment: " << *optarg << endl;
				num_options++;
				break;
			}
			case '?': {		/* The user specified an invalid option.  */
				PrintUsage (string("Unknown option \'").append(optarg).append("\'"), BAD_USAGE);
			}
			case -1: {			/* Done with options.  */
				if (num_options != 5)
					PrintUsage("Mandatory options not present", BAD_USAGE);
				break;
			}
			default:    /* Something else unexpected.  */
				abort();
		}
	} while (next_option != -1);
	VariablesInfo* admissible_actions = new VariablesInfo();
	CE_VI* agent = NULL;
	AddAgentNames(&agent_names);
	
	AddAgentActions(NUM_ACTIONS, admissible_actions);
	// Set the intial state of grid environment with random position for all agents
	// Obviously is passed as argument to each agent...
	stateDescription = SetInitialState(NUM_AGENTS);
	environment->Init(stateDescription);

	//stateDescription = environment->GetStateDescription();
	
	//environment->AppendInit(AppendToInitialState(stateDescription,NUM_AGENTS));
	cout << "****" << endl;
	if (algorithm == 'C' || algorithm == 'F'|| algorithm == 'M'|| algorithm == 'T') {
	cout << "Creating VI..." <<endl << "algorithm: "<<algorithm<< "  option:" << opt<<endl;
	agent = new CE_VI(agent_names[0],
		       0,
		       NUM_AGENTS,
		       admissible_actions,
		       discount_factor,
		       max_reward,
		       is_verbose,
		       algorithm,
		       opt,
		       weight,
		       environment);
		if (agent == NULL) {
			cerr << "main: VI creation NULL!" << endl;
			exit(NO_MEM_AVAILABLE);
		}
	}
	else {
		cout << "The specified algorithm is not available" << endl;
	  	cout << "The only algorithms available are:" << endl;
	  	cout << "'C': CE-VI" << endl;
	  	cout << "'F': Friend-VI" << endl;
	  	cout << "'M': MiniMax-VI" << endl;
	  	cout << "'T': TB-VI" << endl;
	  	exit(1);
	}
		
	agent->SetTransitionModel(environment->GetTransitionModel());

	
	//FOR DEBUG	
	/*vector <unsigned> vState;
	for (unsigned i= 0; i<81;i++)
	{
		cout << "State " << i;
		vState = environment->GetFromIndex(i,4,3);
		cout <<": " << vState[0] << "," <<vState[1] << "," <<vState[2] << "," <<vState[3] <<endl;
		cout << "<-state:" << environment->ComputeJointIndex(vState, 3) << endl;
	}*/
	
	agent->Execute(); // select and execute action
	//agent->BuildConvexHull();

	delete stateDescription;
	stateDescription = NULL;
	
	//test_environment(environment);
	//cout << "TOTAL PAYOFF --> G = " << endl << endl;
	/*for (i = 0; i < agents.size(); i++) {
		cout << agents[i]->PrintQTable();
	}*/
	delete admissible_actions;
	delete environment; 

	// perché diavolo segmenta?!?
	/*for (i = 0; i < agents.size(); i++) {
		cout << "main: delete agent " << i << endl; flush(cout);
		delete agents[i];
	}*/

	end_main = GetTime();
	cout << ">>> main.cpp: elapsed time = " << (double)(end_main-begin_main)/CLOCKS_PER_SEC << " seconds" <<endl;
	return 0;
}
