export module error;

//all Tree Errors
export enum treeError {

	//Used as a null error, isn't really an error (Error code: 0)
	ERROR_TREESUCCESS,

	//A format error occurs when the interpreter detects that some part of the program has been malformed.
	//These errors should only occur because of faulty compilers.

	//Error given when an parameter given is not within the restrictions of its usage within the command. (Error code: 1)
	FORMAT_INVALID_PARAM,
	//Error given when a parameter given has an incorrect type.(Error code: 2)
	FORMAT_INVALID_PARAM_TYPE,
	//Error given when the signature at the beginning of the file (TREEWN64) is nonexistent or corrupted. (Error code: 3)
	FORMAT_INVALID_SIGNATURE,
	//Error given when a command name is unrecognized. (Error code: 4)
	FORMAT_INVALID_COMMAND_NAME,
	//Error given when an invalid program structure is detected. (Error code: 5)
	FORMAT_INVALID_STRUCTURE,
	//Error given when a pointer to an event is out of file bounds. (Error code: 6)
	FORMAT_EVENT_OUT_OF_BOUNDS,

	//Runtime errors happen during execution, and are more likely to be caused by issues on the programmer's side.

	//Error code: 7
	RUNTIME_INVALID_VARIABLE_NAME,
	//Occurs when a variable has an improper type when used in a command. (Error code: 8)
	RUNTIME_INVALID_PARAM_TYPE,
	//Occurs when an attempt at opening a file has failed. (Error code: 9)
	RUNTIME_FILE_NOT_FOUND,
	//Given when an attempt at obtaining a variable has failed. (Error code: 10)
	RUNTIME_NO_SUCH_VARIABLE,
	//Given when an array being read from file becomes too large, as to protect against memory leaks/crashes. (Error code: 11)
	RUNTIME_ARRAY_OVERFLOW,
	//Given when the given standard function does not exist. (Error code: 12)
	RUNTIME_NO_SUCH_FUNCTION,
	//Given when the program environment itself throws an error for an unknown reason. (Error code: 13)
	RUNTIME_ENVIRONMENT_EXCEPTION,
	//Given when the application throws an uncaught exception. (Error code: 14)
	RUNTIME_APPLICATION_EXCEPTION,
	//Given when the application cannot interpret the string correctly. (Error code: 15)
	RUNTIME_STRING_FORMAT_EXCEPTION,

};

export const char* errorNames[] = {
	"ERROR_TREESUCCESS",
	"FORMAT_INVALID_PARAM",
	"FORMAT_INVALID_PARAM_TYPE",
	"FORMAT_INVALID_SIGNATURE",
	"FORMAT_INVALID_COMMAND_NAME",
	"FORMAT_INVALID_STRUCTURE",
	"FORMAT_EVENT_OUT_OF_BOUNDS",
	"RUNTIME_INVALID_VARIABLE_NAME",
	"RUNTIME_INVALID_PARAM_TYPE",
	"RUNTIME_FILE_NOT_FOUND",
	"RUNTIME_NO_SUCH_VARIABLE",
	"RUNTIME_ARRAY_OVERFLOW",
	"RUNTIME_NO_SUCH_FUNCTION",
	"RUNTIME_ENVIRONMENT_EXCEPTION",
	"RUNTIME_APPLICATION_EXCEPTION"
};

//Function used to report such instances of errors
export void fatalError(treeError error);