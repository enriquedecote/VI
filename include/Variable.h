#ifndef VARIABLE_H
#define VARIABLE_H

#include <string>
#include <vector>
#include <mystl.h>

using namespace std;

class Variable {

	public:

		Variable();

		Variable(string varName, float minValue, float maxValue);

		virtual ~Variable() { }

		virtual string GetVarName()const;

		virtual void SetVarName(string varName);

		virtual float GetMinValue() const;

		virtual void SetMinValue(float minValue);

		virtual float GetMaxValue() const;

		virtual void SetMaxValue(float maxValue);

		virtual float GetVarValue() const;

		virtual bool SetVarValue(float varValue);

		virtual void Equal(const Variable*const pVar);

		virtual Variable* Clone();
		
		/**
		 * Return true iff this and pVar are two equals variables, i.e.:
		 *  * this.minValue == pVar->minValue
		 *  * this.maxValue == pVar->maxValue
		 * ... and so on
		 */
		virtual bool Equals(Variable* pVar);

		string mVarName;
		
	private:
	
		float mMinValue;
	
		float mMaxValue;
	
		float mVarValue;

};

#endif // VARIABLE_H
