#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include "constants.h"
using namespace std;

string *Trim(string *device_command_output);
void AddDeviceToList(string line, int flag);
void GetDeviceLists(string *device_command_output);
void ReadFromCommandLine(FILE **fp, string *device_command_output);