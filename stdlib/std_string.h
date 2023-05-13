#pragma once
#include "stddeps.h"
import utils;

void string_substr()
{
	//Get variables
	string firstArg = getArrayIndex(getVariable(PARAMETER_VARIABLE), 0);
	string secondArg = getArrayIndex(getVariable(PARAMETER_VARIABLE), 1);
	string thirdArg = getArrayIndex(getVariable(PARAMETER_VARIABLE), 2);

	if (secondArg[0] != VARNAME && secondArg[0] != REGISTER && secondArg[0] != NUM64)
	{
		fatalError(FORMAT_INVALID_PARAM_TYPE);
	}
	else if (secondArg[0] == REGISTER)
	{
		secondArg = registers[secondArg[1]];
	}
	if (secondArg[0] == VARNAME)
	{
		secondArg = getVariable(stringToUI64(secondArg.substr(1)));
	}
	if (secondArg[0] != NUM64)
	{
		fatalError(RUNTIME_INVALID_PARAM_TYPE);
	}

	longer indexStart = stringToUI64(thirdArg.substr(1));

	if (thirdArg[0] != VARNAME && thirdArg[0] != REGISTER && thirdArg[0] != NUM64)
	{
		fatalError(FORMAT_INVALID_PARAM_TYPE);
	}
	else if (thirdArg[0] == REGISTER)
	{
		thirdArg = registers[thirdArg[1]];
	}
	if (thirdArg[0] == VARNAME)
	{
		thirdArg = getVariable(stringToUI64(thirdArg.substr(1)));
	}
	if (thirdArg[0] != NUM64)
	{
		fatalError(RUNTIME_INVALID_PARAM_TYPE);
	}

	longer indexEnd = stringToUI64(thirdArg.substr(1));

	string strToBeChanged;
	if (firstArg[0] != VARNAME && firstArg[0] != REGISTER)
	{
		fatalError(FORMAT_INVALID_PARAM_TYPE);
	}
	else if (firstArg[0] == REGISTER)
	{
		strToBeChanged = registers[firstArg[1]];
	}
	if (firstArg[0] == VARNAME)
	{
		strToBeChanged = getVariable(stringToUI64(firstArg.substr(1)));
	}
		
	ulonger strLen = getPrimitiveLength(strToBeChanged);

	strToBeChanged = convertToProperStr(strToBeChanged);
	if (strToBeChanged == "")
	{
		registers[RETURN_REGISTER] = convertToSaveableStr(strToBeChanged);
	}
	else
	{
		registers[RETURN_REGISTER] = convertToSaveableStr(strToBeChanged.substr(indexStart, indexEnd));
	}
}

void string_indexOf() 
{
	string firstArg = getArrayIndex(getVariable(PARAMETER_VARIABLE), 0); // string to be searched through
	string secondArg = getArrayIndex(getVariable(PARAMETER_VARIABLE), 1); // string to be searched for

	if (getPrimitiveLength(getVariable(PARAMETER_VARIABLE)) == 3) {
		
	}

	firstArg = retrieveValue(firstArg);
	if (firstArg[0] != STRING)
		fatalError(RUNTIME_INVALID_PARAM_TYPE);
	secondArg = retrieveValue(secondArg);
	if (secondArg[0] != STRING && secondArg[0] != CHARA)
		fatalError(RUNTIME_INVALID_PARAM_TYPE);

	longer index = firstArg.substr(1).find(secondArg.substr(1));

	registers[RETURN_REGISTER] = (char)NUM64 + UI64ToString(index);
}

void string_toString()
{
	string firstArg = getArrayIndex(getVariable(PARAMETER_VARIABLE), 0);

	registers[RETURN_REGISTER] = convertToSaveableStr(convertToPrintableStr(firstArg));
}