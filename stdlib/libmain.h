#pragma once


#include "std_array.h"
#include "std_string.h"
#include "std_io.h"
#include "std_files.h"
#include "std_time.h"
#include "std_primitive.h"


//for sake of brevity, just do this
#define DECLARE_STD_METHOD(x) functions.insert_or_assign(#x, x);

std::map<std::string, std::function<void()>> functions;

void initializeLibraries()
{
	// Initializes std_io.h
	DECLARE_STD_METHOD(io_getNextLine)
	DECLARE_STD_METHOD(program_pause)
	DECLARE_STD_METHOD(io_clearConsole)
	DECLARE_STD_METHOD(io_getNextChar)
	DECLARE_STD_METHOD(io_cout)

	// Initializes std_array.h
	DECLARE_STD_METHOD(array_insert)
	DECLARE_STD_METHOD(array_unshift)
	DECLARE_STD_METHOD(array_shift)

	// Initializes std_string.h
	DECLARE_STD_METHOD(string_substr)
	DECLARE_STD_METHOD(string_toString)
	DECLARE_STD_METHOD(string_indexOf)

	// Initializes std_files.h
	DECLARE_STD_METHOD(file_openFile)
	DECLARE_STD_METHOD(file_getFileContent)
	DECLARE_STD_METHOD(file_getSize)
	DECLARE_STD_METHOD(file_closeFile)

	// Initializes std_time.h
	DECLARE_STD_METHOD(time_unix)
	DECLARE_STD_METHOD(time_sec)
	DECLARE_STD_METHOD(time_min)
	DECLARE_STD_METHOD(time_hour)
	DECLARE_STD_METHOD(time_wday)
	DECLARE_STD_METHOD(time_month)
	DECLARE_STD_METHOD(time_year)

    // Initializes std_primitive.h
	DECLARE_STD_METHOD(primitive_str_to_num)
	DECLARE_STD_METHOD(primitive_sqrt)
}