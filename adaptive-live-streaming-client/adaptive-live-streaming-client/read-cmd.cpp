#include "read-cmd.h"

bool is_obs;
constexpr int MAXLINE = 5000;

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
    if (start == string::npos) // 실제 이름인 경우 (obs인지 체크)
    {
        is_obs = (line.find("OBS") != string::npos) ? true : false;
    }
    else {
        if (!is_obs)
        {
            line = line.substr(start, -1);
            if (flag == 1) g_video_device_list.push_back(line);
            else if (flag == 2) g_audio_device_list.push_back(line);
        }
        is_obs = false;
    }
}

void GetDeviceLists(string *device_command_output)
{
    int prev = 0;
    int now = device_command_output->find('\n');
    int flag = 0; // 1: video 2: audio
    bool is_obs = false; // check if OBS Virtual Camera/Mic
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

    cout << "test: " << *device_command_output << endl; // test
    Trim(device_command_output);
    GetDeviceLists(device_command_output);
}