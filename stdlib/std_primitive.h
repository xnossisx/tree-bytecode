#include "stddeps.h"
import utils;

void primitive_str_to_num() {
	int commandSize = getPrimitiveLength(getVariable(PARAMETER_VARIABLE));
	
	if (commandSize > 1) {
		string strForm = getArrayIndex(getVariable(PARAMETER_VARIABLE), 0).substr(1); // get the first index and cut off the header
		if (strForm.find(".") == string::npos) {
			if (strForm.length() > 8)
				fatalError(RUNTIME_STRING_FORMAT_EXCEPTION);
			registers[RETURN_REGISTER] = (char)vartypes::NUM64 + UI64ToString(std::stol(strForm));
		}
		else {
			registers[RETURN_REGISTER] = (char)vartypes::NUMFLOAT + numericalToString<double>(std::stod(strForm));
		}
	}
}