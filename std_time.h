#include "stddeps.h"
#include <chrono>
import utils;

// Allows for access to unix time (seconds since 1970)
void time_unix() {
	registers[RETURN_REGISTER] = 
		(char)vartypes::NUM64 + UI64ToString(std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count());
}

tm time_gen() {
	std::time_t time = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
	tm cTime;
	localtime_s(&cTime, &time);
	return cTime;
}

void time_sec() {
	tm cTime = time_gen();
	registers[RETURN_REGISTER] = (char)vartypes::NUMBYTE + UI64ToString(cTime.tm_sec, 1);
}

void time_min() {
	tm cTime = time_gen();
	registers[RETURN_REGISTER] = (char)vartypes::NUMBYTE + UI64ToString(cTime.tm_min, 1);
}

void time_hour() {
	tm cTime = time_gen();
	registers[RETURN_REGISTER] = (char)vartypes::NUMBYTE + UI64ToString(cTime.tm_hour, 1);
}

void time_wday() {
	tm cTime = time_gen();
	registers[RETURN_REGISTER] = (char)vartypes::NUMBYTE + UI64ToString(cTime.tm_wday, 1);
}

void time_month() {
	tm cTime = time_gen();
	registers[RETURN_REGISTER] = (char)vartypes::NUMBYTE + UI64ToString(cTime.tm_mon, 1);
}

void time_year() {
	tm cTime = time_gen();
	registers[RETURN_REGISTER] = (char)vartypes::NUM32 + UI64ToString(cTime.tm_year, 4);
}