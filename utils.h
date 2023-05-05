#pragma once
#include "stddeps.h"
import error;

typedef long long longer;
typedef unsigned long long ulonger;
typedef std::pair<longer, ulonger> pointer_size_pair;
typedef std::pair<char*, ulonger> dpointer_size_pair;
typedef unsigned char byte; 
typedef std::string data_block;

using std::cout; using std::string; using std::ifstream; using std::cin; using std::vector; using std::stringstream;


/* Vital variables and functions in tree*/

inline std::map<longer, dpointer_size_pair> varInfo{};

inline std::vector<std::thread> threads{};
inline std::vector<std::string> threadsRunning{};

inline std::vector<std::string> stackVars{};

namespace file_info {
	extern string filePath; //stores the file path of the program
	extern string curFile;//the path of the current file
	extern stringstream programCode;//the stringstream of the file
}

inline std::string registers[256];

void printStrasInt(string x);

extern int runEventCodeAt(stringstream* codePart, longer pointer, bool mainEvent = false);

/* Flags */

namespace error_info {
	extern data_block errorFlag; //contains the thrown error data
	extern longer runtimeCatchJumpSpotTransfer; //contains the position of the current functioning catch code
}

//Numerical Definitions

constexpr auto RETURN_REGISTER = 127;
constexpr auto ARGUMENT_VARIABLE = ULLONG_MAX;
constexpr auto COMMAND_REGISTER = 255;
constexpr auto ERROR_REGISTER = 128;


//In Tree, types are explicitly defined in the source
enum vartypes {
	NUM64, //Stored using 8 bytes
	NUM32, //Stored using 4 bytes
	NUM16, //Stored using 2 bytes
	NUMBYTE, //Stored using a byte
	CHARA, //Stored with a byte, but treated as a character
	STRING, //Strings contain length in 8 bytes, and then the string block
	BOOLE, //Contains boolean information
	ARRAY, //Uses a spider-style array format
	UNDEFINED, //Contains no data
	NUMFLOAT, //Contains a floating-point number (8 bytes)
	REGISTER, //Reference to a register
	VARNAME, //Reference to a variable
	OBJECT, //JS-style object
	FUNCTION,
};

//converts a longer to a string
extern string longLongToString(ulonger x, int trunc = 8);

//does the opposite
extern ulonger stringToLongLong(string x, size_t size = 8);

template<typename T> T stringToNumerical(string x, size_t size = 8);
template double stringToNumerical<double>(string x, size_t size);

//converts a string to a float
extern double stringToFloat(string x);

//converts a accessable string to a useable string (uses the front signature, if you remove it, this will break!!)
extern string convertToProperStr(string x);

//turns a string into one that can be read safely
static string convertToSaveableStr(string x)
{
	return '\5' + longLongToString(x.length(), 8) + x;
}

template <typename T>
static T stringToNumerical(string x, size_t size)
{
	union strnum {
		char str[sizeof T];
		T num;
	} str;

	std::memcpy(str.str, x.append(size - x.length(), '\0').substr(0, sizeof T).c_str(), sizeof T); //initialize str.str so that it can be converted

	return str.num;
}

static string longLongToString(ulonger x, int trunc) {
	union strnum {
		char str[8];
		ulonger num;
	} number;
	number.num = x;
	return string(number.str, 8);
}

static ulonger stringToLongLong(string x, size_t size)
{
	return stringToNumerical<ulonger>(x, size);
}

static double stringToFloat(string x)
{
	return stringToNumerical<double>(x);
}

static string convertToProperStr(string x)
{
	if (x[0] != STRING || x.length() == 0) //jump out to avoid access violation
		return "";

	//PAGE_FAULT_IN_NONPAGED_AREA: Never forget
	return x.substr(9, 8 + stringToLongLong(x.substr(1, 8)));
}

static void createVariable(ulonger name, data_block x)
{
	char* dpointer = new char[x.length()];

	for (int i = 0; i < x.length(); i++)
	{
		dpointer[i] = x[i];
	}

	varInfo.insert_or_assign(name, dpointer_size_pair(dpointer, x.length()));
}

static char* createVariable(string x)
{
	char* dpointer = new char[x.length()];

	for (int i = 0; i < x.length(); i++)
	{
		dpointer[i] = x[i];
	}

	return dpointer;
}

static bool isThereVariable(longer name)
{
	return varInfo.find(name) != varInfo.end();
}

static string getVariable(ulonger name)
{
	string ret;

	if (!isThereVariable(name))
		return "" + (char)vartypes::UNDEFINED;

	char* dpointer = varInfo.at(name).first;

	ret.resize(varInfo.at(name).second);
	for (int i = 0; i < varInfo.at(name).second; i++)
	{
		ret[i] = dpointer[i];
	}

	return ret;
}

static string getVarViaAddress(char* dpointer)
{
	longer bytePointer = 0;
	longer y;
	string z;//returned value
	char typ = dpointer[bytePointer];
	z += typ;
	bytePointer++;
	switch (typ)//a bunch of cases for each value type
	{
	case vartypes::STRING:
		y = 0;
		for (byte i = 0; i < 8; i++)
		{
			y += (dpointer[bytePointer + i] * pow(256, i));
		}
		z.append(longLongToString(y));
		for (longer i = 0; i < y; i++)
		{
			z.push_back(dpointer[bytePointer + i + 8]);
		}
		break;
	case vartypes::ARRAY:
		z += longLongToString(bytePointer);
		break;
	case vartypes::VARNAME:
	case vartypes::NUM64:
		for (byte i = 0; i < 8; i++)
		{
			z += dpointer[bytePointer + i];
		}
		break;
	case vartypes::NUM32:
		for (byte i = 0; i < 4; i++)
		{
			z += dpointer[bytePointer + i];
		}
		break;
	case vartypes::NUM16:
		for (byte i = 0; i < 2; i++)
		{
			z += dpointer[bytePointer + i];
		}
		break;
	case vartypes::NUMBYTE:
		z += dpointer[bytePointer];
		break;
	case vartypes::BOOLE:
		z += dpointer[bytePointer];
		break;
	case vartypes::CHARA:
		z += dpointer[bytePointer];
		break;
	case vartypes::UNDEFINED://no need to add anything here
		break;
	case vartypes::OBJECT:
		for (byte i = 0; i < 8; i++)
		{
			z += dpointer[bytePointer + i];
		}
		break;
	default:
		fatalError(FORMAT_INVALID_PARAM_TYPE);//this error is pretty common
	}
	return z;
}

static char* getAddress(longer name)
{
	return isThereVariable(name) ? (varInfo.at(name).first) : nullptr;
}

static string convertToPrintableStr(string x)
{
	if (x[0] == REGISTER)
	{
		x = registers[stringToLongLong(x.substr(1))];
	}
	if (x[0] == VARNAME)
	{
		x = getVariable(stringToLongLong(x.substr(1)));
	}

	char typeName = x[0];
	switch (typeName)
	{
	case STRING:
		return convertToProperStr(x);
	case CHARA:
		return std::to_string(x.at(1));
	case NUM64:
		return std::to_string((longer)stringToLongLong(x.substr(1)));
	case NUM32:
		return std::to_string((longer)stringToLongLong(x.substr(1), 4));
	case NUM16:
		return std::to_string((longer)stringToLongLong(x.substr(1), 2));
	case NUMBYTE:
		return std::to_string((longer)stringToLongLong(x.substr(1), 1));
	case NUMFLOAT:
		return std::to_string(stringToFloat(x.substr(1)));
	case UNDEFINED:
		return "undefined";
	case ARRAY:
		return "Array[" + std::to_string(stringToLongLong(x.substr(1, 8))) + "]";
	case OBJECT:
		return "Object";
	case BOOLE:
		return x[1] ? "true" : "false";
	case FUNCTION:
		return "Function";
	case REGISTER:
		return convertToPrintableStr(registers[x[1]]);
	case VARNAME:
		return convertToPrintableStr(getVariable(stringToLongLong(x.substr(1, 8))));
	default:
		fatalError(RUNTIME_INVALID_PARAM_TYPE);//we've gone through all types
	}
}

static char getVariableType(longer name)
{
	return getVariable(name)[0];
}

static char getVariableType(char* dpointer)
{
	return dpointer[0]; //debatable whether this is smart or not, but it runs in constant time... ¯\_(ツ)_/¯
}

//deallocator

static void deleteVariable(longer name)
{
	try 
	{
		delete[] varInfo.at(name).first;
		
	}
	catch (...)
	{
		fatalError(RUNTIME_NO_SUCH_VARIABLE);
	}
}

//edit variable, deletes previous data stored in the variable
static void createOrAssignVariable(longer name, data_block x)
{
	//Create variable if it doesn't exist
	if (varInfo.find(name) == varInfo.end())
	{
		createVariable(name, x);
		return;
	}

	//Deallocate whatever was here before in the variable
	delete[] varInfo.at(name).first;

	//Allocate space for the new data
	char* newDpointer = new char[x.length()];

	//Input the new data into the list byte by byte
	memcpy_s(newDpointer, x.length(), x.data(), x.length());

	//Attach all information to the variable database
	varInfo.insert_or_assign(name, dpointer_size_pair(newDpointer, x.length()));
}

//edit anonymous variable (note: keep track of this in the container)
static char* assignVariable(char* dpointer, data_block x)
{
	//deallocate whatever was here before, stored in dpointer hopefully
	delete[] dpointer;

	//create new data
	char* newDpointer = new char[x.length()];

	memcpy_s(newDpointer, x.length(), x.data(), x.length());

	dpointer = newDpointer;

	return newDpointer;
}

//does variable x exist? if so, return true
extern bool isThereVariable(longer name);

//gives item directly
static string getArrayIndex(longer name, longer index)
{
	string array = getVariable(name);
	string y;
	longer posOfPointer = (index + 1) * 8;

	for (int i = 0; i < 8; i++)
	{
		y += array[posOfPointer + 1 + i];
	}

	return getVarViaAddress(reinterpret_cast<char*>(stringToLongLong(y)));
}

static string getArrayIndex(string array, longer index)
{
	string y;
	longer posOfPointer = (index + 1) * 8;

	for (int i = 0; i < 8; i++)
	{
		y += array[posOfPointer + 1 + i];
	}
	return getVarViaAddress(reinterpret_cast<char*>(stringToLongLong(y)));
}

static char* getArrayIndexAddress(longer name, longer index)
{
	string array = getVariable(name);
	string y;
	longer posOfPointer = (index + 1) * 8;

	for (int i = 0; i < 8; i++)
	{
		y += array[posOfPointer + 1 + i];
	}

	return reinterpret_cast<char*>(stringToLongLong(y));
}

static char* getArrayIndexAddress(string array, longer index)
{
	string y;
	longer posOfPointer = (index + 1) * 8;

	for (int i = 0; i < 8; i++)
	{
		y += array[posOfPointer + 1 + i];
	}

	return reinterpret_cast<char*>(stringToLongLong(y));
}

struct object {
public:
	char marker[10];
	std::map<std::string, std::string> data;

	object()
	{
		strcpy_s(marker, ((char)vartypes::OBJECT+longLongToString(reinterpret_cast<intptr_t>(&data))).c_str());
	}

	~object()
	{
		data.clear();
	}
};

static object* createObject()
{
	return new object();
}

static void setObjectProperty(object* obj, string property, string value)
{
	obj->data.insert_or_assign(property, value);
}

static string getObjectProperty(object* obj, string property)
{
	try {
		return obj->data.at(property);
	}
	catch (std::out_of_range)
	{
		return "";
	}
}

static void deleteObjectProperty(object* obj, string property)
{
	obj->data.erase(property);
}

static object* convertNameToObject(char* dpointer)
{
	return reinterpret_cast<object*>(dpointer);
}

static string convertObjectToString(object* obj)
{
	return (char)OBJECT + longLongToString(reinterpret_cast<longer>(obj));
}

//requires the header
static object* retrieveObjectFromString(string str)
{
	return reinterpret_cast<object*>(stringToLongLong(str.substr(1)));
}

static void generateEventThread(stringstream* x, longer pointer)
{
	for (int i = 0; i < threadsRunning.size(); i++)
	{
		if (longLongToString(pointer) == threadsRunning[i])
		{
			return;
		}
	}

	if (threads.size() < 4)
	{
		threadsRunning.push_back(longLongToString(pointer));

		std::atomic_bool threadLock = true; //Makes sure that the thread has copied over the stringstream successfully before handing back control

		threads.push_back(std::move(std::thread([&x, &pointer, &threadLock]()
			{
				stringstream tempProgramCode;

				int streamSaveSpot = x->tellg(); //Saves the spot of the main stream
				file_info::programCode.seekg(0, std::ios::beg);

				std::copy(std::istreambuf_iterator<char>(*x), // Transfer the stream
					std::istreambuf_iterator<char>(),
					std::ostreambuf_iterator<char>(tempProgramCode));
				file_info::programCode.seekg(streamSaveSpot, std::ios::beg);

				threadLock = false;

				runEventCodeAt(&tempProgramCode, pointer, true);

				for (int i = 0; i < threads.size(); i++)
				{
					if (threads[i].get_id() == std::this_thread::get_id())
					{
						threads[i].detach();
						cout << "detach";
						break;
					}
					cout << "hmmm";
				}
			})));

		while(threadLock) {}
	}
	else
	{
		std::cout << "Out of threads!";
	}
}

//turns a variable value into something that you can print
extern string convertToPrintableStr(string x);