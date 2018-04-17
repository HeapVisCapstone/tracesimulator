#ifndef SIMULATOR_H
#define SIMULATOR_H

#include <cstdio>
#include <fstream>
#include <iostream>
#include <map>
#include <set>
#include <sstream>
#include <vector>
#include <algorithm>

#include "classinfo.h"
#include "execution.h"
#include "heap.h"
#include "tokenizer.h"

using namespace std;

// Read tracefile from FILE* and namefile from nameFile
// 
// Make use of global state under the covers, so don't call
// more than once
pair<HeapState, ExecState> read_trace_file(char* nameFile, FILE *f);


#endif