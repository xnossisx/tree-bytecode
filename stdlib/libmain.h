#pragma once
#include "stddeps.h"
#include "std_array.h"
#include "std_string.h"
#include "std_io.h"
#include "std_files.h"


//for sake of brevity, just do this
#define DECLARE_STD_METHOD(x) functions.insert_or_assign(#x, x);

std::map<std::string, std::function<void()>> functions;

void initializeLibraries()
{
	//Initializes std_io.cpp
	DECLARE_STD_METHOD(io_getNextLine)
	DECLARE_STD_METHOD(program_pause)
	DECLARE_STD_METHOD(io_clearConsole)
	DECLARE_STD_METHOD(io_getNextChar)
	DECLARE_STD_METHOD(io_cout)

	//Initializes std_array.cpp
	DECLARE_STD_METHOD(array_insert)
	DECLARE_STD_METHOD(array_unshift)
	DECLARE_STD_METHOD(array_shift)

	//Initializes std_string.cpp
	DECLARE_STD_METHOD(string_substr)
	DECLARE_STD_METHOD(toString)
}