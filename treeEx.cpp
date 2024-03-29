// treeEx.cpp : This file contains the 'main' function. Program execution begins and ends there.
//



#include <fstream>//here for obvious reasons
#include <cmath>//for pow() and ldexp()
//#include <algorithm>
#include "stdlib/libmain.h"
import utils;
import error;
#pragma warning(disable:4244)

/*
 array file streams done
 getting variable values from string done
 getting values from array done

*/
//all numbers are in little endian

//shown when fatal error occurs
void fatalError(treeError error)
{
	//Creates error file
	std::ofstream errorfile{ "errorcode.txt" };
	errorfile << "An error occured with error code: " << error << "(" << errorNames[error] << ")" << " at: " << file_info::programCode.tellg()<<" in file: "<< file_info::curFile <<'\n';
	errorfile.close();

	string errorMessage = "Error code: ";
	errorMessage += std::to_string(error);
	errorMessage += " (" + string(errorNames[error]) + ")" + " \nat: " + std::to_string((int)file_info::programCode.tellg()) + " in file: " + file_info::curFile + '\n' + "Further information can be found in errorcode.txt, which shares the same directory as " + file_info::curFile;
	//Creates an error box for the program user
	MessageBoxA(nullptr, errorMessage.c_str(), "Tree Unhandled Exception", 0);

	exit(-1);
}

//print a string as an int
void printStrasInt(string x)
{
	for (int i = 0; i < x.length(); i++)
	{
		cout << (int)(unsigned char)(x[i])<<'\n';
	}
}

char cmp=0;

ulonger getFile64Int(stringstream *x);

//variable allocation functions

string floatToString(double x)
{
	union strnum {
		char str[8];
		double num;
	} number;
	number.num = x;
	return string(number.str, 8);
}

//gets a null-terminable string from a file (unlikely to be useful)
string getFileNulltermString(stringstream* x)
{
	string y="";
	char z;
	x->get(z);
	while (z != 0)//0 is the null char 
	{
		y += z;
		x->get(z);
	}
	return y;
}

int stringToInt(string x)
{
	int y = 0;
	for (int i = 0; i < 4; i++)
	{
		y += x[i]*pow(256, i);
	}
	return y;
}

//gets a 64-bit floating point number from a file (no type carrying)
ulonger getFileFloat(stringstream* x)
{
	//yep this works, because they have the same size, so we can copy it over
	return getFile64Int(x);
}

//gets an integer from basic chars in a file, of differing sizes(no type carrying)
ulonger getFile64Int(stringstream* x)
{
	unsigned char y;
	longer z = 0;
	longer startSize = 1;
	for (byte i = 0; i < 7; i++)
	{
		y=x->get();
		z += startSize * y;
		startSize *= 256;
	}
	char a=x->get();
	if (a < 0)
	{
		y = -y;
	}
	z += startSize * a;
	return z;
}
int getFile32Int(stringstream* x)
{
	unsigned char y;
	int z = 0;
	int startSize = 1;
	for (byte i = 0; i < 3; i++)
	{
		y=x->get();
		z += startSize * y;
		startSize *= 256;
	}
	char a = x->get();
	if (a < 0)
	{
		y = -y;
	}
	z += startSize * a;
	return z;
}
short getFile16Int(stringstream* x)
{
	unsigned char y;
	short z = 0;
	short startSize = 1;

	y=x->get();
	z += startSize*y;
	startSize *= 256;

	char a = x->get();
	if (a < 0)
	{
		y = -y;
	}
	z += startSize * a;

	return z;
}
byte getFileByteInt(stringstream* x)
{
	char y;
	x->get(y);
	return y;
}
//gets a string from a file as the extracted string
string getFileStr(stringstream* file)
{
	string y;
	longer strLen = getFile64Int(file);
	char z = '\01';
	y += UI64ToString(strLen);
	for (longer i=0;i<strLen;i++)
	{
		file->get(z);
		y += z;
	}
	return y;
}

//obtain raw function reference
string getFileFunctionRef(stringstream* file)
{
	string y;
	y += getFileStr(file);
	char hold[8];
	file->read(hold, 8);
	y.append(hold, 8);
	return y;
}

//gets a boolean from a file(no type carrying)
bool getFileBool(stringstream* file)
{
	return file->get();
}


//gets an array from a file
char* createArrayFromFile(stringstream* file, ulonger arrName, bool indexed=true)
{
	char variableHeader;//reading char
	vector<intptr_t> dataPointers = {};
	file->get(variableHeader);
	string a;
	while (variableHeader != 'ÿ')
	{
		a = "";
		//while there are still indexes left to read
		switch (variableHeader)
		{
		case vartypes::VARNAME:
			a.push_back(vartypes::VARNAME);
			a+=UI64ToString(getFile64Int(file));
			break;
		case vartypes::STRING:
			a.push_back(vartypes::STRING);
			a.append(getFileStr(file));
			break;
		case vartypes::NUM32:
			a.push_back(vartypes::NUM32);
			for (int i = 0; i < 4; i++)
			{
				a.push_back((char)(file->get()));
			}
			break;
		case vartypes::NUM64:
			a.push_back(vartypes::NUM64);
			for (int i = 0; i < 8; i++)
			{
				a.push_back((char)(file->get()));
			}
			break;
		case vartypes::NUM16:
			a.push_back(vartypes::NUM16);
			for (int i = 0; i < 2; i++)
			{
				a.push_back((char)(file->get()));
			}
			break;
		case vartypes::NUMBYTE:
			a.push_back(vartypes::NUMBYTE);
			a.push_back((char)(file->get()));
			break;
		case vartypes::BOOLE:
			a.push_back(vartypes::BOOLE);
			a.push_back((char)(file->get()));
			break;
		case vartypes::CHARA://char case
			a.push_back(vartypes::CHARA);
			a.push_back((char)(file->get()));
			break;
		case vartypes::UNDEFINED://no need to add anything here
			a.push_back(vartypes::UNDEFINED);
			break;
		case vartypes::OBJECT:
			a.push_back(vartypes::OBJECT);
			a+=UI64ToString(reinterpret_cast<intptr_t>(createObject()));
			break;
		case vartypes::ARRAY:
			a.push_back(vartypes::ARRAY);
			dataPointers.push_back(reinterpret_cast<intptr_t>(createArrayFromFile(file, 0/*not needed*/ ,false)));
			break;
		case vartypes::FUNCTION:
		{
			a.push_back(vartypes::FUNCTION);
			string ref = getFileFunctionRef(file);
			a.append(ref);
		}
		}
		if (variableHeader != vartypes::ARRAY)//special handling for arrays
		{
			dataPointers.push_back(reinterpret_cast<intptr_t>(createVariable(a)));//this pushes the locations of data in
		}
		file->get(variableHeader);
		if (file->eof())
		{
			fatalError(RUNTIME_ARRAY_OVERFLOW);
		}
	}
	
	string arrStructure;
	arrStructure.push_back(vartypes::ARRAY);
	arrStructure.append(UI64ToString(dataPointers.size()));
	for (unsigned int i = 0; i < dataPointers.size(); i++)
	{
		arrStructure.append(UI64ToString(dataPointers[i]));
	}

	if (!indexed)
	{
		return createVariable(arrStructure);
	}
	else
	{
		createVariable(arrName, arrStructure);
		return (char*)getAddress(arrName);
	}
}

//get any variable type from the .tree
string getFileVarVal(stringstream* x)
{
	byte varTyp = x->get();
	string retStr;
	switch (varTyp)
	{
	case vartypes::NUMBYTE:
		retStr = x->get();
		break;
	case vartypes::NUM16:
		retStr = UI64ToString(getFile16Int(x)).substr(0, 2);//a little weird, but it's for allocation
		break;
	case vartypes::NUM32:
		retStr = UI64ToString(getFile32Int(x)).substr(0, 4);
		break;
	case vartypes::NUM64:
		retStr = UI64ToString(getFile64Int(x));
		break;
	case vartypes::CHARA:
		retStr = x->get();
		break;
	case vartypes::BOOLE:
		retStr = x->get();
		break;
	case vartypes::STRING:
		retStr = getFileStr(x);
		break;
	case vartypes::NUMFLOAT:
		retStr = UI64ToString(getFile64Int(x));
		break;
	case vartypes::REGISTER://special register reference scenario
		retStr = x->get();
		break;
	case vartypes::VARNAME:
		retStr = UI64ToString(getFile64Int(x));
		break;
	case vartypes::FUNCTION:
		retStr = getFileFunctionRef(x);
		break;

	}

	return (char)varTyp+retStr;
}


string getFileVarVal(stringstream* x);

void deleteArray(longer index);

void deleteNonIndexedArray(char* indexPointer);

//removes an array from the allocation database, and deallocates all members
void deleteArray(longer arrayName)
{
	//Get primary array structure (assumed that arrayName is a valid variable name, but there is error checking in getVariable anyways)
	string arrayStructure = getVariable(arrayName);

	longer length = stringToUI64(arrayStructure.substr(1,8));

	//Initializing everything in the loop
	longer arrayElementIter = 9;
	string pointerSum;
	char* pointer; //pointer to the element that will be deleted

	//loop where all sub-elements are deleted
	for (longer i = 0; i < length; i++)
	{
		pointerSum = ""; //resets the sum
		for (int j = 0; j < 8; j++)
		{
			pointerSum += arrayStructure[arrayElementIter];

			arrayElementIter++;
		}

		pointer = reinterpret_cast<char*>(stringToUI64(pointerSum));

		if (pointer[0] == ARRAY)
			deleteNonIndexedArray(pointer); //delete any subarrays
		else if (pointer[0] == OBJECT)
			delete reinterpret_cast<Object*>(stringToUI64(string(pointer, 9).substr(1, 8)));
		else
			delete[] pointer;
	}

	delete[] getAddress(arrayName);
}

//internal version used to delete semi-anonymous arrays, parameter is invalidated
void deleteNonIndexedArray(char* indexPointer)
{
	//Get primary array structure (assumed that arrayName is a valid variable name, but there is error checking in getVariable anyways)
	string arrayStructure = getVarViaAddress(indexPointer);

	longer length = stringToUI64(arrayStructure.substr(1, 8));

	//Initializing everything in the loop
	longer arrayElementIter = 9;
	string pointerSum;
	char* pointer; //pointer to the element that will be deleted

	//loop where all sub-elements are deleted
	for (longer i = 0; i < length; i++)
	{
		pointerSum = ""; //resets the sum
		for (int j = 0; j < 8; j++)
		{
			pointerSum += arrayStructure[arrayElementIter];

			arrayElementIter++;
		}

		pointer = reinterpret_cast<char*>(stringToUI64(pointerSum));

		if (pointer[0] == ARRAY)
			deleteNonIndexedArray(pointer); //delete any subarrays
		else if (pointer[0] == OBJECT)
			delete reinterpret_cast<Object*>(stringToUI64(string(pointer, 9).substr(1, 8)));
		else
			delete[] pointer;
	}

	delete[] indexPointer;
}

// runs a single command
// @param stringstream* x: the stream at which to read commands from
longer runStatement(stringstream* x)
{
	//initializes the command type and first argument
	byte commandType = getFileByteInt(x);
	
	if ((char)commandType == '\xFF')//this is 0xFF
	{
		return 1;
	}
	
	//cout << "\nArgtype: " << (int)commandType << " Position: " << (int)x->tellg() << "\n";

	string firstArg = getFileVarVal(x);

	if (firstArg[0]==vartypes::OBJECT)
	{
		firstArg = (char)vartypes::OBJECT + UI64ToString(reinterpret_cast<intptr_t>(createObject()));
	}

	// we initalize these here because switch statements are annoying
	string secondArg;
	string varVal;
	int flag=0;
	string flagstr;
	std::fstream file; //used primarily to jump between files
	stringstream code;

	switch (commandType)
	{
	case 0: //mov regorvar<-other data(not a variable value)
		secondArg = getFileVarVal(x); //get the second argument
		if (firstArg[0] == REGISTER) //moving into a register
			if (secondArg[0] != REGISTER)
				registers[firstArg[1]] = secondArg;
			else
				registers[firstArg[1]] = registers[secondArg[1]];
		else if (firstArg[0] == VARNAME) //so it's a variable we're moving into
			if (secondArg[0] != REGISTER)
				createOrAssignVariable(stringToUI64(firstArg.substr(1)), secondArg);
			else
				createOrAssignVariable(stringToUI64(firstArg.substr(1)), registers[secondArg[1]]);
		else//it's not a valid type
			fatalError(FORMAT_INVALID_PARAM_TYPE);
		break;
	case 1: //movv regorvar<-variable address
		secondArg = getFileVarVal(x);
		if (secondArg[0] != VARNAME)
			fatalError(FORMAT_INVALID_PARAM_TYPE);
		if (!isThereVariable(stringToUI64(secondArg.substr(1))))
		{
			fatalError(RUNTIME_INVALID_VARIABLE_NAME);
		}
		varVal = getVariable(stringToUI64(secondArg.substr(1)));
		if (firstArg[0] == REGISTER)//moving into a register
			registers[firstArg[1]] = varVal;
		else if (firstArg[0] == VARNAME)//so it's a variable we're moving into
		{
			createOrAssignVariable(stringToUI64(firstArg.substr(1)), varVal);
		}
		else//it's not a valid type
			fatalError(FORMAT_INVALID_PARAM_TYPE);

		break;
	case 2: //call local function
		if (firstArg[0] != NUM64)
			fatalError(FORMAT_INVALID_PARAM_TYPE);
		if (runEventCodeAt(x, stringToUI64(firstArg.substr(1)))==-1)
		{
			return -1;
		}
		break;
	case 3://jmp
		if (firstArg[0] != NUM64)
			fatalError(FORMAT_INVALID_PARAM_TYPE);
		x->seekg(stringToUI64(firstArg.substr(1)));
		break;
	case 4://push
		if (firstArg[1] != REGISTER)
			stackVars.push_back(firstArg);
		else
			stackVars.push_back(registers[firstArg[1]]);
		break;
	case 5://pop
		if (firstArg[0] != REGISTER)
		{
			fatalError(FORMAT_INVALID_PARAM_TYPE);
		}
		registers[firstArg[1]] = stackVars[stackVars.size() - 1];
		stackVars.pop_back();
		break;
	case 6://print
		cout << convertToPrintableStr(firstArg);
		break;
	case 7://cmp
		//if cmp==0, then it's equal, if number 1 is less than number 2, it's -1, if number 1 is greater than number 2, it's 1, and if the two numbers are not equal but can't be compared, it's -2
		secondArg = getFileVarVal(x);

		if (firstArg == secondArg)
		{
			cmp = 0;
			break;//if they're auto-equal, we know we can break
		}
		if (firstArg[0] == REGISTER)
			firstArg = registers[firstArg[1]];
		if (secondArg[0] == REGISTER)
			secondArg = registers[secondArg[1]];
		if (secondArg[0] == STRING && firstArg[0] == STRING)
		{
			cmp = -2;//because the equals case would have already broken from here if it was a string(not in other cases)
			break;
		}
		if (firstArg[0] == NUM64 || firstArg[0] == NUM32 || firstArg[0] == NUM16 || firstArg[0] == NUMBYTE || firstArg[0] == CHARA)
		{//if the first arg is a number
			if (secondArg[0] == NUM64 || secondArg[0] == NUM32 || secondArg[0] == NUM16 || secondArg[0] == NUMBYTE || secondArg[0] == CHARA)//if the second arg is a number
			{
				if (stringToUI64(firstArg.substr(1), firstArg.length() - 1) > stringToUI64(secondArg.substr(1), secondArg.length() - 1))
					cmp = 1;
				else if (stringToUI64(firstArg.substr(1), firstArg.length() - 1) == stringToUI64(secondArg.substr(1), secondArg.length() - 1))
					cmp = 0;
				else
					cmp = -1;
			}
			else if (secondArg[0] != NUMFLOAT) fatalError(FORMAT_INVALID_PARAM_TYPE);
		}
		else if (firstArg[0] != NUMFLOAT) fatalError(FORMAT_INVALID_PARAM_TYPE);
		if (firstArg[0] == NUMFLOAT)//now we don't have to copy this over to the second argument case
		{
			if (secondArg[0] == NUMFLOAT)
			{
				if (stringToFloat(firstArg.substr(1)) > stringToFloat(secondArg.substr(1)))
					cmp = 1;
				else if (stringToFloat(firstArg.substr(1)) == stringToFloat(secondArg.substr(1)))
					cmp = 0;
				else
					cmp = -1;
			}
			else if (secondArg[0] == NUM64 || secondArg[0] == NUM32 || secondArg[0] == NUM16 || secondArg[0] == NUMBYTE || secondArg[0] == CHARA)//if the second arg is a number
			{
				if (stringToFloat(firstArg.substr(1)) > stringToUI64(secondArg.substr(1), secondArg.length() - 1))
					cmp = 1;
				else if (stringToFloat(firstArg.substr(1)) == stringToUI64(secondArg.substr(1), secondArg.length() - 1))
					cmp = 0;
				else
					cmp = -1;
			}
		}
		else if (secondArg[0] == NUMFLOAT)
		{
			if (firstArg[0] == NUM64 || firstArg[0] == NUM32 || firstArg[0] == NUM16 || firstArg[0] == NUMBYTE || firstArg[0] == CHARA)
			{//if the first arg is a number
				if (stringToUI64(firstArg.substr(1), firstArg.length() - 1) > stringToFloat(secondArg.substr(1)))
					cmp = 1;
				else if (stringToUI64(firstArg.substr(1), firstArg.length() - 1) == stringToFloat(secondArg.substr(1)))
					cmp = 0;
				else
					cmp = -1;
			}
		}
		break;
	case 8://je
		if (firstArg[0] != NUM64)//throws error if the program recieves an invalid argument
			fatalError(FORMAT_INVALID_PARAM_TYPE);
		if (cmp == 0)//if they are equal, cmp==0
			x->seekg(stringToUI64(firstArg.substr(1)));
		break;
	case 9://jne
		if (firstArg[0] != NUM64)//all of these are pretty similar
			fatalError(FORMAT_INVALID_PARAM_TYPE);
		if (cmp != 0)
			x->seekg(stringToUI64(firstArg.substr(1)));
		break;
	case 10://jg
		if (firstArg[0] != NUM64)
			fatalError(FORMAT_INVALID_PARAM_TYPE);
		if (cmp == 1)
			x->seekg(stringToUI64(firstArg.substr(1)));
		break;
	case 11://jl
		if (firstArg[0] != NUM64)
			fatalError(FORMAT_INVALID_PARAM_TYPE);
		if (cmp == -1)
			x->seekg(stringToUI64(firstArg.substr(1)));
		break;
	case 12://jng
		if (firstArg[0] != NUM64)
			fatalError(FORMAT_INVALID_PARAM_TYPE);
		if (cmp != 1)
			x->seekg(stringToUI64(firstArg.substr(1)));
		break;
	case 13://jnl
		if (firstArg[0] != NUM64)
			fatalError(FORMAT_INVALID_PARAM_TYPE);
		if (cmp != -1)
			x->seekg(stringToUI64(firstArg.substr(1)));
		break;
	case 14://add
		if (firstArg[0] == VARNAME)
			firstArg = getVariable(stringToUI64(firstArg.substr(1)));
		if (firstArg[0] == REGISTER)
			firstArg = registers[firstArg[1]];
		secondArg = getFileVarVal(x);
		if (secondArg[0] == VARNAME)
			secondArg = getVariable(stringToUI64(secondArg.substr(1)));
		if (secondArg[0] == REGISTER)
			secondArg = registers[secondArg[1]];
		if (firstArg[0] == NUM64 ? 8 : firstArg[0] == NUM32 ? 4 : firstArg[0] == NUM16 ? 2 : 1 < secondArg[0] == NUM64 ? 8 : secondArg[0] == NUM32 ? 4 : secondArg[0] == NUM16 ? 2 : 1)
		{
			std::swap(firstArg, secondArg);//swap it here so we can simplify the addition procedure by not having to have multiple ifs
		}

		if ((firstArg[0] == NUM64 || firstArg[0] == NUM32 || firstArg[0] == NUM16 || firstArg[0] == NUMBYTE || firstArg[0] == CHARA) && (secondArg[0] == NUM64 || secondArg[0] == NUM32 || secondArg[0] == NUM16 || secondArg[0] == NUMBYTE || secondArg[0] == CHARA))
		{
			registers[COMMAND_REGISTER] = firstArg[0] + (UI64ToString(stringToUI64(firstArg.substr(1), firstArg.length() - 1) + stringToUI64(secondArg.substr(1), secondArg.length() - 1), firstArg[0] == NUM64 ? 8 : firstArg[0] == NUM32 ? 4 : firstArg[0] == NUM16 ? 2 : 1));
		}
		else if (firstArg[0] == STRING && secondArg[0] == STRING)
		{
			registers[COMMAND_REGISTER] = convertToSaveableStr(convertToProperStr(firstArg) + convertToProperStr(secondArg));
		}
		else if (firstArg[0] == CHARA && secondArg[0] == CHARA)
		{
			registers[COMMAND_REGISTER] = convertToSaveableStr(string(1, (char)STRING).append(1, firstArg[1]).append(1, firstArg[2]));
		}
		else if (firstArg[0] == STRING && secondArg[0] == CHARA)
		{
			registers[COMMAND_REGISTER] = convertToSaveableStr(convertToProperStr(firstArg) + secondArg[1]);
		}

		break;
	case 15://sub
		if (firstArg[0] == VARNAME)
			firstArg = getVariable(stringToUI64(firstArg.substr(1)));
		if (firstArg[0] == REGISTER)
			firstArg = registers[firstArg[1]];
		secondArg = getFileVarVal(x);
		if (secondArg[0] == VARNAME)
			secondArg = getVariable(stringToUI64(secondArg.substr(1)));
		if (secondArg[0] == REGISTER)
			secondArg = registers[secondArg[1]];
		if (firstArg[0] == NUM64 ? 8 : firstArg[0] == NUM32 ? 4 : firstArg[0] == NUM16 ? 2 : 1 < secondArg[0] == NUM64 ? 8 : secondArg[0] == NUM32 ? 4 : secondArg[0] == NUM16 ? 2 : 1)
		{
			firstArg[0] = secondArg[0];
		}
		if ((firstArg[0] == NUM64 || firstArg[0] == NUM32 || firstArg[0] == NUM16 || firstArg[0] == NUMBYTE || firstArg[0] == CHARA) && (secondArg[0] == NUM64 || secondArg[0] == NUM32 || secondArg[0] == NUM16 || secondArg[0] == NUMBYTE || secondArg[0] == CHARA))
		{
			registers[COMMAND_REGISTER] = firstArg[0] + (UI64ToString(stringToUI64(firstArg.substr(1), firstArg.length() - 1) - stringToUI64(secondArg.substr(1), secondArg.length() - 1), firstArg[0] == NUM64 ? 8 : firstArg[0] == NUM32 ? 4 : firstArg[0] == NUM16 ? 2 : 1));
		}
		break;
	case 16://mul
		if (firstArg[0] == VARNAME)
			firstArg = getVariable(stringToUI64(firstArg.substr(1)));
		if (firstArg[0] == REGISTER)
			firstArg = registers[firstArg[1]];
		secondArg = getFileVarVal(x);
		if (secondArg[0] == VARNAME)
			secondArg = getVariable(stringToUI64(secondArg.substr(1)));
		if (secondArg[0] == REGISTER)
			secondArg = registers[secondArg[1]];
		if (firstArg[0] < 5 && secondArg[0] < 5) /* all type values less than 5 are integer types */
		{
			registers[COMMAND_REGISTER] = firstArg[0] + (UI64ToString(stringToUI64(firstArg.substr(1), firstArg.length() - 1) * stringToUI64(secondArg.substr(1), secondArg.length() - 1), firstArg[0] == NUM64 ? 8 : firstArg[0] == NUM32 ? 4 : firstArg[0] == NUM16 ? 2 : 1));
		} 
		else if (firstArg[0] == vartypes::NUMFLOAT && secondArg[0] == vartypes::NUMFLOAT) 
		{
			registers[COMMAND_REGISTER] = (char)vartypes::NUMFLOAT + (numericalToString<double>(stringToFloat(firstArg.substr(1)) * stringToFloat(secondArg.substr(1))));
		}
		break;
	case 17://div
		if (firstArg[0] == VARNAME)
			firstArg = getVariable(stringToUI64(firstArg.substr(1)));
		if (firstArg[0] == REGISTER)
			firstArg = registers[firstArg[1]];
		secondArg = getFileVarVal(x);
		if (secondArg[0] == VARNAME)
			secondArg = getVariable(stringToUI64(secondArg.substr(1)));
		if (secondArg[0] == REGISTER)
			secondArg = registers[secondArg[1]];
		if (firstArg[0] == NUM64 ? 8 : firstArg[0] == NUM32 ? 4 : firstArg[0] == NUM16 ? 2 : 1 < secondArg[0] == NUM64 ? 8 : secondArg[0] == NUM32 ? 4 : secondArg[0] == NUM16 ? 2 : 1)
		{
			firstArg[0] = secondArg[0];
		}
		if ((firstArg[0] < STRING) && (secondArg[0] < STRING))
		{
			longer denominator = stringToUI64(secondArg.substr(1), secondArg.length() - 1);
			if (denominator == 0) fatalError(RUNTIME_MATH_EXCEPTION);
			registers[COMMAND_REGISTER] = firstArg[0] + (UI64ToString(stringToUI64(firstArg.substr(1), firstArg.length() - 1) / denominator, firstArg[0] == NUM64 ? 8 : firstArg[0] == NUM32 ? 4 : firstArg[0] == NUM16 ? 2 : 1));
		}
		else if (firstArg[0] == NUMFLOAT && secondArg[0] == NUMFLOAT) 
		{
			registers[COMMAND_REGISTER] = (char)vartypes::NUMFLOAT + (numericalToString<double>(stringToFloat(firstArg.substr(1)) / stringToFloat(secondArg.substr(1))));
		}
		break;
	case 18://arrdef
		if (firstArg[0] != VARNAME)
			fatalError(FORMAT_INVALID_PARAM_TYPE);
		secondArg = getFileVarVal(x);

		if (secondArg[0] != ARRAY)
			fatalError(FORMAT_INVALID_PARAM_TYPE);

		createArrayFromFile(x, stringToUI64(firstArg.substr(1)));
		break;
	case 19://arrgetbyname
		if (firstArg[0] != VARNAME)
			fatalError(FORMAT_INVALID_PARAM_TYPE);
		secondArg = getFileVarVal(x);

		if (secondArg[0] != NUM64)
			fatalError(FORMAT_INVALID_PARAM_TYPE);

		registers[COMMAND_REGISTER] = getArrayIndex(stringToUI64(firstArg.substr(1)), stringToUI64(secondArg.substr(1)));
		break;
	case 20://obj get
	{
		//Initialize all variables required in the interaction
		if (firstArg[0] != VARNAME && firstArg[0] != REGISTER)//obj1
		{
			fatalError(FORMAT_INVALID_PARAM_TYPE);
		}
		else if (!((firstArg[0] == VARNAME && getVariable(stringToUI64(firstArg.substr(1)))[0] == OBJECT) || (firstArg[0] == REGISTER && registers[firstArg[1]][0] == OBJECT)))
		{
			std::cout << (int)getVariable(stringToUI64(firstArg.substr(1)))[0];
			fatalError(RUNTIME_INVALID_PARAM_TYPE);
		}

		Object* objSource;

		if (firstArg[0] == VARNAME)//conversion from firstArg to objSource
		{
			objSource = retrieveObjectFromString(getVariable(stringToUI64(firstArg.substr(1))));
		}
		else
		{
			objSource = retrieveObjectFromString(registers[firstArg[1]].substr(1));
		}

		secondArg = getFileVarVal(x);

		if ((secondArg[0] != VARNAME && secondArg[0] != REGISTER) && secondArg[0] != STRING)//prop
		{
			fatalError(FORMAT_INVALID_PARAM_TYPE);
		}
		else if (!((secondArg[0] == VARNAME && getVariable(stringToUI64(secondArg.substr(1)))[0] == STRING) || (secondArg[0] == REGISTER && registers[secondArg[1]][0] == STRING)) && secondArg[0] != STRING)
		{
			fatalError(RUNTIME_INVALID_PARAM_TYPE);
		}

		string propertyName; //property name
		if (secondArg[0] == VARNAME)
		{
			propertyName = convertToProperStr(getVariable(stringToUI64(secondArg.substr(1))));
		}
		else if (secondArg[0] == REGISTER)
		{
			propertyName = convertToProperStr(registers[secondArg[1]]);
		}
		else
		{
			propertyName = convertToProperStr(secondArg);
		}

		//Get the object property
		string objectProperty = getObjectProperty(objSource, propertyName);

		string thirdArg = getFileVarVal(x); //destination

		//Feed the object property into the final destination
		if (thirdArg[0] == REGISTER)
		{
			registers[thirdArg[1]] = objectProperty;
		}
		else if (thirdArg[0] == VARNAME)
		{
			createOrAssignVariable(stringToUI64(thirdArg.substr(1)), objectProperty);
		}
		else //this means that argument three is invalid
		{
			fatalError(FORMAT_INVALID_PARAM_TYPE);
		}
		break;
	}
	case 21: //inc
	{
		char argType = firstArg[0];

		if (argType == REGISTER)
		{
			byte registerIndex = firstArg[1];
			firstArg = registers[registerIndex];
			if (firstArg[0] == NUM64 || firstArg[0] == NUM32 || firstArg[0] == NUM16 || firstArg[0] == NUMBYTE || firstArg[0] == CHARA)
			{
				registers[registerIndex] = firstArg[0] + (UI64ToString(stringToUI64(firstArg.substr(1), firstArg.length() - 1) + 1, firstArg[0] == NUM64 ? 8 : firstArg[0] == NUM32 ? 4 : firstArg[0] == NUM16 ? 2 : 1));
			}
			else if (firstArg[0] == NUMFLOAT)
			{
				registers[registerIndex] = firstArg[0] + floatToString(stringToFloat(firstArg.substr(1)) + 1);
			}
			else if (firstArg[0] == VARNAME)
			{
				argType = firstArg[0];
			}
		}

		if (argType == VARNAME)
		{
			longer variableName = stringToUI64(firstArg.substr(1));
			firstArg = getVariable(variableName);
			if (firstArg[0] == NUM64 || firstArg[0] == NUM32 || firstArg[0] == NUM16 || firstArg[0] == NUMBYTE || firstArg[0] == CHARA)
			{
				createOrAssignVariable(variableName, firstArg[0] + UI64ToString(stringToUI64(firstArg.substr(1), firstArg.length() - 1) + 1, firstArg[0] == NUM64 ? 8 : firstArg[0] == NUM32 ? 4 : firstArg[0] == NUM16 ? 2 : 1));
			}
			else if (firstArg[0] == NUMFLOAT)
			{
				createOrAssignVariable(variableName, firstArg[0] + floatToString(stringToFloat(firstArg.substr(1)) + 1));
			}
		}

		break;
	}
	case 22://call from file
		if (firstArg[0] != STRING)
		{
			fatalError(FORMAT_INVALID_PARAM_TYPE);
		}
		secondArg = getFileVarVal(x);
		if (secondArg[0] != NUM64)
		{
			fatalError(FORMAT_INVALID_PARAM_TYPE);
		}
		file.open(convertToProperStr(firstArg));
		if (!file.is_open())
		{
			fatalError(RUNTIME_FILE_NOT_FOUND);
		}

		flagstr = file_info::curFile;
		file_info::curFile = convertToProperStr(firstArg);
		std::copy(std::istreambuf_iterator<char>(file),
			std::istreambuf_iterator<char>(),
			std::ostreambuf_iterator<char>(code));
		if (runEventCodeAt(&code, stringToUI64(secondArg.substr(1))) == -1)
		{
			return -1;
		}
		file_info::curFile = flagstr;
		break;
	case 23://arrvar getaddr
		if (firstArg[0] != VARNAME)
			fatalError(FORMAT_INVALID_PARAM_TYPE);
		secondArg = getFileVarVal(x);
		if (secondArg[0] != NUM64)
			fatalError(FORMAT_INVALID_PARAM_TYPE);


		//\f is char 12, ADDRESS
		registers[COMMAND_REGISTER] = '\f' + UI64ToString(reinterpret_cast<long long>(getArrayIndexAddress(stringToUI64(firstArg.substr(1)), stringToUI64(secondArg.substr(1)))));
		break;
	case 24: //delete
		if (firstArg[0] != VARNAME)
		{
			fatalError(FORMAT_INVALID_PARAM_TYPE);
		}

		flagstr = firstArg;

		firstArg = getVariable(stringToUI64(firstArg.substr(1)));

		if (firstArg[0] == ARRAY)
		{
			deleteArray(stringToUI64(flagstr.substr(1)));
		}
		else
		{
			if (firstArg[0] == OBJECT)
			{
				retrieveObjectFromString(firstArg)->~Object();
			}
			deleteVariable(stringToUI64(flagstr.substr(1)));
		}
		break;
	case 25: //dec
	{
		char argType = firstArg[0];

		if (argType == REGISTER)
		{
			byte registerIndex = firstArg[1];
			firstArg = registers[registerIndex];
			if (firstArg[0] == NUM64 || firstArg[0] == NUM32 || firstArg[0] == NUM16 || firstArg[0] == NUMBYTE || firstArg[0] == CHARA)
			{
				registers[registerIndex] = firstArg[0] + (UI64ToString(stringToUI64(firstArg.substr(1), firstArg.length() - 1) - 1, firstArg[0] == NUM64 ? 8 : firstArg[0] == NUM32 ? 4 : firstArg[0] == NUM16 ? 2 : 1));
			}
			else if (firstArg[0] == NUMFLOAT)
			{
				registers[registerIndex] = firstArg[0] + floatToString(stringToFloat(firstArg.substr(1)) - 1);
			}
			else if (firstArg[0] == VARNAME)
			{
				argType = firstArg[0];
			}
		}

		if (argType == VARNAME)
		{
			longer variableName = stringToUI64(firstArg.substr(1));
			firstArg = getVariable(variableName);
			if (firstArg[0] == NUM64 || firstArg[0] == NUM32 || firstArg[0] == NUM16 || firstArg[0] == NUMBYTE || firstArg[0] == CHARA)
			{
				createOrAssignVariable(variableName, firstArg[0] + UI64ToString(stringToUI64(firstArg.substr(1), firstArg.length() - 1) - 1, firstArg[0] == NUM64 ? 8 : firstArg[0] == NUM32 ? 4 : firstArg[0] == NUM16 ? 2 : 1));
			}
			else if (firstArg[0] == NUMFLOAT)
			{
				createOrAssignVariable(variableName, firstArg[0] + floatToString(stringToFloat(firstArg.substr(1)) - 1));
			}
		}
		break;
	}
	case 26://mod
		if (firstArg[0] == REGISTER)
			firstArg = registers[firstArg[1]];
		if (firstArg[0] == VARNAME)
			firstArg = getVariable(stringToUI64(firstArg.substr(1)));
		secondArg = getFileVarVal(x);
		if (secondArg[0] == REGISTER)
			secondArg = registers[secondArg[1]];
		if (secondArg[0] == VARNAME)
			secondArg = getVariable(stringToUI64(secondArg.substr(1)));
		if ((firstArg[0] == NUM64 || firstArg[0] == NUM32 || firstArg[0] == NUM16 || firstArg[0] == NUMBYTE || firstArg[0] == CHARA) && (secondArg[0] == NUM64 || secondArg[0] == NUM32 || secondArg[0] == NUM16 || secondArg[0] == NUMBYTE || secondArg[0] == CHARA))
		{
			registers[COMMAND_REGISTER] = firstArg[0] + (UI64ToString(stringToUI64(firstArg.substr(1), firstArg.length() - 1) % stringToUI64(secondArg.substr(1), secondArg.length() - 1), firstArg[0] == NUM64 ? 8 : firstArg[0] == NUM32 ? 4 : firstArg[0] == NUM16 ? 2 : 1));
		}
		break;
	case 27://rs
		if (firstArg[0] == REGISTER)
			firstArg = registers[firstArg[1]];
		if (firstArg[0] == VARNAME)
			firstArg = getVariable(stringToUI64(firstArg.substr(1)));
		secondArg = getFileVarVal(x);
		if (secondArg[0] == REGISTER)
			secondArg = registers[secondArg[1]];
		if (secondArg[0] == VARNAME)
			secondArg = getVariable(stringToUI64(secondArg.substr(1)));
		if ((firstArg[0] == NUM64 || firstArg[0] == NUM32 || firstArg[0] == NUM16 || firstArg[0] == NUMBYTE || firstArg[0] == CHARA) && (secondArg[0] == NUM64 || secondArg[0] == NUM32 || secondArg[0] == NUM16 || secondArg[0] == NUMBYTE || secondArg[0] == CHARA))
		{
			registers[COMMAND_REGISTER] = firstArg[0] + (UI64ToString(stringToUI64(firstArg.substr(1), firstArg.length() - 1) >> stringToUI64(secondArg.substr(1), secondArg.length() - 1), firstArg[0] == NUM64 ? 8 : firstArg[0] == NUM32 ? 4 : firstArg[0] == NUM16 ? 2 : 1));
		}
		else
		{
			fatalError(FORMAT_INVALID_PARAM_TYPE);
		}
		break;
	case 28://ls
		if (firstArg[0] == REGISTER)
			firstArg = registers[firstArg[1]];
		if (firstArg[0] == VARNAME)
			firstArg = getVariable(stringToUI64(firstArg.substr(1)));
		secondArg = getFileVarVal(x);
		if (secondArg[0] == REGISTER)
			secondArg = registers[secondArg[1]];
		if (secondArg[0] == VARNAME)
			secondArg = getVariable(stringToUI64(secondArg.substr(1)));
		if ((firstArg[0] == NUM64 || firstArg[0] == NUM32 || firstArg[0] == NUM16 || firstArg[0] == NUMBYTE || firstArg[0] == CHARA) && (secondArg[0] == NUM64 || secondArg[0] == NUM32 || secondArg[0] == NUM16 || secondArg[0] == NUMBYTE || secondArg[0] == CHARA))
		{
			registers[COMMAND_REGISTER] = firstArg[0] + (UI64ToString(stringToUI64(firstArg.substr(1), firstArg.length() - 1) << stringToUI64(secondArg.substr(1), secondArg.length() - 1), firstArg[0] == NUM64 ? 8 : firstArg[0] == NUM32 ? 4 : firstArg[0] == NUM16 ? 2 : 1));
		}
		else
		{
			fatalError(FORMAT_INVALID_PARAM_TYPE);
		}
		break;
	case 29://andb
		if (firstArg[0] == REGISTER)
			firstArg = registers[firstArg[1]];
		if (firstArg[0] == VARNAME)
			firstArg = getVariable(stringToUI64(firstArg.substr(1)));
		secondArg = getFileVarVal(x);
		if (secondArg[0] == REGISTER)
			secondArg = registers[secondArg[1]];
		if (secondArg[0] == VARNAME)
			secondArg = getVariable(stringToUI64(secondArg.substr(1)));
		if (firstArg[0] == BOOLE && secondArg[0] == BOOLE)
		{
			registers[COMMAND_REGISTER] = (char)BOOLE;
			registers[COMMAND_REGISTER] += (firstArg[1] & secondArg[1]);
		}
		else
		{
			fatalError(FORMAT_INVALID_PARAM_TYPE);
		}
		break;
	case 30://orb
		if (firstArg[0] == REGISTER)
			firstArg = registers[firstArg[1]];
		if (firstArg[0] == VARNAME)
			firstArg = getVariable(stringToUI64(firstArg.substr(1)));
		secondArg = getFileVarVal(x);
		if (secondArg[0] == REGISTER)
			secondArg = registers[secondArg[1]];
		if (secondArg[0] == VARNAME)
			secondArg = getVariable(stringToUI64(secondArg.substr(1)));
		if (firstArg[0] == BOOLE && secondArg[0] == BOOLE)
		{
			registers[COMMAND_REGISTER] = (char)BOOLE;
			registers[COMMAND_REGISTER] += (firstArg[1] | secondArg[1]);
		}
		else
		{
			fatalError(FORMAT_INVALID_PARAM_TYPE);
		}
	case 31://notb
		if (firstArg[0] == REGISTER)
			firstArg = registers[firstArg[1]];
		if (firstArg[0] == VARNAME)
			firstArg = getVariable(stringToUI64(firstArg.substr(1)));
		if (firstArg[0] == BOOLE)
		{
			registers[COMMAND_REGISTER] = (char)BOOLE;
			registers[COMMAND_REGISTER] += ~firstArg[1];
		}
		else
		{
			fatalError(FORMAT_INVALID_PARAM_TYPE);
		}
	case 32://bitwise and
		if (firstArg[0] == REGISTER)
			firstArg = registers[firstArg[1]];
		if (firstArg[0] == VARNAME)
			firstArg = getVariable(stringToUI64(firstArg.substr(1)));
		secondArg = getFileVarVal(x);
		if (secondArg[0] == REGISTER)
			secondArg = registers[secondArg[1]];
		if (secondArg[0] == VARNAME)
			secondArg = getVariable(stringToUI64(secondArg.substr(1)));
		if ((firstArg[0] == NUM64 || firstArg[0] == NUM32 || firstArg[0] == NUM16 || firstArg[0] == NUMBYTE || firstArg[0] == CHARA) && (secondArg[0] == NUM64 || secondArg[0] == NUM32 || secondArg[0] == NUM16 || secondArg[0] == NUMBYTE || secondArg[0] == CHARA))
		{
			registers[COMMAND_REGISTER] = firstArg[0] + (UI64ToString(stringToUI64(firstArg.substr(1), firstArg.length() - 1) & stringToUI64(secondArg.substr(1), secondArg.length() - 1), firstArg[0] == NUM64 ? 8 : firstArg[0] == NUM32 ? 4 : firstArg[0] == NUM16 ? 2 : 1));
		}
		break;
	case 33://bitwise or
		if (firstArg[0] == REGISTER)
			firstArg = registers[firstArg[1]];
		if (firstArg[0] == VARNAME)
			firstArg = getVariable(stringToUI64(firstArg.substr(1)));
		secondArg = getFileVarVal(x);
		if (secondArg[0] == REGISTER)
			secondArg = registers[secondArg[1]];
		if (secondArg[0] == VARNAME)
			secondArg = getVariable(stringToUI64(secondArg.substr(1)));
		if ((firstArg[0] == NUM64 || firstArg[0] == NUM32 || firstArg[0] == NUM16 || firstArg[0] == NUMBYTE || firstArg[0] == CHARA) && (secondArg[0] == NUM64 || secondArg[0] == NUM32 || secondArg[0] == NUM16 || secondArg[0] == NUMBYTE || secondArg[0] == CHARA))
		{
			registers[COMMAND_REGISTER] = firstArg[0] + (UI64ToString(stringToUI64(firstArg.substr(1), firstArg.length() - 1) | stringToUI64(secondArg.substr(1), secondArg.length() - 1), firstArg[0] == NUM64 ? 8 : firstArg[0] == NUM32 ? 4 : firstArg[0] == NUM16 ? 2 : 1));
		}
		break;
	case 34://bitwise not
		if (firstArg[0] == REGISTER)
			firstArg = registers[firstArg[1]];
		if (firstArg[0] == VARNAME)
			firstArg = getVariable(stringToUI64(firstArg.substr(1)));
		if ((firstArg[0] == NUM64 || firstArg[0] == NUM32 || firstArg[0] == NUM16 || firstArg[0] == NUMBYTE || firstArg[0] == CHARA))
		{
			registers[COMMAND_REGISTER] = firstArg[0] + (UI64ToString(~stringToUI64(firstArg.substr(1), firstArg.length() - 1)));
		}
		break;
	case 35://charget
		//here we prepare our arguments
		if (firstArg[0] == REGISTER)
			firstArg = registers[firstArg[1]];
		if (firstArg[0] == VARNAME)
			firstArg = getVariable(stringToUI64(firstArg.substr(1)));
		secondArg = getFileVarVal(x);
		if (secondArg[0] == REGISTER)
			secondArg = registers[secondArg[1]];
		if (secondArg[0] == VARNAME)
			secondArg = getVariable(stringToUI64(secondArg.substr(1)));
		if (firstArg[0] == STRING && secondArg[0] == NUM64)
		{
			registers[COMMAND_REGISTER] = (char)CHARA + convertToProperStr(firstArg)[stringToUI64(secondArg.substr(1))];
		}
		else
		{
			fatalError(FORMAT_INVALID_PARAM_TYPE);
		}
		break;
	case 36://C++ based library call: makes standard library faster  Note: work here
		if (firstArg[0] == REGISTER)
			firstArg = registers[firstArg[1]];
		if (firstArg[0] != STRING && firstArg[0] != VARNAME)
			fatalError(FORMAT_INVALID_PARAM_TYPE);

		if (firstArg[0] == VARNAME)
			firstArg = getVariable(stringToUI64(firstArg.substr(1)));
		else
			firstArg = convertToProperStr(firstArg);

		if (functions.find(firstArg) != functions.end())
		{
			functions.at(firstArg)();
		}
		else
		{
			cout << "Error info: Attempted to call C++-based library function " << firstArg << "\n";
			fatalError(RUNTIME_NO_SUCH_FUNCTION);
		}
		break;
	case 37://do nothing, i.e. it's a no-op
		break;
	case 38://exit current event
		return -1;
	case 39://exit entire program peacefully
		file_info::programDone = true;
		return -1;
	case 40://strlen
		if (firstArg[0] == REGISTER)
			firstArg = registers[firstArg[1]];
		if (firstArg[0] == VARNAME)
			firstArg = getVariable(stringToUI64(firstArg.substr(1)));

		if (firstArg[0] == STRING)
		{
			registers[COMMAND_REGISTER] = (char)NUM64 + UI64ToString(firstArg.length() - 9); //not 8 since we still have a leftover header
		}
		else
		{
			fatalError(FORMAT_INVALID_PARAM_TYPE);
		}
		break;
	case 41://set array index
	{
		if (firstArg[0] != VARNAME && getVariableType(stringToUI64(firstArg.substr(1))) != ARRAY) //array
			fatalError(FORMAT_INVALID_PARAM_TYPE);
		string arrayStructure = getVariable(stringToUI64(firstArg.substr(1)));
		secondArg = getFileVarVal(x);
		if (secondArg[0] == REGISTER)
		{
			secondArg = registers[secondArg[1]];
		}
		if (secondArg[0] == VARNAME)
		{
			secondArg = getVariable(stringToUI64(secondArg.substr(1)));
		}
		if (secondArg[0] != NUM64) //set as
			fatalError(FORMAT_INVALID_PARAM_TYPE);

		longer index = stringToUI64(secondArg.substr(1));

		string thirdArg = getFileVarVal(x);
		if (thirdArg[0] == REGISTER)
		{
			thirdArg = registers[thirdArg[1]];
		}
		if (thirdArg[0] == VARNAME)
		{
			thirdArg = getVariable(stringToUI64(thirdArg.substr(1)));
		}

		string itemToBeChanged = getArrayIndex(arrayStructure, index);
		if (itemToBeChanged[0] == vartypes::OBJECT)
		{
			delete reinterpret_cast<Object*>(stringToUI64(itemToBeChanged.substr(1, 8)));
		}
		else
			delete[] getArrayIndexAddress(arrayStructure, index);
		char* newData = createVariable(thirdArg);

		string newDataPointer = UI64ToString(reinterpret_cast<intptr_t>(newData));

		for (int i = 0; i < 8; i++)//rewrites the pointer, needs (index+1) because the 0th position is the array length
		{
			arrayStructure.at(i + (index + 1) * 8 + 1) = newDataPointer[i];
		}
		createOrAssignVariable(stringToUI64(firstArg.substr(1)), arrayStructure);

		break;
	}
	case 42://push
	{
		if (firstArg[0] != VARNAME && getVariableType(stringToUI64(firstArg.substr(1))) != ARRAY)
			fatalError(FORMAT_INVALID_PARAM_TYPE);

		secondArg = getFileVarVal(x);//no type limitations
		if (secondArg[0] == REGISTER)
		{
			secondArg = registers[secondArg[1]];
		}
		if (secondArg[0] == VARNAME)
		{
			secondArg = getVariable(stringToUI64(secondArg.substr(1)));
		}

		auto arrayStructure = getVariable(stringToUI64(firstArg.substr(1))).substr(8);

		longer length = stringToUI64(arrayStructure.substr(1, 8));
		length++;

		char* newElement = createVariable(secondArg);

		string newElementPointer = UI64ToString(reinterpret_cast<intptr_t>(newElement));
		arrayStructure += newElementPointer;

		string lengthAsString = UI64ToString(length);
		arrayStructure = lengthAsString + arrayStructure;
		createOrAssignVariable(stringToUI64(firstArg.substr(1)), arrayStructure);

		break;
	}
	case 43://pop
	{
		if (firstArg[0] != VARNAME && getVariableType(stringToUI64(firstArg.substr(1))) != ARRAY)
			fatalError(FORMAT_INVALID_PARAM_TYPE);

		auto arrayStructure = getVariable(stringToUI64(firstArg.substr(1)));

		//delete the variable
		string itemToBeRemoved = getArrayIndex(arrayStructure, stringToUI64(arrayStructure.substr(1, 8)) - 1);
		if (itemToBeRemoved[0] == vartypes::OBJECT)
		{
			delete reinterpret_cast<Object*>(stringToUI64(itemToBeRemoved.substr(1, 8)));
		}
		else
		{
			delete[] getArrayIndexAddress(arrayStructure, stringToUI64(arrayStructure.substr(1, 8)) - 1);
		}

		arrayStructure = arrayStructure.substr(0, arrayStructure.length() - 8);

		longer length = stringToUI64(arrayStructure.substr(1, 8));
		length--;

		string lengthAsString = UI64ToString(length);
		for (int i = 0; i < 8; i++)
		{
			arrayStructure[i] = lengthAsString[i];
		}
		createOrAssignVariable(stringToUI64(firstArg.substr(1)), arrayStructure);
		break;
	}
	case 44://jif
		secondArg = getFileVarVal(x);
		if (secondArg[0] == REGISTER)
			secondArg = registers[secondArg[1]];
		if (secondArg[0] == VARNAME)
			secondArg = getVariable(stringToUI64(secondArg.substr(1)));
		if (secondArg[0] != BOOLE)
		{
			fatalError(RUNTIME_INVALID_PARAM_TYPE);
		}
		if (secondArg[1] != '\0')
		{
			if (firstArg[0] != NUM64)
				fatalError(FORMAT_INVALID_PARAM_TYPE);
			x->seekg(stringToUI64(firstArg.substr(1)));
		}
		break;
	case 45://jnif
		secondArg = getFileVarVal(x);
		if (secondArg[0] == REGISTER)
			secondArg = registers[secondArg[1]];
		if (secondArg[0] == VARNAME)
			secondArg = getVariable(stringToUI64(secondArg.substr(1)));
		if (secondArg[0] != BOOLE)
		{
			fatalError(RUNTIME_INVALID_PARAM_TYPE);
		}
		if (secondArg[1] == '\0')
		{
			if (firstArg[0] != NUM64)
				fatalError(FORMAT_INVALID_PARAM_TYPE);
			x->seekg(stringToUI64(firstArg.substr(1)));
		}
		break;
	case 46://throw
		if (firstArg[0] != STRING && firstArg[0] != REGISTER && firstArg[0] != VARNAME)
		{
			fatalError(FORMAT_INVALID_PARAM_TYPE);
		}
		if (firstArg[0] == REGISTER)
		{
			firstArg = registers[firstArg[1]];
		}
		if (firstArg[0] == VARNAME)
		{
			firstArg = getVariable(stringToUI64(firstArg.substr(1)));
		}
		if (firstArg[0] != STRING)
		{
			fatalError(RUNTIME_INVALID_PARAM_TYPE);
		}

		error_info::errorFlag = firstArg;
		break;
	case 47://cmpeq (Note: Cmp needs to be run before commands 47-52 will be useful)
		if (firstArg[0] == VARNAME)
		{
			createOrAssignVariable(stringToUI64(firstArg.substr(1)), string() + (char)BOOLE + (char)(cmp == 0));
		}
		else if (firstArg[0] == REGISTER)
		{
			registers[firstArg[1]] = string() + (char)BOOLE + (char)(cmp == 0);
		}
		else
		{
			fatalError(FORMAT_INVALID_PARAM_TYPE);
		}
		break;
	case 48://cmpls
		if (firstArg[0] == VARNAME)
		{
			createOrAssignVariable(stringToUI64(firstArg.substr(1)), string() + (char)BOOLE + (char)(cmp == -1));
		}
		else if (firstArg[0] == REGISTER)
		{
			registers[firstArg[1]] = string() + (char)BOOLE + (char)(cmp == -1);
		}
		else
		{
			fatalError(FORMAT_INVALID_PARAM_TYPE);
		}
		break;
	case 49://cmpgr
		if (firstArg[0] == VARNAME)
		{
			createOrAssignVariable(stringToUI64(firstArg.substr(1)), string() + (char)BOOLE + (char)(cmp == 1));
		}
		else if (firstArg[0] == REGISTER)
		{
			registers[firstArg[1]] = string() + (char)BOOLE + (char)(cmp == 1);
		}
		else
		{
			fatalError(FORMAT_INVALID_PARAM_TYPE);
		}
		break;
	case 50://cmpneq
		if (firstArg[0] == VARNAME)
		{
			createOrAssignVariable(stringToUI64(firstArg.substr(1)), string() + (char)BOOLE + (char)(cmp != 0));
		}
		else if (firstArg[0] == REGISTER)
		{
			registers[firstArg[1]] = string() + (char)BOOLE + (char)(cmp != 0);
		}
		else
		{
			fatalError(FORMAT_INVALID_PARAM_TYPE);
		}
		break;
	case 51://cmpgre
		if (firstArg[0] == VARNAME)
		{
			createOrAssignVariable(stringToUI64(firstArg.substr(1)), string() + (char)BOOLE + (char)(cmp >= 0));
		}
		else if (firstArg[0] == REGISTER)
		{
			registers[firstArg[1]] = string() + (char)BOOLE + (char)(cmp >= 0);
		}
		else
		{
			fatalError(FORMAT_INVALID_PARAM_TYPE);
		}
		break;
	case 52://cmplse
		if (firstArg[0] == VARNAME)
		{
			createOrAssignVariable(stringToUI64(firstArg.substr(1)), string() + (char)BOOLE + (char)(cmp <= 0));
		}
		else if (firstArg[0] == REGISTER)
		{
			registers[firstArg[1]] = string() + (char)BOOLE + (char)(cmp <= 0);
		}
		else
		{
			fatalError(FORMAT_INVALID_PARAM_TYPE);
		}
		break;
	case 53://catch marker
		if (firstArg[0] != NUM64)
		{
			fatalError(FORMAT_INVALID_PARAM_TYPE);
		}

		error_info::runtimeCatchJumpSpotTransfer = stringToUI64(firstArg.substr(1));
		break;
	case 54: //set object property
	{
		//firstArg = source
		if (firstArg[0] != REGISTER && firstArg[0] != VARNAME)
		{
			fatalError(FORMAT_INVALID_PARAM_TYPE);
		}
		else if ((firstArg[0] == REGISTER && registers[firstArg[1]][0] != OBJECT) || (firstArg[0] == VARNAME && getVariable(stringToUI64(firstArg.substr(1)))[0] != OBJECT))
		{
			fatalError(RUNTIME_INVALID_PARAM_TYPE);
		}

		Object* objSource;

		if (firstArg[0] == VARNAME)//conversion from firstArg to objSource
		{
			objSource = retrieveObjectFromString(getVariable(stringToUI64(firstArg.substr(1))));
		}
		else
		{
			objSource = retrieveObjectFromString(registers[firstArg[1]].substr(1));
		}

		secondArg = getFileVarVal(x);

		if ((secondArg[0] != VARNAME && secondArg[0] != REGISTER) && secondArg[0] != STRING)//prop
		{
			fatalError(FORMAT_INVALID_PARAM_TYPE);
		}
		else if (!((secondArg[0] == VARNAME && getVariable(stringToUI64(secondArg.substr(1)))[0] == STRING) || (secondArg[0] == REGISTER && registers[secondArg[1]][0] == STRING)) && secondArg[0] != STRING)
		{
			fatalError(RUNTIME_INVALID_PARAM_TYPE);
		}

		string propertyName; //property name
		if (secondArg[0] == VARNAME)
		{
			propertyName = convertToProperStr(getVariable(stringToUI64(secondArg.substr(1))));
		}
		else if (secondArg[0] == REGISTER)
		{
			propertyName = convertToProperStr(registers[secondArg[1]]);
		}
		else
		{
			propertyName = convertToProperStr(secondArg);
		}

		string thirdArg;
		thirdArg = getFileVarVal(x);

		setObjectProperty(objSource, propertyName, thirdArg);
		break;
	}
	case 55: //create object
		if (firstArg[0] != REGISTER && firstArg[0] != VARNAME)
		{
			fatalError(FORMAT_INVALID_PARAM_TYPE);
		}

		if(firstArg[0]==VARNAME)
			createVariable(stringToUI64(firstArg.substr(1)), convertObjectToString(createObject()));
		
		if (firstArg[0] == REGISTER)
			registers[firstArg[1]] = convertObjectToString(createObject());
		break;
	case 56: //delete object property
	{
		//firstArg = source
		if (firstArg[0] != REGISTER && firstArg[0] != VARNAME)
		{
			fatalError(FORMAT_INVALID_PARAM_TYPE);
		}
		else if ((firstArg[0] == REGISTER && registers[firstArg[1]][0] != OBJECT) || (firstArg[0] == VARNAME && getVariable(stringToUI64(firstArg.substr(1)))[0] != OBJECT))
		{
			fatalError(RUNTIME_INVALID_PARAM_TYPE);
		}

		Object* objSource;

		if (firstArg[0] == VARNAME)//conversion from firstArg to objSource
		{
			objSource = retrieveObjectFromString(getVariable(stringToUI64(firstArg.substr(1))));
		}
		else
		{
			objSource = retrieveObjectFromString(registers[firstArg[1]].substr(1));
		}

		secondArg = getFileVarVal(x);

		if ((secondArg[0] != VARNAME && secondArg[0] != REGISTER) && secondArg[0] != STRING)//prop
		{
			fatalError(FORMAT_INVALID_PARAM_TYPE);
		}
		else if (!((secondArg[0] == VARNAME && getVariable(stringToUI64(secondArg.substr(1)))[0] == STRING) || (secondArg[0] == REGISTER && registers[secondArg[1]][0] == STRING)) && secondArg[0] != STRING)
		{
			fatalError(RUNTIME_INVALID_PARAM_TYPE);
		}

		string propertyName; //property name
		if (secondArg[0] == VARNAME)
		{
			propertyName = convertToProperStr(getVariable(stringToUI64(secondArg.substr(1))));
		}
		else if (secondArg[0] == REGISTER)
		{
			propertyName = convertToProperStr(registers[secondArg[1]]);
		}
		else
		{
			propertyName = convertToProperStr(secondArg);
		}

		deleteObjectProperty(objSource, propertyName);
		break;
	}
	/*
	execute function
	
	Function vartype format:
	-Filename to access from (STRING value, value of "" signifies local function)
	-Index to read at (NUM64 value, 8 bytes)
	*/
	
	case 57: 
	{
		if (firstArg[0] == VARNAME)
		{
			firstArg = getVariable(stringToUI64(firstArg.substr(1)));
		}
		if (firstArg[0] != FUNCTION) // it's reasonable to assume that we can't assign a variable to another variable
		{
			fatalError(FORMAT_INVALID_PARAM_TYPE);
		}

		string filename = convertToProperStr(firstArg);
		ulonger addr = stringToUI64(firstArg.substr(filename.length())) + 8;

		if (filename != "")
		{
			file.open(convertToProperStr(firstArg));
			if (!file.is_open())
			{
				fatalError(RUNTIME_FILE_NOT_FOUND);
			}

			flagstr = file_info::curFile;
			file_info::curFile = convertToProperStr(firstArg);
			std::copy(std::istreambuf_iterator<char>(file),
				std::istreambuf_iterator<char>(),
				std::ostreambuf_iterator<char>(code));

			if (runEventCodeAt(&code, addr) == -1)
			{
				return -1;
			}
			file_info::curFile = flagstr;
		}
		else
		{
			if (runEventCodeAt(&file_info::programCode, addr) == -1)
			{
				return -1;
			}
		}

		break;
	}

	case 58: // get string length
	{
		if (firstArg[0] == VARNAME) {
			firstArg = getVariable(stringToUI64(firstArg.substr(1)));
		}
		if (firstArg[0] == REGISTER) {
			firstArg = registers[firstArg[1]];
		}
		if (firstArg[0] != STRING) {
			fatalError(FORMAT_INVALID_PARAM_TYPE);
		}
		registers[COMMAND_REGISTER] = (char)NUM64 + getPrimitiveLength(firstArg);
	}

	case 59: // get type 
	{
		if (firstArg[0] == VARNAME) {
			firstArg = getVariable(stringToUI64(firstArg.substr(1)));
		}
		if (firstArg[0] == REGISTER) {
			firstArg = registers[firstArg[1]];
		}
		registers[COMMAND_REGISTER] = firstArg[0];
	}
	case 254: //load debug info
	{
		while (true)
		{
			if (GetKeyState(VK_ESCAPE) & 0x8000)
			{
				break;
			}
		}
	}

	default://it's not a valid command
		if (x->tellg() == -1)
		{
			cout << "i see";
			return 2;
		}
		cout << "Error info: "<< x->tellg() << " has invalid command number \"" << (int)commandType <<"\"\n";
		fatalError(FORMAT_INVALID_COMMAND_NAME);
		return -1;
	}

	return 0;
}

// runs actual code
int runEventCodeAt(stringstream* codePart, longer pointer, bool mainEvent)
{
	//saves the file reader's position
	longer eventReloadSpot = codePart->tellg();
	vector<longer> runtimeCatchJumpSpots{};
	vector<longer> savedRuntimeSpot{};

	bool catchMode = false;

	codePart->seekg(pointer, std::ios::beg);

	longer statementStatus = 0;
	try {
		while (statementStatus == 0)//main function running loop
		{
			statementStatus = runStatement(codePart);
			if (error_info::runtimeCatchJumpSpotTransfer != -1 &&
				std::find(runtimeCatchJumpSpots.begin(), runtimeCatchJumpSpots.end(), error_info::runtimeCatchJumpSpotTransfer) == runtimeCatchJumpSpots.end())
			{
				runtimeCatchJumpSpots.push_back(error_info::runtimeCatchJumpSpotTransfer);
				error_info::runtimeCatchJumpSpotTransfer = -1;
			}

			if (convertToProperStr(error_info::errorFlag) != "")
			{
				if (runtimeCatchJumpSpots.empty())
				{
					codePart->seekg(eventReloadSpot, std::ios::beg); //go back to the previous position
					return -1;
				}
				else
				{
					registers[ERROR_REGISTER] = error_info::errorFlag;
					error_info::errorFlag = convertToSaveableStr("");
					savedRuntimeSpot.push_back(codePart->tellg());
					codePart->seekg(runtimeCatchJumpSpots[runtimeCatchJumpSpots.size() - 1], std::ios::beg);
					runtimeCatchJumpSpots.pop_back();
					catchMode = true;
					continue;
				}
			}
			if (statementStatus == 1 && catchMode == true)
			{
				catchMode = false;
				codePart->seekg(savedRuntimeSpot.back(), std::ios::beg);
				savedRuntimeSpot.pop_back();
				continue;
			}
			else if (statementStatus == -1)
			{
				return -1;
			}
		}
	}
	catch (std::exception& x)
	{
		cout << "Error info: " << x.what() << "\n";
		fatalError(RUNTIME_ENVIRONMENT_EXCEPTION);
	}

	//return the file reader to the event loop
	codePart->seekg(eventReloadSpot, std::ios::beg);
	return 0;
}
//main function that runs tree
void runProgram(const char* fileName)
{
	file_info::filePath = fileName;

	//main file stream
	ifstream programFile{ fileName, std::ios::binary };
	if (!programFile.is_open())
	{
		fatalError(RUNTIME_FILE_NOT_FOUND);
	}

	std::copy(std::istreambuf_iterator<char>(programFile), //convert it to a stringstream, more versatile
		std::istreambuf_iterator<char>(),
		std::ostreambuf_iterator<char>(file_info::programCode));
	for (byte i = 0; i < 8; i++)
	{
		if (file_info::programCode.get()!="TREEWN64"[i])
		{
			fatalError(FORMAT_INVALID_SIGNATURE);
			return;
		}
	}
	programFile.close();
	//Setup threads


	//setup for program

	//initialize the standard library here
	initializeLibraries();


	//Start event (located directly after the signature)
	longer startEventPointer = getFile64Int(&file_info::programCode);
	
	if (file_info::programCode.str().length() <= startEventPointer)
	{
		fatalError(FORMAT_EVENT_OUT_OF_BOUNDS);
	}

	runEventCodeAt(&file_info::programCode, startEventPointer, true);
	
	while (!file_info::programDone)
	{
		/*for (int i = 0; i < threads.size(); i++)
		{
			if (!threads[i].joinable())
			{
				threads.erase(threads.begin() + i);
				threadsRunning.erase(threadsRunning.begin() + i);
				i--;
				cout << "erased\n";
			}
		}*/

		byte eventName = getFileByteInt(&file_info::programCode);
		switch (eventName)
		{
			case 0:
			{
				ulonger pointer = getFile64Int(&file_info::programCode);
				if (file_info::programCode.str().length() <= pointer)
				{
					fatalError(FORMAT_EVENT_OUT_OF_BOUNDS);
				}

				runEventCodeAt(&file_info::programCode, pointer);
				break;
			}
			case 255: 
				file_info::programCode.seekg(16, std::ios::beg);
				continue;
		}
	}
	
}
//entry point for the program
int main(int argc, char** argv)
{
	//so we don't have a warning when compiled, does nothing
	argc;

	//Any function tests will go here

	//Sets up the name of the application
	SetConsoleTitle(TEXT("Tree Runtime Environment v1.0"));

	if (!argv[1])
	{
		MessageBoxA(nullptr, "No input given to runtime", "Tree Runtime Environment", 0);
		exit(0);
	}

	file_info::curFile = argv[1];
	const char* name = argv[1];
	runProgram(name);


	return 0;
}