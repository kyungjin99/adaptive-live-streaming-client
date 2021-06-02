#define _CRT_SECURE_NO_WARNINGS
#include "read-cmd.h"
#include "write-batch.h"
using namespace std;

int main(int argc, char **argv) 
{
    FILE* fp, *bat;
    string device_command_input, device_command_output;
    string stream_key = argv[1];
    string addr = argv[2];

	/* TODO: CHANGE ENCODING TO 'UTF-8' */
//	encoding_command = "chcp 65001;";
//	fp = _popen(encoding_command.c_str(), "r");
//	if (!fp || feof(fp)) // file pointer error || EOF error
//	{
//		perror("errno: ");
//		return -1;
//	}

	/* GET DEVICE NAME */
    device_command_input = "ffmpeg -list_devices true -f dshow -i dummy";
    device_command_input.append(" 2>&1"); // redirect the stderr (>2) to where stdout is being redirected to (&1).
    device_command_output = "";
    fp = _popen(device_command_input.c_str(), "r"); // open cmd to read
    if (!fp || feof(fp)) // file pointer error || EOF error
    { 
        perror("errno: ");
        return -1;
    }	

    ReadFromCommandLine(&fp, &device_command_output);

    /* WRITE TO BATCH FILE */
    // TODO: check if the device is available
    bat = fopen("batch.bat", "w+");
    if (!bat) 
    {
        perror("errno: ");
        return -1;
    }
    else 
    {
        WriteBatchFile(&bat, stream_key, addr);
        system("batch.bat");
    }

//    system("pause");
    return 0;
}