#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include "constants.h"
using namespace std;

void WriteBatchFile(FILE **bat, string stream_key, string addr);