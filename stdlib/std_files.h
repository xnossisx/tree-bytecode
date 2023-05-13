import utils;

//Plan: File object will have two arguments: one being a pointer to the actual IO reference, the second being a position number
//names: _file, pos

void file_openFile()
{
	// Step 1: Get all information and create all needed variables
	string fileNameData = getArrayIndex(getVariable(PARAMETER_VARIABLE), 0);
	string fileName = convertToPrintableStr(fileNameData);
	Object* file = createObject();
	
	// Step 2: Open file
	std::fstream* fileStream = new std::fstream{ fileName, std::ios::in | std::ios::out | std::ios::binary };
	if (!fileStream->is_open()) {
		fatalError(RUNTIME_FILE_NOT_FOUND);
	}

	// Step 3: Link file to object
	setObjectProperty(file, string("_file"), (char) vartypes::NUM64 + UI64ToString(reinterpret_cast<ulonger>(fileStream)));
	setObjectProperty(file, string("pos"), (char) vartypes::NUM64 + UI64ToString(0));

	registers[RETURN_REGISTER] = convertObjectToString(file);
}

void file_getSize() {
	string fileVar = getArrayIndex(getVariable(PARAMETER_VARIABLE), 0);
	Object* fileObject = retrieveObjectFromString(fileVar);
	std::fstream* file = reinterpret_cast<std::fstream*>(stringToUI64(getObjectProperty(fileObject, string("_file")).substr(1)));

	ulonger len = getPrimitiveLength(getVariable(PARAMETER_VARIABLE)) > 1 ? stringToUI64(getArrayIndex(getVariable(PARAMETER_VARIABLE), 1).substr(1)) : ULLONG_MAX;

	file->seekg(0, std::ios::end);
	len = (len < file->tellg()) ? len : (ulonger)file->tellg();
	file->seekg(0, std::ios::beg);

	registers[RETURN_REGISTER] = (char)vartypes::NUM64 + UI64ToString(len);
}

//void file_openFileObj()
//{
//	string fileVar = getArrayIndex(getVariable(ARGUMENT_VARIABLE), 0);
//	string fileNameData = getArrayIndex(getVariable(ARGUMENT_VARIABLE), 1);
//	fileVar = retrieveValue(fileVar);
//	
//	Object* fileObject = retrieveObjectFromString(fileVar); 
//	string fileName = convertToPrintableStr(fileNameData); // this should do error checking for us
//	std::fstream* file = reinterpret_cast<std::fstream*>(stringToLongLong(getObjectProperty(fileObject, string("_file")).substr(1)));
//}

void file_closeFile()
{
	string fileVar = getArrayIndex(getVariable(PARAMETER_VARIABLE), 0);
	Object* fileObject = retrieveObjectFromString(fileVar);
	std::fstream* file = reinterpret_cast<std::fstream *>(stringToUI64(getObjectProperty(fileObject, string("_file")).substr(1)));

	file->close();

	setObjectProperty(fileObject, string("file"), (char)vartypes::STRING + UI64ToString(0));
}

// Gets characters from the file up to character len or until the file is finished
void file_getFileContent()
{
	string fileVar = getArrayIndex(getVariable(PARAMETER_VARIABLE), 0);
	Object* fileObject = retrieveObjectFromString(fileVar);
	std::fstream* file = reinterpret_cast<std::fstream*>(stringToUI64(getObjectProperty(fileObject, string("_file")).substr(1)));

	ulonger len = getPrimitiveLength(getVariable(PARAMETER_VARIABLE)) > 1 ? stringToUI64(getArrayIndex(getVariable(PARAMETER_VARIABLE), 1).substr(1)) : ULLONG_MAX;

	file->seekg(0, std::ios::end);
	len = (len < file->tellg()) ? len : (ulonger)file->tellg();
	file->seekg(0, std::ios::beg);
	
	string data(len, '\0');
	file->read(&data[0], len);

	registers[RETURN_REGISTER] = (char)vartypes::STRING + UI64ToString(len) + data;
}

void file_writeFileContent()
{
	string fileVar = getArrayIndex(getVariable(PARAMETER_VARIABLE), 0);
	Object* fileObject = retrieveObjectFromString(fileVar);
	std::fstream* file = reinterpret_cast<std::fstream*>(stringToUI64(getObjectProperty(fileObject, string("_file")).substr(1)));
	string insert = convertToProperStr(getArrayIndex(getVariable(PARAMETER_VARIABLE), 1).substr(1));
	*file << insert;
}