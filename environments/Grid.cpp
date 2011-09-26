#include <iostream>
#include <sstream>
#include <ext/hash_map>
#include <mystl.h>
#include "Grid.h"
#include "../models/TransitionModel.h"
using namespace std;

/*
 * CONVENZIONE IMPIEGATA --> grid[riga][colonna] --> origine in alto a sx
 */

// just a very stupid debugging function ;) --> 2B removed after testing...
void printGrid(VariablesInfo* aGrid[GRID_SIZE][GRID_SIZE]) {
	unsigned i;
	unsigned j;
	unsigned counter;
	for (i = 0, counter = 1; i < GRID_SIZE; i++) {
		for (j = 0; j < GRID_SIZE; j++) {
			if (aGrid[i][j] != NULL) {
				Sgi::hash_map<int, Variable*> cell;
				Sgi::hash_map<int, Variable*>::iterator it;
				aGrid[i][j]->GetVariableList(cell);
				it = cell.begin();
				cout << "\t[" << counter++ << "] (" << i << "," << j << ") ";
				while (it != cell.end()) {
					cout << it->second->GetVarName() << "\t(val=" << it->second->GetVarValue() << "),\t";
					it++;
				}
				cout << endl;
			}
		}
	}
}

Grid::Grid():Environment() {
	unsigned i;
	unsigned j;
	for (i = 0; i < GRID_SIZE; i++)
		for (j = 0; j < GRID_SIZE; j++) {
			grid[i][j] = NULL;
			gainGrid[i][j] = NULL;
			copyGrid[i][j] = NULL;
			originalGrid[i][j] = NULL;
		}
	partialGlobalPayoff = 0.0;
	grandTotalPayoff = 0.0;
	othersPayoff = 0.0;
	agentCounter = 0;
	numAgents = 0;
	isTheEndOfThisStep = false;
}

Grid::~Grid() {
	cout << "Grid::~Grid --> Destructor called" << endl;
	Reset();
}

void  Grid::CustomInit() {
}

void  Grid::Reset() {
	cout << "Grid::Reset" << endl;
	// mpStateInfo is deleted in Environmnent, do NOT delete it here!
	unsigned i;
	unsigned j;
	for (i = 0; i < GRID_SIZE; i++) {
		for (j = 0; j < GRID_SIZE; j++) {
			if (grid[i][j] != NULL)
				delete grid[i][j];
			grid[i][j] = NULL;
			if (originalGrid[i][j] != NULL)
				delete originalGrid[i][j];
			originalGrid[i][j] = NULL;
			if (copyGrid[i][j] != NULL)
				delete copyGrid[i][j];
			copyGrid[i][j] = NULL;
			if (gainGrid[i][j] != NULL)
				delete gainGrid[i][j];
			gainGrid[i][j] = NULL;
		}
	}
	partialGlobalPayoff = 0.0;
	othersPayoff = 0.0;
	agentCounter = 0;
	numAgents = 0;
	isTheEndOfThisStep = false;
	Sgi::hash_map<string, VariablesInfo*>::iterator it = mpDependenciesMap.begin();
	while (it != mpDependenciesMap.end()) {
		delete it->second;
		it++;
	}
}

// pAction is the initial placement of all agents
void Grid::Init(VariablesInfo* pActions) {
	mpStateInfo = pActions->Clone();
	Sgi::hash_map<int, Variable*> state;
	mpStateInfo->GetVariableList(state);
	Sgi::hash_map<int, Variable*>::iterator it = state.begin();
	int r;
	int c;
	string agentName;
	Variable* temp = NULL;
	while (it != state.end()) {
		//coord = (int) (it->second->GetVarValue());
		agentName = it->second->GetVarName().substr(
				it->second->GetVarName().find("_",0)+1).substr(
				0,it->second->GetVarName().substr(
						it->second->GetVarName().find("_",0)+1).find("_"));
		r = (int) (it->second->GetVarValue()); // row
		it++;
		c = (int) (it->second->GetVarValue()); // column
		temp = new Variable(agentName,0.0,0.0);
		if (temp == NULL) {
			cout << endl << endl << "\t\tGrid::Init --> no memory available, now I will exit!" << endl << endl;
			exit(NO_MEM_AVAILABLE);
		}
		temp->SetVarValue(0.0);
		grid[r][c] = new VariablesInfo();
		if (grid[r][c] == NULL) {
			cout << endl << endl << "\t\tGrid::Init --> no memory available, now I will exit!" << endl << endl;
			exit(NO_MEM_AVAILABLE);
		}
		grid[r][c]->AddVariable(temp->Clone());
		cout << "Aggiungo in (" << r << "," << c << ") l'agente \"" << temp->GetVarName() << "\"" << endl;
		delete temp;
		it++;
		numAgents++;
	}
	agentCounter = 0;
	GenerateTransitionModel();
}

// pAction is the initial placement of all tokens
void Grid::AppendToInit(VariablesInfo* pActions) {
	Sgi::hash_map<int, Variable*> state;
	pActions->GetVariableList(state);
	Sgi::hash_map<int, Variable*>::iterator it = state.begin();
	int row;
	int column;
	Variable* temp = NULL;
	int tokenCounter = 0;
	string tokenName;
	unsigned i;
	unsigned j;
	while (it != state.end()) {
		mpStateInfo->AddVariable(it->second);
		row = (int) (it->second->GetVarValue());
		it++;
		column = (int) (it->second->GetVarValue());
		tokenName = "Token_";
		temp = new Variable(tokenName.append(1,tokenCounter+OFFSET),0.0,1.0);
		tokenCounter++;
		if (temp == NULL) {
			cout << endl << endl << "\t\tGrid::AppendToInit --> no memory available, now I will exit!" << endl << endl;
			exit(NO_MEM_AVAILABLE);
		}
		//	randomValue = rand()%9+1;
		//	temp->SetVarValue(1/randomValue); // value of the token is in [0.1;1]
		temp->SetVarValue(10);
		// position occupied by a token
		grid[row][column] = new VariablesInfo();
		if (grid[row][column] == NULL) {
			cout << endl << endl << "\t\tGrid::AppendToInit --> no memory available, now I will exit!" << endl << endl;
			exit(NO_MEM_AVAILABLE);
		}
		grid[row][column]->AddVariable(temp->Clone());
		cout << "Aggiungo in (" << row << "," << column << ") il token \"" << temp->GetVarName() << "\" con valore " << temp->GetVarValue() << endl;
		delete temp;
		it++; // next token
	}
	for (i = 0; i < GRID_SIZE; i++) { // this initialization is necessary!
		for (j = 0; j < GRID_SIZE; j++) {
			originalGrid[i][j] = NULL;
		}
	}
	BackupEnvironmentConfiguration(originalGrid); // originalGrid is used after to obtain the position of all the tokens
	/*cout << "originalGrid" << endl;
	printGrid(originalGrid); flush(cout);
	cout << endl << "grid" << endl;
	printGrid(grid); flush(cout);
	cout << endl;*/
}

// create a copy of "grid" in "aGrid": aGrid <- grid
void Grid::BackupEnvironmentConfiguration(VariablesInfo* aGrid[GRID_SIZE][GRID_SIZE]) {
	unsigned i;
	unsigned j;
	for (i = 0; i < GRID_SIZE; i++)
		for (j = 0; j < GRID_SIZE; j++) {
			if (aGrid[i][j] != NULL)
				delete aGrid[i][j];
			aGrid[i][j] = NULL;
			if (grid[i][j] != NULL) {
				aGrid[i][j] = grid[i][j]->Clone();
			}
		}
				
}

// substitute "grid" with "aGrid": grid <-aGrid
void Grid::RestoreEnvironmentConfiguration(VariablesInfo* aGrid[GRID_SIZE][GRID_SIZE]) {
	unsigned i;
	unsigned j;
	for (i = 0; i < GRID_SIZE; i++)
		for (j = 0; j < GRID_SIZE; j++) {
			if (grid[i][j] != NULL)
				delete grid[i][j];
			grid[i][j] = NULL;
			if (aGrid[i][j] != NULL)
				grid[i][j] = aGrid[i][j]->Clone();
		}
}

// Remove an agent from the grid "aGrid"
void Grid::RemoveAgent(string AgentName, VariablesInfo* aGrid[GRID_SIZE][GRID_SIZE]) {
	unsigned i;
	unsigned j;
	for (i = 0; i < GRID_SIZE; i++) {
		for (j = 0; j < GRID_SIZE; j++) {
			if (aGrid[i][j] != NULL) {
				Sgi::hash_map<int, Variable*> cell;
				Sgi::hash_map<int, Variable*>::iterator it;
				aGrid[i][j]->GetVariableList(cell);
				it = cell.begin();
				while (it != cell.end()) {
					if (it->second->GetVarName() == AgentName) {  // agent found
						aGrid[i][j]->Clear();
						delete aGrid[i][j];
						aGrid[i][j] = NULL;
					}
					it++;
					if (it != cell.end()) {
						cout << endl << endl << "\t\tGrid::RemoveAgent --> malformed cell!" << endl;
						cout << "\t\t cell (" << i << "," << j << ") contains " << it->second->GetVarName() << endl << endl;
						exit(SOMETHING_WRONG);
					}
				}
			}
		}
	}
}

void Grid::RestoreTokensTakenByAgent(string AgentName, VariablesInfo* aGrid[GRID_SIZE][GRID_SIZE]) {
	bool enter = false;
	unsigned i;
	unsigned j;
	VariablesInfo* tokensTaken = GetDependencies(AgentName); // tokens taken by AgentName
	if (tokensTaken == NULL) {
		//cout << "        " << AgentName << ": non ho nessun token da riposizionare, esco" << endl; flush(cout);
		return;  // agent hasn't taken any token 'til now, then there's nothing to restore
	}
	Sgi::hash_map<int, Variable*> tokens;  // tokens taken by AgentName
	Sgi::hash_map<int, Variable*>::iterator it_tokens;
	tokensTaken->GetVariableList(tokens);
	it_tokens = tokens.begin();
	while (it_tokens != tokens.end()) { // analize all tokens taken by AgentName
		/*
		 * Given a token taken by AgentName (it_tokens), it will look for the original position of that token
		 * (it uses originalGrid to determine that position)
		 */
		for (i = 0; i < GRID_SIZE; i++) {
			for (j = 0; j < GRID_SIZE; j++) {
				if (originalGrid[i][j] != NULL) {
					Sgi::hash_map<int, Variable*> originalCell; // original grid configuration
					Sgi::hash_map<int, Variable*>::iterator it_originalCell;
					originalGrid[i][j]->GetVariableList(originalCell);
					it_originalCell = originalCell.begin();
					while (it_originalCell != originalCell.end()) {
						if (it_originalCell->second->GetVarName() == it_tokens->second->GetVarName()) {
							//*cout << "        " << AgentName << ": il token " << it_tokens->second->GetVarName();
							//*cout << " (val = " << it_tokens->second->GetVarValue() << ") è in (" << i << "," << j << "), lo riposiziono"; flush(cout);
							/* 
							 * Position of token it_tokens found in (i,j), let's put down it in "grid"
							 * Possible case history for a cell of "grid"
							 *  1. empty
							 *  2. cointains only one agent
							 */
							if (aGrid[i][j] == NULL) { // empty cell --> put down the token found (case 1)
								aGrid[i][j] = new VariablesInfo();
								aGrid[i][j]->AddVariable(it_tokens->second->Clone());
								//*cout << " -- ok!" << endl; flush(cout);
								enter = true;
								it_originalCell = originalCell.end(); // così esce dal while interno su it_originalCell
							}
							else { // cell with an agent (case 2)
								//*cout << " -- !!! CELLA OCCUPATA DA "; flush(cout);
								Sgi::hash_map<int, Variable*> cell;
								Sgi::hash_map<int, Variable*>::iterator it_cell;
								aGrid[i][j]->GetVariableList(cell);
								it_cell = cell.begin();
								if (it_cell != cell.end()) {
									//*cout << it_cell->second->GetVarName(); flush(cout);
									if (it_cell->second->GetVarName().find("Token_") != string::npos) {
										cout << endl << endl << "\t\tGrid::RestoreTokensTakenByAgent --> Agent " << AgentName;
										cout << " -- grid(" << i << "," << j << ") malformed, found a token " << it_cell->second->GetVarName();
										cout << " while I must restore token " << it_tokens->second->GetVarName() << "!" << endl << endl;
										exit(SOMETHING_WRONG);
									}
									it_cell++;
									if (it_cell != cell.end()) {
										cout << endl << endl << "\t\tGrid::RestoreTokensTakenByAgent --> Agent " << AgentName;
										cout << " -- grid(" << i << "," << j << ") malformed, it contains ";
										cout << it_cell->second->GetVarName() << endl << endl;
										exit(SOMETHING_WRONG);
									}
									enter = true;
									//*cout << " -- ok" << endl; flush(cout);
									aGrid[i][j]->AddVariable(it_tokens->second->Clone()); // token restored
									it_originalCell = originalCell.end(); // così esce dal while interno su it_originalCell
								}
								else {
									cout << endl << endl << "\t\tGrid::RestoreTokensTakenByAgent called by " << AgentName <<  endl;
									cout << "\t\tEmpty cell... why?!?" << endl << endl;
								}
							}
						}
						else {
							// else: this cell of originalGrid doesn't contain a token, let's consider the next one
							it_originalCell++;
						}
					}
				}
			}
		}
		// consider next token taken by this agent
		it_tokens++;
	}
	if (!enter) {
		cout << endl << endl << "\t\tGrid::RestoreTokensTakenByAgent called by " << AgentName <<  endl;
		cout << "\t\tNessun token riposizionato: perché!?!?" << endl;
		cout << "\t\tQuesto agente ha i seguenti token da risposizionare:" << endl;
		it_tokens = tokens.begin();
		while (it_tokens != tokens.end()) {
			cout << "\t\t [*] " << it_tokens->second->GetVarName() << ", value = " << it_tokens->second->GetVarValue() << endl;
			it_tokens++;
		}
		exit(SOMETHING_WRONG);
	}
}

float Grid::EvaluateActions(string AgentName) {
	//*cout << "   Valutazione azione di " << AgentName << endl; flush(cout);
	float payoff;
	if (agentCounter == 0) { // this is the first call made by first agent
		BackupEnvironmentConfiguration(gainGrid);			// gainGrid <- grid;
		partialGlobalPayoff = GenericPayoff(grid);			// modifies "grid": <agent,token> become <ag>
		grandTotalPayoff+= partialGlobalPayoff;
		BackupEnvironmentConfiguration(copyGrid);			// copyGrid <- grid
	}
	else { // other calls of this function made by second, third, ..., Nth agent
		RestoreEnvironmentConfiguration(copyGrid);
	}
	agentCounter++;
	if (agentCounter == numAgents) {
		agentCounter = 0;
		isTheEndOfThisStep = true;
	}
	try {
		AssertConditionsOnGrid(grid);
	} catch (int ex) {
		cout << endl << "\t!!!!EXCEPTION: Grid::EvaluateActions (begin), after asserting condition on the original grid, ex = " << ex << endl << endl;
		exit(ex);
	}
	try {
		AssertConditionsOnGrid(gainGrid);
	} catch (int ex) {
		cout << endl << "\t!!!!EXCEPTION: Grid::EvaluateActions (begin), after asserting condition on gainGrid, ex = " << ex << endl << endl;
		exit(ex);
	}
	try {
		AssertConditionsOnGrid(copyGrid);
	} catch (int ex) {
		cout << endl << "\t!!!!EXCEPTION: Grid::EvaluateActions (begin), after asserting condition on copyGrid, ex = " << ex << endl << endl;
		exit(ex);
	}
	payoff = CoinPayoff(AgentName);
	//payoff = SimpleTokenPayoff(AgentName);
	/**if (payoff != 0.0) {
		cout << "      ~~~(COIN) Grid::EvaluateActions --> Agent \"" << AgentName << "\" has payoff = " << payoff << "~~~" << endl;
	}
	cout << "   -------------------------------" << endl; flush(cout);**/
	return payoff;
}

// Payoff obtained by all agents != AgentName
float Grid::GenericPayoff(string AgentName, VariablesInfo* aGrid[GRID_SIZE][GRID_SIZE]) {
	/*
	 * Compute the payoff obtained by all agents other than AgentName
	 */
	string agentName;
	float payoff = 0.0;
	unsigned i;
	unsigned j;
	for (i = 0; i < GRID_SIZE; i++) {
		for (j = 0; j < GRID_SIZE; j++) {
			if (aGrid[i][j] != NULL) {
				Sgi::hash_map<int, Variable*> cell;
				Sgi::hash_map<int, Variable*>::iterator it_cell;
				aGrid[i][j]->GetVariableList(cell);
				it_cell = cell.begin();
				while (it_cell != cell.end()) {
					agentName = it_cell->second->GetVarName();
					if (agentName != AgentName) {
						it_cell++; // now let's consider the token (if exists)
						if (it_cell != cell.end()) { // 2nd element (if exists) --> then is a cell with a token
							// agent took the token and the associated payoff
							payoff+= it_cell->second->GetVarValue();
							//*cout << "          >> Sono " << agentName << " (" << i << "," << j << ") e ho preso il token ";
							//*cout << it_cell->second->GetVarName() << " (" << i << "," << j << ")" << endl; flush(cout);
							//cout << ". Al momento G(not " << AgentName << ") = " << payoff << endl;
							/*
							 * NON aggiornare questa griglia (è gainGrid), visto che questa funzione serve per vedere quanto guadagnano gli
							 * agenti != AgentName --> ogni agente richiama questa funzione e calcola la somma dei token raccolti dagli altri
							 * agenti (cioè escludendo sé stesso)... --> trova le celle composte da <agent,token>
							 */
							it_cell++;
						}
						// else: this cell contains a token or an agent (without a token)
					}
					else {
						// agent AgentName found --> don't consider it!
						it_cell = cell.end(); // so it exits from while, that is passes to another cell
					}
				}
			}
			// else: empty cell, proceed to the next one
		}
	}
	return payoff;
}

// Payoff obtained by all agents = sum of the value of the tokens taken by all agents
float Grid::GenericPayoff(VariablesInfo* aGrid[GRID_SIZE][GRID_SIZE]) {
	Variable* temp = NULL;
	float payoff = 0.0;
	unsigned i;
	unsigned j;
	for (i = 0; i < GRID_SIZE; i++) {
		for (j = 0; j < GRID_SIZE; j++) {
			if (aGrid[i][j] != NULL) {
				Sgi::hash_map<int, Variable*> cell;
				Sgi::hash_map<int, Variable*>::iterator it_cell;
				aGrid[i][j]->GetVariableList(cell);
				it_cell = cell.begin();
				if (it_cell->second->GetVarName().find("Token_") == string::npos) { // cell with an agent
					temp = it_cell->second->Clone(); // agent
					it_cell++; // now let's consider the token if exists
					while (it_cell != cell.end()) {
						if (it_cell->second->GetVarName().find("Token_") == string::npos) {
							cout << endl << endl << "\t\tGrid::GenericPayoff(no agentName) --> malformed cell" << endl;
							cout << "\t Found in (" << i << "," << j << ") this variable: " << it_cell->second->GetVarName() << endl;
							exit(SOMETHING_WRONG);
						}
						payoff+= it_cell->second->GetVarValue();  // agent took the token and the associated payoff
						aGrid[i][j]->Clear();
						aGrid[i][j]->AddVariable(temp->Clone());  // update the cell with the agent's name (since the token was just taken before)
						delete temp;
						it_cell++;
					}
				}
				// else: cell with only a token --> don't consider it, pass to next one
			}
		}
	}
	BackupEnvironmentConfiguration(copyGrid);
	return payoff;
}

float Grid::CoinPayoff(string AgentName) {
	unsigned i;
	unsigned j;
	float payoff = 0.0;
	bool enter = false; // debugging variable
	for (i = 0; i < GRID_SIZE; i++) {
		for (j = 0; j < GRID_SIZE; j++) {
			if (grid[i][j] != NULL) {
				Sgi::hash_map<int, Variable*> cell;
				Sgi::hash_map<int, Variable*>::iterator it;
				grid[i][j]->GetVariableList(cell);
				it = cell.begin(); // 1st element of cell --> agent's name
				if (it->second == NULL) {
					cout << endl << endl << "\t****************** Grid::CoinPayoff --> in grid[" << i << "][" << j << "] there's a NULL variable!!!" << endl;
					exit(NO_MEM_AVAILABLE);
				}
				if (it->second->GetVarName() == AgentName) { // Agent found in grid
					enter = true;
					// 2. Remove this agent from grid
					RemoveAgent(AgentName,grid);
					try {
						AssertConditionsOnGrid(grid);
					} catch (int ex) {
						cout << endl << "\t!!!!EXCEPTION: CoinPayoff, after RemoveAgent(grid), ex = " << ex << endl << endl;
						exit(ex);
					}
					// 3. Restore tokens taken by agent AgentName and put them in "grid"
					RestoreTokensTakenByAgent(AgentName, grid);
					try {
						AssertConditionsOnGrid(grid);
					} catch (int ex) {
						cout << endl << "\t!!!!EXCEPTION: CoinPayoff, after RestoreTokensTakenByAgent(grid), ex = " << ex << endl << endl;
						exit(ex);
					}
					// 4. Compute G(not AgentName) --> use GenericPayoff
					othersPayoff = GenericPayoff(AgentName, gainGrid); // token raccolti da altri agenti dopo aver effettuato una mossa
					othersPayoff+= GenericPayoff(AgentName, grid); // token riposizionati poco prima e raccolti da altri agenti
					//cout << "     Other agents gained " << othersPayoff << endl; flush(cout);
					try {
						AssertConditionsOnGrid(gainGrid);
					} catch (int ex) {
						cout << endl << "\t!!!!EXCEPTION: CoinPayoff, after GenericPayoff(grid), ex = " << ex << endl << endl;
						exit(ex);
					}
					payoff = partialGlobalPayoff - othersPayoff; // WLU_agent = G_tot_di_questo_step - G(not AgentName)...
					//cout << " grid::CoinPayoff --> globalPayoff = " << partialGlobalPayoff << " -- othersPayoff = " << othersPayoff;
					//cout << " --> agent " << AgentName << " has gained " << payoff << endl << endl << endl; flush(cout);
					if (isTheEndOfThisStep) {
						/*
						 * In this timestep all agents have selected, executed and evaluate its actions.
						 * Now I'll restore the actual grid configuration (aka the actual state)
						 */
						isTheEndOfThisStep = false;
						RestoreEnvironmentConfiguration(copyGrid);
					}
					return payoff; // così esce dai cicli for...inutile andare avanti, ormai è già stato "elaborato" AgentName...
				}
				else {
					payoff = 0;
				}
			}
		}
	}
	if (!enter) {
		cout << AgentName << ": NON MI SON TROVATO!!!" << endl << endl;
		exit(SOMETHING_WRONG);
	}
	return payoff;
}

float Grid::SimpleTokenPayoff(string AgentName) {
	unsigned i;
	unsigned j;
	Variable* temp = NULL;
	string agent_name;
	float payoff = 0.0;
	for (i = 0; i < GRID_SIZE; i++) {
		for (j = 0; j < GRID_SIZE; j++) {
			if (grid[i][j] != NULL) {
				Sgi::hash_map<int, Variable*> cell;
				Sgi::hash_map<int, Variable*>::iterator it;
				grid[i][j]->GetVariableList(cell);
				it = cell.begin(); // 1st element of cell --> agent's name
				if (it->second == NULL) {
					cout << endl << endl << "\t****************** Grid::SimpleTokenPayoff --> in grid[" << i << "][" << j << "] there's a NULL variable!!!" << endl;
					exit(SOMETHING_WRONG);
				}
				if (it->second->GetVarName() == AgentName) {
					agent_name = it->second->GetVarName();
					it++; // now let's consider the token (if exists)
					if (it != cell.end()) { // 2nd element (if exists) --> then is a cell with a token
						// agent took the token and the associated payoff
						payoff = it->second->GetVarValue();
						grid[i][j]->Clear();
						// update the cell with the agent's name (since the token was just taken before)
						temp = new Variable(agent_name,0.0,0.0);
						if (temp == NULL) {
							cout << endl << endl << "\t\tGrid::SimpleTokenPayoff --> no memory available, now I will exit!" << endl << endl;
							exit(NO_MEM_AVAILABLE);
						}
						temp->SetVarValue(0.0);
						grid[i][j]->AddVariable(temp->Clone());
						delete temp;
					}
					else {
						payoff = 0;
					}
				}
				else {
					payoff = 0;
				}
			}
		}
	}
	return payoff;
}

float Grid::GetMaxReward() {
	return 1.0; // greatest token value
}

bool Grid::IsFeasibleAction(Variable* pAction, string AgentName) {
	//cout << "Grid::IsFeasibleAction --> Agent " << AgentName << ", action " << pAction->GetVarName() << endl;
	string actionName;
	string tokenName;
	float tokenValue;
	bool agentFound = false; // just for debugging purpose
	Variable* temp = NULL;
	unsigned i;
	unsigned j;
	for (i = 0; i < GRID_SIZE; i++) { // rows
		for (j = 0; j < GRID_SIZE; j++) { // columns
			if (grid[i][j] != NULL) {
				Sgi::hash_map<int, Variable*> cell;
				Sgi::hash_map<int, Variable*>::iterator it_cell;
				grid[i][j]->GetVariableList(cell);
				it_cell = cell.begin();
				if (it_cell == cell.end()) {
					cout << endl << endl << "\t\tGrid::IsFeasibleAction -->" << endl;
					cout << "\t\tMalformed cell --> it appeared as not empty, but in fact it doesn't contain any element!" << endl << endl;
					exit(SOMETHING_WRONG);
				}
				if (it_cell->second->GetVarName() == AgentName) { // it_cell punta all'agente
					//cout << "\tGrid::IsFeasibleAction --> Agent " << AgentName << " found in (" << i << "," << j << ")" << endl;
					agentFound = true;
					if (pAction == NULL) {
						cout << "\n\n************* Grid::IsFeasibleAction called by " << AgentName << ": pAction is NULL!!!" << endl;
						exit(NO_MEM_AVAILABLE);
					}
					actionName = pAction->GetVarName().substr(pAction->GetVarName().find("_")+1);
					if (actionName == "U") {	// Up (agent moves itself up of one row --> i-1)
						if (i > 0) {
							//cout << "Agente " << AgentName << ": azione " << actionName << " fattibile" << endl;
							if (grid[i-1][j] == NULL) {  // empty cell --> feasible action
								grid[i-1][j] = grid[i][j]->Clone(); // move the agent to new location
								delete grid[i][j]; // clear previous location
								grid[i][j] = NULL;
								try {
									AssertConditionsOnGrid(grid);
								} catch (int ex) {
									cout << endl << "\t!!!!EXCEPTION: IsFeasibleAction, after asserting condition on the original grid, ex = " << ex << endl << endl;
									exit(ex);
								}
								//cout << "all conditions are satisfied" << endl;
								return true;
							}
							else {
								Sgi::hash_map<int, Variable*> next_cell;
								Sgi::hash_map<int, Variable*>::iterator it_next_cell;
								grid[i-1][j]->GetVariableList(next_cell);
								it_next_cell = next_cell.begin();
								tokenName = it_next_cell->second->GetVarName();
								if (tokenName.find("Token_") != string::npos) {  // cell ONLY with a token
									tokenValue = it_next_cell->second->GetVarValue();
									cout << "  [*] Agent \"" << AgentName << "\" (" << i << "," << j << "),\taction " << actionName;
									cout << "\t--> token \"" << tokenName << "\" found in (" << i-1 << "," << j << ") with value " << tokenValue << endl;
									grid[i-1][j]->AddVariableInFront(it_cell->second);
									delete grid[i][j];
									grid[i][j] = NULL;
									temp = new Variable(tokenName,0.0,0.0);
									if (temp == NULL) {
										cout << endl << endl << "\t\tGrid::IsFeasibleAction --> no memory available, now I will exit!" << endl << endl;
										exit(NO_MEM_AVAILABLE);
									}
									temp->SetVarValue(tokenValue);
									AddDependence(AgentName,temp);
									delete temp;
									//PrintDependencies();
									try {
										AssertConditionsOnGrid(grid);
									} catch (int ex) {
										cout << endl << "\t!!!!EXCEPTION: IsFeasibleAction, after asserting condition on the original grid, ex = " << ex << endl << endl;
										exit(ex);
									}
									//cout << "all conditions are satisfied" << endl;
									return true;
								}
								// else this cell is yet occupied by another agent (+eventually the token it will take));
								return false;
							}
						}
						// not possible action
						return false;
					}
					if (actionName == "D") {	// Down (agent moves itself down of one row --> i+1)
						if (i < GRID_SIZE - 1) {
							//cout << "Agente " << AgentName << ": azione " << actionName << " fattibile" << endl;
							if (grid[i+1][j] == NULL) {
								grid[i+1][j] = grid[i][j]->Clone();
								delete grid[i][j];
								grid[i][j] = NULL;
								try {
									AssertConditionsOnGrid(grid);
								} catch (int ex) {
									cout << endl << "\t!!!!EXCEPTION: IsFeasibleAction, after asserting condition on the original grid, ex = " << ex << endl << endl;
									exit(ex);
								}
								//cout << "all conditions are satisfied" << endl;
								return true;
							}
							else {
								Sgi::hash_map<int, Variable*> next_cell;
								Sgi::hash_map<int, Variable*>::iterator it_next_cell;
								grid[i+1][j]->GetVariableList(next_cell);
								it_next_cell = next_cell.begin();
								tokenName = it_next_cell->second->GetVarName();
								if (tokenName.find("Token_") != string::npos) {  // cell ONLY with a token
									tokenValue = it_next_cell->second->GetVarValue();
									cout << "  [*] Agent \"" << AgentName << "\" (" << i << "," << j << "),\taction " << actionName;
									cout << "\t--> token \"" << tokenName << "\" found in (" << i+1 << "," << j << ") with value " << tokenValue << endl;
									grid[i+1][j]->AddVariableInFront(it_cell->second);
									delete grid[i][j];
									grid[i][j] = NULL;
									temp = new Variable(tokenName,0.0,0.0);
									if (temp == NULL) {
										cout << endl << endl << "\t\tGrid::IsFeasibleAction --> no memory available, now I will exit!" << endl << endl;
										exit(NO_MEM_AVAILABLE);
									}
									temp->SetVarValue(tokenValue);
									AddDependence(AgentName,temp);
									delete temp;
									//PrintDependencies();
									try {
										AssertConditionsOnGrid(grid);
									} catch (int ex) {
										cout << endl << "\t!!!!EXCEPTION: IsFeasibleAction, after asserting condition on the original grid, ex = " << ex << endl << endl;
										exit(ex);
									}
									//cout << "all conditions are satisfied" << endl;
									return true;
								}
								// else this cell is yet occupied by another agent (+eventually the token it will take)
								return false;
							}
						}
						// not possible action
						return false;
					}
					if (actionName == "L") {		// Left (agent moves itself left of one column --> j-1)
						if (j > 0) {
							//cout << "Agente " << AgentName << ": azione " << actionName << " fattibile" << endl;
							if (grid[i][j-1] == NULL) {
								grid[i][j-1] = grid[i][j]->Clone();
								delete grid[i][j];
								grid[i][j] = NULL;
								try {
									AssertConditionsOnGrid(grid);
								} catch (int ex) {
									cout << endl << "\t!!!!EXCEPTION: IsFeasibleAction, after asserting condition on the original grid, ex = " << ex << endl << endl;
									exit(ex);
								}
								//cout << "all conditions are satisfied" << endl;
								return true;
							}
							else {
								Sgi::hash_map<int, Variable*> next_cell;
								Sgi::hash_map<int, Variable*>::iterator it_next_cell;
								grid[i][j-1]->GetVariableList(next_cell);
								it_next_cell = next_cell.begin();
								tokenName = it_next_cell->second->GetVarName();
								if (tokenName.find("Token_") != string::npos) {  // cell ONLY with a token
									tokenValue = it_next_cell->second->GetVarValue();
									cout << "  [*] Agent \"" << AgentName << "\" (" << i << "," << j << "),\taction " << actionName;
									cout << "\t--> token \"" << tokenName << "\" found in (" << i << "," << j-1 << ") with value " << tokenValue << endl;
									grid[i][j-1]->AddVariableInFront(it_cell->second);
									delete grid[i][j];
									grid[i][j] = NULL;
									temp = new Variable(tokenName,0.0,0.0);
									if (temp == NULL) {
										cout << endl << endl << "\t\tGrid::IsFeasibleAction --> no memory available, now I will exit!" << endl << endl;
										exit(NO_MEM_AVAILABLE);
									}
									temp->SetVarValue(tokenValue);
									AddDependence(AgentName,temp);
									delete temp;
									//PrintDependencies();
									try {
										AssertConditionsOnGrid(grid);
									} catch (int ex) {
										cout << endl << "\t!!!!EXCEPTION: IsFeasibleAction, after asserting condition on the original grid, ex = " << ex << endl << endl;
										exit(ex);
									}
									//cout << "all conditions are satisfied" << endl;
									return true;
								}
								// else this cell is yet occupied by another agent (+eventually the token it will take)
								return false;
							}
						}
						// not possible action
						return false;
					}
					if (actionName == "R") {	// Right (agent moves itself right of one column --> j+1)
						if (j < GRID_SIZE - 1) {
							//cout << "Agente " << AgentName << ": azione " << actionName << " fattibile" << endl;
							if (grid[i][j+1] == NULL) {
								grid[i][j+1] = grid[i][j]->Clone();
								delete grid[i][j];
								grid[i][j] = NULL;
								try {
									AssertConditionsOnGrid(grid);
								} catch (int ex) {
									cout << endl << "\t!!!!EXCEPTION: IsFeasibleAction, after asserting condition on the original grid, ex = " << ex << endl << endl;
									exit(ex);
								}
								//cout << "all conditions are satisfied" << endl;
								return true;
							}
							else {
								Sgi::hash_map<int, Variable*> next_cell;
								Sgi::hash_map<int, Variable*>::iterator it_next_cell;
								grid[i][j+1]->GetVariableList(next_cell);
								it_next_cell = next_cell.begin();
								tokenName = it_next_cell->second->GetVarName();
								if (tokenName.find("Token_") != string::npos) {  // cell ONLY with a token
									tokenValue = it_next_cell->second->GetVarValue();
									cout << "  [*] Agent \"" << AgentName << "\" (" << i << "," << j << "),\taction " << actionName;
									cout << "\t--> token \"" << tokenName << "\" found in (" << i << "," << j+1 << ") with value " << tokenValue << endl;
									grid[i][j+1]->AddVariableInFront(it_cell->second);
									delete grid[i][j];
									grid[i][j] = NULL;
									temp = new Variable(tokenName,0.0,0.0);
									if (temp == NULL) {
										cout << endl << endl << "\t\tGrid::IsFeasibleAction --> no memory available, now I will exit!" << endl << endl;
										exit(NO_MEM_AVAILABLE);
									}
									temp->SetVarValue(tokenValue);
									AddDependence(AgentName,temp);
									delete temp;
									//PrintDependencies();
									try {
										AssertConditionsOnGrid(grid);
									} catch (int ex) {
										cout << endl << "\t!!!!EXCEPTION: IsFeasibleAction, after asserting condition on the original grid, ex = " << ex << endl << endl;
										exit(ex);
									}
									//cout << "all conditions are satisfied" << endl;
									return true;
								}
								// else this cell is yet occupied by another agent (+eventually the token it will take)
								return false;
							}
						}
						// not possible action
						return false;
					}
				}
				else {
					//cout << "\tGrid::IsFeasibleAction --> la cella (" << i << "," << j << ") contiene " << it_cell->second->GetVarName() << endl;
					agentFound = false; // this agent wasn't yet found
				}
			}
			// else: empty cell, do not consider it and pass to the next one
			agentFound = false;
		}
	}
	cout << "\n\n\n\n\t\t\t************ Grid::IsFeasibleAction --> Agent " << AgentName << "; perché è qui?!" << endl; flush(cout);
	cout << "\t\tAgent was"; flush(cout);
	if (!agentFound) {
		cout << " not"; flush(cout);
	}
	cout << " found" << endl; flush(cout);
	exit(SOMETHING_WRONG); // A little drastic decision (elsewhere we can return false...), but it must NEVER reach this point!!!!
}

/**
 * Conditions must be true:
 *  1. a cell may be empty, or
 *  2. a cell may contain one and only one token, or
 *  3. a cell may containt one and only one agent, or
 *  4. a cell may contain one and only one agent "followed" by one and only one token: <agent,token>
 */
void Grid::AssertConditionsOnGrid(VariablesInfo*const aGrid[GRID_SIZE][GRID_SIZE]) {
	unsigned counter = 0;
	for (unsigned i = 0; i < GRID_SIZE; i++) {
		for (unsigned j = 0; j < GRID_SIZE; j++) {
			if (aGrid[i][j] != NULL) {
				Sgi::hash_map<int, Variable*> cell;
				Sgi::hash_map<int, Variable*>::iterator it;
				aGrid[i][j]->GetVariableList(cell);
				it = cell.begin(); // "it" points at 1st element of this cell (must be an agent XOR a token)
				while (it != cell.end()) {
					counter++;
					it++;
				}
				if (counter > 2) {
					// violation of conditions 2, 3 and 4
					cout << endl << endl << "\a\t\tGrid::AssertConditions --> condition failed in (" << i << "," << j << ") --> more than 2 elements in this cell!!!" << endl;
					cout << "\t\tThat elements are:" << endl;
					it = cell.begin();
					while (it != cell.end()) {
						cout << "\t\t\t[*] Name = \"" << it->second->GetVarName() << "\", value = " << it->second->GetVarValue() << endl;
						it++;
					}
					cout << endl;
					throw SOMETHING_WRONG;
				}
				else {
					if (counter == 2) { // Check if this cell contains an agent followed by a token
						it = cell.begin();
						if (it->second->GetVarName().find("Token_") != string::npos) {
							// Violation of condition 4 --> first element is not an agent, but a token
							cout << endl << endl << "\a\t\tGrid::AssertConditions --> condition failed in (" << i << "," << j << ") --> malformed cell!!!" << endl;
							cout << "\t\tThis cell contains a token (" << it->second->GetVarName() << ", value = " << it->second->GetVarValue() << ")";
							cout << " followed by ";
							it++;
							cout << it->second->GetVarName() << ", value = " << it->second->GetVarValue() << endl << endl;
							throw SOMETHING_WRONG;
						}
						else {
							it++;
							if (it->second->GetVarName().find("Token_") == string::npos) {
								// Violation of condition 4 --> first element wasn't a token (right), but as well neither the second (bad)
								cout << endl << endl << "\a\t\tGrid::AssertConditions --> condition failed in (" << i << "," << j << ") --> malformed cell!!!" << endl;
								cout << "\t\tThis cell contanis:" << endl;
								it = cell.begin();
								cout << "\t\t  [*] " << it->second->GetVarName() << " (value = " << it->second->GetVarValue() << ")" << endl;
								it++;
								cout << "\t\t  [*] " << it->second->GetVarName() << " (value = " << it->second->GetVarValue() << ")" << endl;
								throw SOMETHING_WRONG;
							}
						}
					}
				}
				counter = 0;
			}
			// else this cell is empty --> condition 1 is ok
		}
	}
}

VariablesInfo* Grid::GetStateDescription() {
	UpdateStateDescription();
	return mpStateInfo->Clone();
}

VariablesInfo* Grid::GetPartialStateDescription() {
	UpdatePartialStateDescription();
	return mpStateInfo->Clone();
}

void Grid::UpdateStateDescription() {
	if (mpStateInfo != NULL) {
		mpStateInfo->Clear();
		delete mpStateInfo;
	}
	mpStateInfo = new VariablesInfo();
	if (mpStateInfo == NULL) {
		cout << endl << endl << "\t\tGrid::UpdateStateDescription(918) --> no memory available for mpStateInfo, now I will exit!" << endl << endl;
		exit(NO_MEM_AVAILABLE);
	}
	Variable* temp;
	char counter = 1;
	unsigned row;
	unsigned column;
	for (row = 0; row < GRID_SIZE; row++) { // rows
		for (column = 0; column < GRID_SIZE; column++) { // columns
			if (grid[row][column] != NULL) {
				Sgi::hash_map<int, Variable*> cell;
				Sgi::hash_map<int, Variable*>::iterator it;
				grid[row][column]->GetVariableList(cell);
				it = cell.begin();
				if (it == cell.end()) {
					cout << endl << endl << "\t\tGrid::UpdateStateDescription --> Malformed cell, it's empty!" << endl << endl;
					exit(SOMETHING_WRONG);
				}
				while (it != cell.end()) {
					/*
					 * Possibili casi:
					 *  + cella con un token
					 *  + cella con un agente
					 *  + cella con un agente ed un token: <agent,token>
					 */
					if (it->second->GetVarName().find("Token_") != string::npos) { // there's a token --> enumerate it
						// come valore della variabile token metto il valore effettivo del token
						//temp = new Variable(it->second->GetVarName().append("_").append(1,counter+48).append("_r"),0.0,0.0);
						temp = new Variable(it->second->GetVarName().append("_r"),0.0,0.0);
						if (temp == NULL) {
							cout << endl << endl << "\t\tGrid::UpdateStateDescription(934) --> no memory available for token, now I will exit!" << endl << endl;
							exit(NO_MEM_AVAILABLE);
						}
						/*cout << "Grid::UpdateStateDescription --> token found in (" << r << "," << c << ")";
						cout << " with value  = " << it->second->GetVarValue() << endl; flush(cout);*/
						temp->SetVarValue(it->second->GetVarValue());
						mpStateInfo->AddVariable(temp->Clone());
						delete temp;
						//temp = new Variable(it->second->GetVarName().append("_").append(1,counter+48).append("_c"),0.0,0.0);
						temp = new Variable(it->second->GetVarName().append("_c"),0.0,0.0);
						if (temp == NULL) {
							cout << endl << endl << "\t\tGrid::UpdateStateDescription(934) --> no memory available for token, now I will exit!" << endl << endl;
							exit(NO_MEM_AVAILABLE);
						}
						temp->SetVarValue(it->second->GetVarValue());
						mpStateInfo->AddVariable(temp->Clone());
						delete temp;
						it++;
						counter++;
					}
					else { // there's an agent optionally followed by a token (this one is considered at the next step of "while"...)
						// come valore della var che rappresenta l'agente metto le sue coordinate (r,c)
						temp = new Variable(it->second->GetVarName().append("_r"),0.0,0.0);
						if (temp == NULL) {
							cout << endl << endl << "\t\tGrid::UpdateStateDescription(940) --> no memory available for agent, now I will exit!" << endl << endl;
							exit(NO_MEM_AVAILABLE);
						}
						/*cout << "Grid::UpdateStateDescription --> Agent " << it->second->GetVarName();
						cout << " found in (" << r << "," << c << ")" << endl; flush(cout);*/
						temp->SetVarValue(row);
						mpStateInfo->AddVariable(temp->Clone());
						delete temp;
						temp = new Variable(it->second->GetVarName().append("_c"),0.0,0.0);
						if (temp == NULL) {
							cout << endl << endl << "\t\tGrid::UpdateStateDescription(934) --> no memory available for token, now I will exit!" << endl << endl;
							exit(NO_MEM_AVAILABLE);
						}
						temp->SetVarValue(column);
						mpStateInfo->AddVariable(temp->Clone());
						delete temp;
						it++;
					}
				}
			}
		}
	}
}

// ritorna la dimensione della griglia e la posizione degli agenti
void Grid::UpdatePartialStateDescription() {
	// rows --> y coordinate (variable j)
	// columns --> x coordinate (variable i)
	if (mpStateInfo != NULL) {
		mpStateInfo->Clear();
		delete mpStateInfo;
	}
	mpStateInfo = new VariablesInfo();
	if (mpStateInfo == NULL) {
		cout << endl << endl << "\t\tGrid::UpdatePartialStateDescription(978) --> no memory available for mpStateInfo, now I will exit!" << endl << endl;
		exit(NO_MEM_AVAILABLE);
	}
	Variable* temp;
	temp = new Variable("CELLS",0.0,0.0);
	if (temp == NULL) {
		cout << endl << endl << "\t\tGrid::UpdatePartialStateDescription(978) --> no memory available for temp, now I will exit!" << endl << endl;
		exit(NO_MEM_AVAILABLE);
	}
	temp->SetVarValue(GRID_SIZE*GRID_SIZE);
	mpStateInfo->AddVariable(temp->Clone());
	delete temp;
	unsigned r;
	unsigned c;
	for (r = 0; r < GRID_SIZE; r++) { // rows
		for (c = 0; c < GRID_SIZE; c++) { // columns
			if (grid[r][c] != NULL) {
				Sgi::hash_map<int, Variable*> cell;
				Sgi::hash_map<int, Variable*>::iterator it;
				grid[r][c]->GetVariableList(cell);
				it = cell.begin();
				while (it != cell.end()) {
					if (it->second->GetVarName().find("Token_") == string::npos) { // cell with an agent (optionally followed by a token)
						temp = new Variable(it->second->GetVarName(),0.0,0.0);
						if (temp == NULL) {
							cout << endl << endl << "\t\tGrid::UpdatePartialStateDescription(1011) --> no memory available for agent, now I will exit!" << endl << endl;
							exit(NO_MEM_AVAILABLE);
						}
						temp->SetVarValue(((r*GRID_SIZE)-1)+c+1);
						mpStateInfo->AddVariable(temp->Clone());
						delete temp;
					}
					// else there's a cell with a token or we're considering now a token in the same cell of an agent
					it++;
				}
			}
		}
	}
}

string Grid::ToString() {
	ostringstream ret(ostringstream::out);
	UpdateStateDescription();
	Sgi::hash_map<int, Variable*> states;
	Sgi::hash_map<int, Variable*>::iterator it;
	mpStateInfo->GetVariableList(states);
	it = states.begin();
	while (it != states.end()) {
		ret << it->second->GetVarName() << endl;
		ret << it->second->GetVarValue() << endl;
		it++;
	}
	return ret.str();
}

const TransitionModel*const  Grid::GetTransitionModel ()
{
  //cout << "Environment TansitionModel: " << &transitionModel << endl;
  return &transitionModel;
}

void  Grid::GenerateTransitionModel ( )
{
  //unsigned int stateSize = mNumRows*mNumCols;
  vector <unsigned> state(4,0);
  // vector <unsigned> actions;
  unsigned s1,s2;
  
  //cycle through the joint action space: |S|=81
  for (unsigned w = 0; w<GRID_SIZE; w++)//Row_1
    {
      for (unsigned x = 0; x<GRID_SIZE; x++)//Row_0
	{
	  for (unsigned y = 0; y<GRID_SIZE; y++)//Col_1
	    {
	      for (unsigned z = 0; z<GRID_SIZE; z++)//Col_0
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

unsigned int  Grid::ComputeJointIndex  (vector <unsigned> vec, unsigned int size)
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

vector <unsigned>  Grid::AvailJointMoves( vector <unsigned int> state)
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