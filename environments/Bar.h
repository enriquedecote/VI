#ifndef BAR_H
#define BAR_H

#include "../include/Environment.h"
#include "../include/Variable.h"
#include "../include/VariablesInfo.h"

class Bar : public Environment {

	public:

		Bar();

		~Bar();

		void CustomInit();

		void Reset();

		void Init(VariablesInfo* pActions);

		void AppendToInit(VariablesInfo* pActions);

		float GetMaxReward();

		float EvaluateActions(string AgentName);

		bool IsFeasibleAction(Variable* pAction, string AgentName);

		VariablesInfo* GetStateDescription();

		VariablesInfo* GetPartialStateDescription();

		string ToString();

};

#endif // BAR_H
