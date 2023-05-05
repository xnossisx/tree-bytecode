#include "stddeps.h"
#include "treeEx.h"

export module utils;
import error;

/* Vital variables and functions in tree*/

export inline std::map<longer, dpointer_size_pair> varInfo{};

export inline std::vector<std::thread> threads{};
export inline std::vector<std::string> threadsRunning{};

export std::vector<std::string> stackVars{};

export namespace file_info {
	string filePath; //stores the file path of the program
	string curFile;//the path of the current file
	stringstream programCode;//the stringstream of the file
}

export std::string registers[256];

export void printStrasInt(string x);

/* Flags */

export namespace error_info {
	data_block errorFlag; //contains the thrown error data
	longer runtimeCatchJumpSpotTransfer; //contains the position of the current functioning catch code
}

//Numerical Definitions

export auto RETURN_REGISTER = 127;
export auto ARGUMENT_VARIABLE = ULLONG_MAX;
export auto COMMAND_REGISTER = 255;
export auto ERROR_REGISTER = 128;


//In Tree, types are explicitly defined in the source
export enum vartypes {
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
string UI64ToString(ulonger x, size_t trunc = 8);
template<typename T> string numericalToString(T num);

//does the opposite
ulonger stringToUI64(string x, size_t size = 8);

template<typename T> T stringToNumerical(string x, size_t size = 8);
template double stringToNumerical<double>(string x, size_t size);

//converts a string to a float
double stringToFloat(string x);

//converts a accessable string to a useable string (uses the front signature, if you remove it, this will break!!)
string convertToProperStr(string x);

//turns a string into one that can be read safely
export string convertToSaveableStr(string x)
{
	return '\5' + UI64ToString(x.length()) + x;
}

export template <typename T>
T stringToNumerical(string x, size_t size)
{
	union strnum {
		char str[sizeof(T)];
		T num;
	};
	strnum gen = {};
	std::memcpy(gen.str, x.append(size - x.length(), '\0').substr(0, sizeof(T)).c_str(), sizeof(T)); //initialize str.str so that it can be converted

	return gen.num;
}

export string UI64ToString(ulonger x, size_t trunc) {
	union strnum {
		char str[8];
		ulonger num;
	} number;
	number.num = x;
	return string(number.str, min(trunc, 8));
}

export template<typename T>
string numericalToString(T x) {
	union strnum {
		char str[sizeof(T)];
		T num;
	} number;
	number.num = x;
	return string(number.str, sizeof(T));
}

export ulonger stringToUI64(string x, size_t size)
{
	return stringToNumerical<ulonger>(x, size);
}

export double stringToFloat(string x)
{
	return stringToNumerical<double>(x);
}

// Converts a string with a header to the underlying string
export string convertToProperStr(string x)
{
	if (x[0] != STRING || x.length() == 0) //jump out to avoid access violation
		return "";

	//PAGE_FAULT_IN_NONPAGED_AREA: Never forget
	return x.substr(9, 8 + stringToUI64(x.substr(1, 8)));
}

/* Takes the direct value of a vartypes::STRING or vartypes::ARRAY, returns its length 
   
   @@value: A type-augmented value*/
export ulonger getPrimitiveLength(string value) {
	return stringToUI64(value.substr(1, 8));
}

export void createVariable(ulonger name, data_block x)
{
	char* dpointer = new char[x.length()];

	for (int i = 0; i < x.length(); i++)
	{
		dpointer[i] = x[i];
	}

	varInfo.insert_or_assign(name, dpointer_size_pair(dpointer, x.length()));
}

export char* createVariable(string x)
{
	char* dpointer = new char[x.length()];

	for (int i = 0; i < x.length(); i++)
	{
		dpointer[i] = x[i];
	}

	return dpointer;
}

export bool isThereVariable(longer name)
{
	return varInfo.find(name) != varInfo.end();
}

export string getVariable(ulonger name)
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

export string getVarViaAddress(char* dpointer)
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
			y += (dpointer[bytePointer + i] * (ulonger)pow(256, i));
		}
		z.append(UI64ToString(y));
		for (longer i = 0; i < y; i++)
		{
			z.push_back(dpointer[bytePointer + i + 8]);
		}
		break;
	case vartypes::ARRAY:
		z += UI64ToString(bytePointer);
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

export char* getAddress(longer name)
{
	return isThereVariable(name) ? (varInfo.at(name).first) : nullptr;
}

export string convertToPrintableStr(string x)
{
	if (x[0] == REGISTER)
	{
		x = registers[stringToUI64(x.substr(1))];
	}
	if (x[0] == VARNAME)
	{
		x = getVariable(stringToUI64(x.substr(1)));
	}

	char typeName = x[0];
	switch (typeName)
	{
	case STRING:
		return convertToProperStr(x);
	case CHARA:
		return std::to_string(x.at(1));
	case NUM64:
		return std::to_string((longer)stringToUI64(x.substr(1)));
	case NUM32:
		return std::to_string((longer)stringToUI64(x.substr(1), 4));
	case NUM16:
		return std::to_string((longer)stringToUI64(x.substr(1), 2));
	case NUMBYTE:
		return std::to_string((longer)stringToUI64(x.substr(1), 1));
	case NUMFLOAT:
		return std::to_string(stringToFloat(x.substr(1)));
	case UNDEFINED:
		return "undefined";
	case ARRAY:
		return "Array[" + std::to_string(stringToUI64(x.substr(1, 8))) + "]";
	case OBJECT:
		return "Object";
	case BOOLE:
		return x[1] ? "true" : "false";
	case FUNCTION:
		return "Function";
	case REGISTER:
		return convertToPrintableStr(registers[x[1]]);
	case VARNAME:
		return convertToPrintableStr(getVariable(stringToUI64(x.substr(1, 8))));
	default:
		fatalError(RUNTIME_INVALID_PARAM_TYPE);//we've gone through all types
	}
}

//Allocators

void createVariable(ulonger name, data_block x);

char* createVariable(string x);

//getter
string getVariable(ulonger name);

//getter (works with addresses)
string getVarViaAddress(char* dpointer);

char* getAddress(longer name);

export char getVariableType(longer name)
{
	return getVariable(name)[0];
}

export char getVariableType(char* dpointer)
{
	return dpointer[0]; //debatable whether this is smart or not, but it runs in constant time... ¯\_(ツ)_/¯
}

//deallocator

export void deleteVariable(longer name)
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
export void createOrAssignVariable(longer name, data_block x)
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
export char* assignVariable(char* dpointer, data_block x)
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
bool isThereVariable(longer name);

//gives item directly
export string getArrayIndex(longer name, longer index)
{
	string array = getVariable(name);
	string y;
	longer posOfPointer = (index + 1) * 8;

	for (int i = 0; i < 8; i++)
	{
		y += array[posOfPointer + 1 + i];
	}

	return getVarViaAddress(reinterpret_cast<char*>(stringToUI64(y)));
}

export string getArrayIndex(string array, longer index)
{
	string y;
	longer posOfPointer = (index + 1) * 8;

	for (int i = 0; i < 8; i++)
	{
		y += array[posOfPointer + 1 + i];
	}
	return getVarViaAddress(reinterpret_cast<char*>(stringToUI64(y)));
}

export char* getArrayIndexAddress(longer name, longer index)
{
	string array = getVariable(name);
	string y;
	longer posOfPointer = (index + 1) * 8;

	for (int i = 0; i < 8; i++)
	{
		y += array[posOfPointer + 1 + i];
	}

	return reinterpret_cast<char*>(stringToUI64(y));
}

export char* getArrayIndexAddress(string array, longer index)
{
	string y;
	longer posOfPointer = (index + 1) * 8;

	for (int i = 0; i < 8; i++)
	{
		y += array[posOfPointer + 1 + i];
	}

	return reinterpret_cast<char*>(stringToUI64(y));
}

export struct Object {
public:
	char marker[10];
	std::map<std::string, std::string> data;

	Object()
	{
		strcpy_s(marker, ((char)vartypes::OBJECT + UI64ToString(reinterpret_cast<intptr_t>(&data))).c_str());
	}

	~Object()
	{
		data.clear();
	}
};

export Object* createObject()
{
	return new Object();
}

export void setObjectProperty(Object* obj, string property, string value)
{
	obj->data.insert_or_assign(property, value);
}

export string getObjectProperty(Object* obj, string property)
{
	try {
		return obj->data.at(property);
	}
	catch (std::out_of_range)
	{
		return "";
	}
}

export void deleteObjectProperty(Object* obj, string property)
{
	obj->data.erase(property);
}

export Object* convertNameToObject(char* dpointer)
{
	return reinterpret_cast<Object*>(dpointer);
}

export string convertObjectToString(Object* obj)
{
	return (char)OBJECT + UI64ToString(reinterpret_cast<longer>(obj));
}

//requires the header
export Object* retrieveObjectFromString(string str)
{
	return reinterpret_cast<Object*>(stringToUI64(str.substr(1)));
}

export data_block retrieveValue(data_block thing) // recursively checks for value references
{
	beg:
	if (thing.at(0) == VARNAME)
	{
		thing = getVariable(stringToUI64(thing.substr(1, 8)));
		goto beg; // This may be ugly, but it should be faster than a while loop
	}
	if (thing.at(0) == REGISTER)
	{
		thing = registers[thing.at(1)];
		goto beg;
	}
	return thing;
}

export void generateEventThread(stringstream* x, longer pointer)
{
	for (int i = 0; i < threadsRunning.size(); i++)
	{
		if (UI64ToString(pointer) == threadsRunning[i])
		{
			return;
		}
	}

	if (threads.size() < 4)
	{
		threadsRunning.push_back(UI64ToString(pointer));

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

		while (threadLock) {}
	}
	else
	{
		std::cout << "Out of threads!";
	}
}

//turns a variable value into something that you can print
string convertToPrintableStr(string x);
