#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <string>
#include <vector>
#include <fstream>
using namespace std;

constexpr int MAXLINE = 5000;
vector<string> g_video_device_list; // video device list
vector<string> g_audio_device_list; // audio device list

string *Trim(string *device_command_output) // leave only sentences starting with "[dshow @"
{ 
    int start = device_command_output->find("[dshow @");
    if (start != string::npos) *device_command_output = device_command_output->substr(start, -1);

    int end = device_command_output->find("dummy");
    if (end != string::npos) *device_command_output = device_command_output->substr(0, end);
    return device_command_output;
}

void AddDeviceToList(string line, int flag) 
{
    int start = line.find("\"@device");
    if (start != string::npos) {
        line = line.substr(start, -1);

        if (flag == 1) g_video_device_list.push_back(line);
        else if (flag == 2) g_audio_device_list.push_back(line);
    }
}

void GetDeviceLists(string *device_command_output)
{
    int prev = 0;
    int now = device_command_output->find('\n');
    int flag = 0; // 1: video 2: audio
    while (now != string::npos) {
        string line = device_command_output->substr(prev, now - prev);

        if (line.find("DirectShow video devices") != string::npos) flag = 1;
        else if (line.find("DirectShow audio devices") != string::npos) flag = 2;
        else AddDeviceToList(line, flag);

        prev = now + 1;
        now = device_command_output->find('\n', prev);
    }
    string last_line = device_command_output->substr(prev, now - prev);
    AddDeviceToList(last_line, flag);
}

void ReadFromCommandLine(FILE **fp, string *device_command_output)
{
    string tmp;
    char buf[MAXLINE];

    while (fgets(buf, MAXLINE, *fp) != NULL) *device_command_output += buf; // fgets() == NULL indicates either EOF or I/O error
    _pclose(*fp);

    Trim(device_command_output);
    GetDeviceLists(device_command_output);
}

void WriteBatchFile(FILE **bat) 
{
    fprintf(*bat, "ffmpeg -f dshow -rtbufsize 100M -i ");
    bool is_using_video = g_video_device_list.size() ? true : false;
    bool is_using_audio = g_audio_device_list.size() ? true : false;

    if (is_using_video) 
    {
        fprintf(*bat, "video=");
        fprintf(*bat, g_video_device_list[0].c_str());
    }

    if (is_using_audio) 
    {
        fprintf(*bat, ":audio=");
        fprintf(*bat, g_audio_device_list[0].c_str());
    }

    fprintf(*bat, " ");
    if (is_using_video) fprintf(*bat, "-profile:v high -pix_fmt yuvj420p -level:v 4.1 -preset ultrafast -tune zerolatency -c:v h264 -r 10 -b:v 512k -s 640x360 ");
    if (is_using_audio) fprintf(*bat, "-c:a aac -ac 2 -ab 32k -ar 44100 ");
    fprintf(*bat, "-f flv -flush_packets 0 ");
    fprintf(*bat, "rtmp://");
    fprintf(*bat, "127.0.0.1"); // ip
    fprintf(*bat, ":1935");
    fclose(*bat); // close batch file
}

int main() 
{
    FILE* fp, *bat;
    string device_command_input, device_command_output;

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
		WriteBatchFile(&bat);
		system("batch.bat");
    }

    system("pause");
    return 0;
}