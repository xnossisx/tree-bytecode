#pragma once
#include "stddeps.h"
import utils;

void string_substr()
{
	//Get variables
	string firstArg = getArrayIndex(getVariable(ARGUMENT_VARIABLE), 0);
	string secondArg = getArrayIndex(getVariable(ARGUMENT_VARIABLE), 1);
	string thirdArg = getArrayIndex(getVariable(ARGUMENT_VARIABLE), 2);

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

void string_toString()
{
	string firstArg = getArrayIndex(getVariable(ARGUMENT_VARIABLE), 0);

	registers[RETURN_REGISTER] = convertToSaveableStr(convertToPrintableStr(firstArg));
}