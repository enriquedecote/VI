// SYSTEM INCLUDES
#include <sstream>
#include <iostream>

// PROJECT INCLUDES

// LOCAL INCLUDES
#include "Environment.h"

Environment::Environment ( )
{
    mpStateInfo = NULL;
}

Environment::~Environment ( )
{
    delete mpStateInfo;
}


void  Environment::Init (VariablesInfo* pActions)
{
   
}

VariablesInfo* Environment::GetStateDescription ( )
{
    return mpStateInfo;
}

string  Environment::ToString (unsigned int n) const
{
#if TOSTRING
    // cout << "Environment::ToString(" << n << ")" << endl;
    return mpStateInfo->ToString(n);
#else

    return "";
#endif
}

string  Environment::ToShortString (unsigned int n) const
{
#if TOSTRING
    // cout << "Environment::ToShortString(" << n << ")" << endl;
    return mpStateInfo->ToShortString(n);
#else

    return "";
#endif
}
