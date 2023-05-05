import utils;
export module std_array;
#include "stddeps.h"

export namespace std_array {
	void array_insert()
	{
		string firstArg = getArrayIndex(getVariable(ARGUMENT_VARIABLE), 0);
		if (firstArg[0] != VARNAME && getVariable(stringToLongLong(firstArg.substr(1)))[0] != ARRAY)
			fatalError(FORMAT_INVALID_PARAM_TYPE);

		string secondArg = getArrayIndex(getVariable(ARGUMENT_VARIABLE), 1);//no type limitations
		if (secondArg[0] == REGISTER)
		{
			secondArg = registers[secondArg[1]];
		}
		if (secondArg[0] == VARNAME)
		{
			secondArg = getVariable(stringToLongLong(secondArg.substr(1)));
		}

		if (secondArg[0] != NUM64)
			fatalError(FORMAT_INVALID_PARAM_TYPE);

		longer insertionIndex = stringToLongLong(secondArg.substr(1));

		string thirdArg = getArrayIndex(getVariable(ARGUMENT_VARIABLE), 2);//no type limitations
		if (thirdArg[0] == REGISTER)
		{
			thirdArg = registers[thirdArg[1]];
		}
		if (thirdArg[0] == VARNAME)
		{
			thirdArg = getVariable(stringToLongLong(thirdArg.substr(1)));
		}

		auto arrayStructure = getVariable(stringToLongLong(firstArg.substr(1)));

		longer length = stringToLongLong(arrayStructure.substr(1, 8));
		length++;

		char* newElement = createVariable(thirdArg);

		string newElementPointer = longLongToString(reinterpret_cast<intptr_t>(newElement));
		arrayStructure = arrayStructure.substr(9, (insertionIndex) * 8) + newElementPointer + arrayStructure.substr((insertionIndex + 1) * 8);

		string lengthAsString = longLongToString(length);
		arrayStructure = (char)ARRAY + lengthAsString + arrayStructure;

		createOrAssignVariable(stringToLongLong(firstArg.substr(1)), arrayStructure);
	}

	void array_unshift()
	{
		string firstArg = getArrayIndex(getVariable(ARGUMENT_VARIABLE), 0);
		if (firstArg[0] != VARNAME && getVariable(stringToLongLong(firstArg.substr(1)))[0] != ARRAY)
			fatalError(FORMAT_INVALID_PARAM_TYPE);

		longer insertionIndex = 0;

		string secondArg = getArrayIndex(getVariable(ARGUMENT_VARIABLE), 2);//no type limitations
		if (secondArg[0] == REGISTER)
		{
			secondArg = registers[secondArg[1]];
		}
		if (secondArg[0] == VARNAME) //but do convert any values from a variable to their sub-value
		{
			secondArg = getVariable(stringToLongLong(secondArg.substr(1)));
		}

		auto arrayStructure = getVariable(stringToLongLong(firstArg.substr(1)));

		longer length = stringToLongLong(arrayStructure.substr(1, 8));
		length++;

		char* newElement = createVariable(secondArg);

		string newElementPointer = longLongToString(reinterpret_cast<intptr_t>(newElement));
		arrayStructure = arrayStructure.substr(9, (insertionIndex) * 8) + newElementPointer + arrayStructure.substr((insertionIndex + 1) * 8);

		string lengthAsString = longLongToString(length);
		arrayStructure = (char)ARRAY + lengthAsString + arrayStructure;

		createOrAssignVariable(stringToLongLong(firstArg.substr(1)), arrayStructure);
	}

	void array_shift()
	{
		string firstArg = getArrayIndex(getVariable(ARGUMENT_VARIABLE), 0);
		if (firstArg[0] != VARNAME && getVariable(stringToLongLong(firstArg.substr(1)))[0] != ARRAY)
			fatalError(FORMAT_INVALID_PARAM_TYPE);

		auto arrayStructure = getVariable(stringToLongLong(firstArg.substr(1)));

		longer length = stringToLongLong(arrayStructure.substr(1, 8));
		length--;

		arrayStructure = (char)ARRAY + longLongToString(length) + arrayStructure.substr(17);
		createOrAssignVariable(stringToLongLong(firstArg.substr(1)), arrayStructure);
	}
}