#pragma once

#include <vector>
#include <functional>
#include <map>
#include <string>
#include <iostream>
#include <sstream> //holds source code
#include <thread>
#include <atomic>
#include <bitset>
#include "windows.h"//here for flex tape and MessageBox()

#include "treeEx.h"

typedef long long longer;
typedef unsigned long long ulonger;
typedef std::pair<longer, ulonger> pointer_size_pair;
typedef std::pair<char*, ulonger> dpointer_size_pair;
typedef unsigned char byte;
typedef std::string data_block;

using std::cout; using std::string; using std::ifstream; using std::cin; using std::vector; using std::stringstream;