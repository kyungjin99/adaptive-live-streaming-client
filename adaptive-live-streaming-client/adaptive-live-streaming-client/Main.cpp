#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#define MAXLINE 5000
using namespace std;

vector<string> vid_dvc_list; // video device list
vector<string> aud_dvc_list; // audio device list

string * trim(string * cmd_result) { // leave only sentences starting with "[dshow @"
	int start = cmd_result->find("[dshow @");
	if (start != string::npos) *cmd_result = cmd_result->substr(start, -1);

	int end = cmd_result->find("dummy");
	if (end != string::npos) *cmd_result = cmd_result->substr(0, end);
	return cmd_result;
}

void add_dvc_to_list(string line, int flag) {
	int start = line.find("\"@device");
	if (start != string::npos) {
		line = line.substr(start, -1);

		if (flag == 1) vid_dvc_list.push_back(line);
		else if (flag == 2) aud_dvc_list.push_back(line);
	}
}

void get_dvc_lists(string * cmd_result) {
	int prev = 0;
	int now = cmd_result->find('\n');
	int flag = 0; // 1: video 2: audio
	while (now != string::npos) {
		string line = cmd_result->substr(prev, now - prev);

		if (line.find("DirectShow video devices") != string::npos) flag = 1;
		else if (line.find("DirectShow audio devices") != string::npos) flag = 2;
		else add_dvc_to_list(line, flag);

		prev = now + 1;
		now = cmd_result->find('\n', prev);
	}
	string last_line = cmd_result->substr(prev, now - prev);
	add_dvc_to_list(last_line, flag);
}

void read_from_cmd(FILE ** fp, string * cmd_result) {
	string tmp;
	char buf[MAXLINE];

	while (fgets(buf, MAXLINE, *fp) != NULL) *cmd_result += buf; // fgets() == NULL indicates either EOF or I/O error
	_pclose(*fp);

	trim(cmd_result); // "[dshow @"로 시작하는 문장만 남기고 다 삭제
	get_dvc_lists(cmd_result);
}

void write_to_bat(FILE ** bat) {
	fprintf(*bat, "ffmpeg -f dshow -rtbufsize 100M -i ");
	bool is_using_vid = vid_dvc_list.size() ? true : false;
	bool is_using_aud = aud_dvc_list.size() ? true : false;

	if (is_using_vid) {
		fprintf(*bat, "video=");
		fprintf(*bat, vid_dvc_list[0].c_str());
	}

	if (is_using_aud) {
		fprintf(*bat, ":audio=");
		fprintf(*bat, aud_dvc_list[0].c_str());
	}

	fprintf(*bat, " ");
	if (is_using_vid) fprintf(*bat, "-profile:v high -pix_fmt yuvj420p -level:v 4.1 -preset ultrafast -tune zerolatency -c:v h264 -r 10 -b:v 512k -s 640x360 ");
	if (is_using_aud) fprintf(*bat, "-c:a aac -ac 2 -ab 32k -ar 44100 ");
	fprintf(*bat, "-f flv -flush_packets 0 ");
	fprintf(*bat, "rtmp://");
	fprintf(*bat, "127.0.0.1"); // ip
	fprintf(*bat, ":1935");
	fclose(*bat); // close batch file
}

int main() {
	FILE * fp, * bat;
	string dvc_cmd, cmd_result;

	// TODO: GUI

/* GET DEVICE NAME */
	dvc_cmd = "ffmpeg -list_devices true -f dshow -i dummy";
	dvc_cmd.append(" 2>&1"); // redirect the stderr (>2) to where stdout is being redirected to (&1).
	cmd_result = "";
	fp = _popen(dvc_cmd.c_str(), "r"); // open cmd to read
	if (!fp || feof(fp)) { // file pointer error || EOF error
		perror("errno: ");
		return -1;
	}	
	// TODO: I/O error

	read_from_cmd(&fp, &cmd_result);

/* WRITE TO BATCH FILE */
	// TODO: check if device is available
	bat = fopen("batch.bat", "w+");
	if (!bat) {
		perror("errno: ");
		return -1;
	}
	else write_to_bat(&bat);

	system("batch.bat");
//	system("pause");
	return 0;
}