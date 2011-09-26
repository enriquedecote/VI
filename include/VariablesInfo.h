#ifndef VARIABLESINFO_H
#define VARIABLESINFO_H

#include <string>
#include <ext/hash_map>
#include <vector>
#include "Variable.h"

class VariablesInfo {

	public:

		VariablesInfo()  { }

		virtual ~VariablesInfo();

		virtual bool GetVarValue(string varName, float& rValue);

		virtual bool SetVarValue(string varName, float varValue);

		/**
		 * Add a variable at the end of this set
		 */
		virtual void AddVariable(Variable* var);

		/**
		 * Add a variable at the begin of this set
		 */
		virtual void AddVariableInFront(Variable* var);

		virtual const Variable*const GetVariable(string varName);
		
		    /**
    	 * Get variable.
    	 * NOTE: return a reference to the variable passed as argument
   	  	 * @param pos  the ordinal position of the variable
    	 * @return the reference to a copy of the variable, NULL if no variable
   	  *         is found, the returned variable is owned by the VariablesInfo
     */
    virtual const Variable*const GetVariable (const unsigned int pos);

		virtual void GetVariableList(Sgi::hash_map<int, Variable*>& variables);

		virtual unsigned int size( );

		virtual void Equal(VariablesInfo* pVar);

		virtual VariablesInfo* Clone( );

		virtual void Clear();

		virtual Variable* End();

		/**
		 * Find a variable in this set.
		 * If pVar is NULL and/or pVar is not in this set, Find return NULL
		 */
		virtual Variable* Find(Variable* pVar);

	private:

		vector<Variable*> mVariables;

};
#endif // VARIABLESINFO_H
