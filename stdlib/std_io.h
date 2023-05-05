#pragma once
#include "stddeps.h"
import utils;


#include <conio.h> //unfortunately, we need this to get a single character

//Gets input from the user
void io_getNextLine()
{
	string value;

	std::getline(std::cin, value);

	registers[RETURN_REGISTER] = convertToSaveableStr(value);
}

//Gets a character from the user (do note: the chars outputted may be strange, especially for control characters, so be warned)
void io_getNextChar()
{
	string value;

	HANDLE hStdin = GetStdHandle(STD_INPUT_HANDLE);
	DWORD mode = 0;
	GetConsoleMode(hStdin, &mode);
	SetConsoleMode(hStdin, mode & (~ENABLE_ECHO_INPUT));

	value = _getch();
	if (value == "\r")
	{
		value = '\n';
	}

	SetConsoleMode(hStdin, mode & (ENABLE_ECHO_INPUT));

	registers[RETURN_REGISTER] = convertToSaveableStr(value);
}

//Pauses system
void program_pause()
{
	_getch();
}

//Clears the console (in case you haven't realized already, this is the Windows version of Tree)
void io_clearConsole()
{
	system("cls");
}

void io_cout()
{
	string firstArg = getArrayIndex(getVariable(PARAMETER_VARIABLE), 0);
	cout << convertToPrintableStr(firstArg);
}

void io_getKeyState()
{
	string firstArg = getArrayIndex(getVariable(PARAMETER_VARIABLE), 0);
	if (firstArg[0] != NUM32)
	{
		fatalError(RUNTIME_INVALID_PARAM_TYPE);
	}

	int keyType = stringToUI64(firstArg.substr(1));
	registers[RETURN_REGISTER] = (char)NUM32 + numericalToString(GetKeyState(keyType));
}
