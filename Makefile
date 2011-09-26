#OBJECTS = Variable.o VariablesInfo.o Environment.o Bar.o Grid.o LearningAgent.o CoLFAgent.o CKCoLFAgent.o main.o
OBJECTS = Variable.o VariablesInfo.o Environment.o Bar.o  LearningAgent.o VI.o CE_VI.o CoLFAgent.o TransitionModel.o GridWorld.o main.o

OBJECTS2 = Variable.o VariablesInfo.o Environment.o Bar.o VI.o CE_VI.o TransitionModel.o GridWorld.o GridWorldWall.o main.o
# Include subdirectory
INCLUDE = include
#CPU type
CPU = pentium4m
# Optimization ratio
OPTIMIZATION = 1
# Add more information for gdb
DEBUG = -ggdb
VERBOSE = #-DVERBOSE

#Agent types:
#	C: CoLF agent
#	K: CKCoLF agent
AGENT_TYPE = C
NUM_AGENTS = 2
STEPS = 100

all : compile link

vi: compile2 link2

prova : prova-random.cpp
	g++ -o prova prova-random.cpp
	./prova

run :
	gdb --args main -a 5 -t $(NUM_AGENTS) -m $(AGENT_TYPE) -s .4 -n .2 -d .1 -x .5 -r .1 -g .5 -l .1 -p $(STEPS) -e G

compile : $(OBJECTS)

compile2 : $(OBJECTS2)

link :
	g++ -march=$(CPU) -Wall $(DEBUG) $(VERBOSE) -O$(OPTIMIZATION) -o main $(OBJECTS)
	
link2 :
	g++ -march=$(CPU) -Wall $(DEBUG) $(VERBOSE) -O$(OPTIMIZATION) -o main $(OBJECTS2) /usr/local/lib/libglpk.a

Variable.o : variable/Variable.cpp include/Variable.h
	g++ -march=$(CPU) -Wall $(DEBUG) $(VERBOSE) -O$(OPTIMIZATION) -I$(INCLUDE) -c variable/Variable.cpp

VariablesInfo.o : variable/VariablesInfo.cpp include/Variable.h include/VariablesInfo.h variable/Variable.cpp
	g++ -march=$(CPU) -Wall $(DEBUG) $(VERBOSE) -O$(OPTIMIZATION) -I$(INCLUDE) -c variable/VariablesInfo.cpp

Environment.o : Environment.cpp include/VariablesInfo.h include/Variable.h variable/Variable.cpp\
	variable/VariablesInfo.cpp include/Environment.h include/constants.h
	g++ -march=$(CPU) -Wall $(DEBUG) $(VERBOSE) -O$(OPTIMIZATION) -I$(INCLUDE) -c Environment.cpp

Bar.o : environments/Bar.cpp environments/Bar.h include/Environment.h Environment.cpp include/VariablesInfo.h\
	include/Variable.h variable/Variable.cpp variable/VariablesInfo.cpp  include/constants.h
	g++ -march=$(CPU) -Wall $(DEBUG) $(VERBOSE) -O$(OPTIMIZATION) -I$(INCLUDE) -c environments/Bar.cpp

#Grid.o : environments/Grid.cpp environments/Grid.h include/Environment.h Environment.cpp include/VariablesInfo.h\
#	include/Variable.h variable/Variable.cpp variable/VariablesInfo.cpp  include/constants.h
#	g++ -march=$(CPU) -Wall $(DEBUG) $(VERBOSE) -O$(OPTIMIZATION) -I$(INCLUDE) -c environments/Grid.cpp

VI.o : agents/VI.cpp include/VI.h include/Variable.h include/VariablesInfo.h\
	variable/Variable.cpp variable/VariablesInfo.cpp
	g++ -march=$(CPU) -Wall $(DEBUG) $(VERBOSE) -O$(OPTIMIZATION) -I$(INCLUDE) -c agents/VI.cpp

LearningAgent.o : agents/LearningAgent.cpp include/LearningAgent.h include/Variable.h include/VariablesInfo.h\
	variable/Variable.cpp variable/VariablesInfo.cpp
	g++ -march=$(CPU) -Wall $(DEBUG) $(VERBOSE) -O$(OPTIMIZATION) -I$(INCLUDE) -c agents/LearningAgent.cpp

CE_VI.o : agents/CE_VI.cpp include/CE_VI.h agents/LearningAgent.cpp include/Variable.h include/VariablesInfo.h\
	variable/Variable.cpp variable/VariablesInfo.cpp
	g++ -march=$(CPU) -Wall $(DEBUG) $(VERBOSE) -O$(OPTIMIZATION) -I$(INCLUDE) -c agents/CE_VI.cpp

CoLFAgent.o : agents/CoLFAgent.cpp include/CoLFAgent.h agents/LearningAgent.cpp include/Variable.h include/VariablesInfo.h\
	variable/Variable.cpp variable/VariablesInfo.cpp
	g++ -march=$(CPU) -Wall $(DEBUG) $(VERBOSE) -O$(OPTIMIZATION) -I$(INCLUDE) -c agents/CoLFAgent.cpp

CKCoLFAgent.o : agents/CKCoLFAgent.cpp include/CKCoLFAgent.h agents/LearningAgent.cpp include/Variable.h include/VariablesInfo.h\
	variable/Variable.cpp variable/VariablesInfo.cpp
	g++ -march=$(CPU) -Wall $(DEBUG) $(VERBOSE) -O$(OPTIMIZATION) -I$(INCLUDE) -c agents/CKCoLFAgent.cpp
	
TransitionModel.o : models/TransitionModel.cpp models/TransitionModel.h include/Variable.h include/VariablesInfo.h\
	variable/Variable.cpp variable/VariablesInfo.cpp
	g++ -march=$(CPU) -Wall $(DEBUG) $(VERBOSE) -O$(OPTIMIZATION) -I$(INCLUDE) -c models/TransitionModel.cpp
	
#GridWorldMarkovGames.o : environments/TransitionModel.cpp environments/TransitionModel.h include/Variable.h include/VariablesInfo.h\
#	variable/Variable.cpp variable/VariablesInfo.cpp
#	g++ -march=$(CPU) -Wall $(DEBUG) $(VERBOSE) -O$(OPTIMIZATION) -I$(INCLUDE) -c models/TransitionModel.cpp

GridWorld.o : environments/GridWorld.cpp environments/GridWorld.h include/Environment.h\
	Environment.cpp
	g++ -march=$(CPU) -Wall $(DEBUG) $(VERBOSE) -O$(OPTIMIZATION) -I$(INCLUDE) -c environments/GridWorld.cpp

GridWorldWall.o : environments/GridWorldWall.cpp environments/GridWorldWall.h environments/GridWorld.h\
	environments/GridWorld.cpp
	g++ -march=$(CPU) -Wall $(DEBUG) $(VERBOSE) -O$(OPTIMIZATION) -I$(INCLUDE) -c environments/GridWorldWall.cpp

main.o : main.cpp include/VariablesInfo.h include/Variable.h environments/Bar.h include/Environment.h\
	variable/VariablesInfo.cpp variable/Variable.cpp environments/Bar.cpp Environment.cpp include/constants.h
	g++ -march=$(CPU) -Wall $(DEBUG) $(VERBOSE) -O$(OPTIMIZATION) -I$(INCLUDE) -c main.cpp

clean :
	rm *.o main
