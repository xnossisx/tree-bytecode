#include "stddeps.h"
import utils;
import error;

void array_insert()
{
	string firstArg = getArrayIndex(getVariable(PARAMETER_VARIABLE), 0);
	if (firstArg[0] != VARNAME && getVariable(stringToUI64(firstArg.substr(1)))[0] != ARRAY)
		fatalError(FORMAT_INVALID_PARAM_TYPE);

	string secondArg = getArrayIndex(getVariable(PARAMETER_VARIABLE), 1);//no type limitations
	if (secondArg[0] == REGISTER)
	{
		secondArg = registers[secondArg[1]];
	}
	if (secondArg[0] == VARNAME)
	{
		secondArg = getVariable(stringToUI64(secondArg.substr(1)));
	}

	if (secondArg[0] != NUM64)
		fatalError(FORMAT_INVALID_PARAM_TYPE);

	longer insertionIndex = stringToUI64(secondArg.substr(1));

	string thirdArg = getArrayIndex(getVariable(PARAMETER_VARIABLE), 2);//no type limitations
	if (thirdArg[0] == REGISTER)
	{
		thirdArg = registers[thirdArg[1]];
	}
	if (thirdArg[0] == VARNAME)
	{
		thirdArg = getVariable(stringToUI64(thirdArg.substr(1)));
	}

	auto arrayStructure = getVariable(stringToUI64(firstArg.substr(1)));

	longer length = stringToUI64(arrayStructure.substr(1, 8));
	length++;

	char* newElement = createVariable(thirdArg);

	string newElementPointer = UI64ToString(reinterpret_cast<intptr_t>(newElement));
	arrayStructure = arrayStructure.substr(9, (insertionIndex) * 8) + newElementPointer + arrayStructure.substr((insertionIndex + 1) * 8);

	string lengthAsString = UI64ToString(length);
	arrayStructure = (char)ARRAY + lengthAsString + arrayStructure;

	createOrAssignVariable(stringToUI64(firstArg.substr(1)), arrayStructure);
}

void array_unshift()
{
	string firstArg = getArrayIndex(getVariable(PARAMETER_VARIABLE), 0);
	if (firstArg[0] != VARNAME && getVariable(stringToUI64(firstArg.substr(1)))[0] != ARRAY)
		fatalError(FORMAT_INVALID_PARAM_TYPE);

	longer insertionIndex = 0;

	string secondArg = getArrayIndex(getVariable(PARAMETER_VARIABLE), 2);//no type limitations
	if (secondArg[0] == REGISTER)
	{
		secondArg = registers[secondArg[1]];
	}
	if (secondArg[0] == VARNAME) //but do convert any values from a variable to their sub-value
	{
		secondArg = getVariable(stringToUI64(secondArg.substr(1)));
	}

	auto arrayStructure = getVariable(stringToUI64(firstArg.substr(1)));

	longer length = stringToUI64(arrayStructure.substr(1, 8));
	length++;

	char* newElement = createVariable(secondArg);

	string newElementPointer = UI64ToString(reinterpret_cast<intptr_t>(newElement));
	arrayStructure = arrayStructure.substr(9, (insertionIndex) * 8) + newElementPointer + arrayStructure.substr((insertionIndex + 1) * 8);

	string lengthAsString = UI64ToString(length);
	arrayStructure = (char)ARRAY + lengthAsString + arrayStructure;

	createOrAssignVariable(stringToUI64(firstArg.substr(1)), arrayStructure);
}

void array_shift()
{
	string firstArg = getArrayIndex(getVariable(PARAMETER_VARIABLE), 0);
	if (firstArg[0] != VARNAME && getVariable(stringToUI64(firstArg.substr(1)))[0] != ARRAY)
		fatalError(FORMAT_INVALID_PARAM_TYPE);

	auto arrayStructure = getVariable(stringToUI64(firstArg.substr(1)));

	longer length = stringToUI64(arrayStructure.substr(1, 8));
	length--;

	arrayStructure = (char)ARRAY + UI64ToString(length) + arrayStructure.substr(17);
	createOrAssignVariable(stringToUI64(firstArg.substr(1)), arrayStructure);
}
