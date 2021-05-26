#include <iostream>
#include <string>
#include <vector>
#include <cstring> // memset
#define MAXLINE 1025
using namespace std;

string get_device_name(string line) {
	string dvc_name = "";
	return dvc_name;
}

int main() {

	FILE * fp;
	string dvc_cmd, cmd_result, tmp;
	char buf[MAXLINE];

	/* 1. get list of client's audio/video devices */
	dvc_cmd = "ffmpeg -list_devices true -f dshow -i dummy";
	dvc_cmd.append(" 2>&1"); // redirect the stderr (>2) to where stdout is being redirected to (&1).
	cmd_result = "";

	fp = _popen(dvc_cmd.c_str(), "r"); // open cmd to read

	if (!fp) { // file pointer error
		perror("errno: ");
		return -1;
	}
	
	if (feof(fp)) { // EOF error
		perror("errno: ");
		return -1;
	}

	// TODO: i/o error	

	while (fgets(buf, MAXLINE, fp) != NULL) { // fgets() == NULL indicates either EOF or I/O error
	// cout << "buf: " << buf << endl; // test
		cmd_result += buf;
	}

	_pclose(fp);
	cout << "cmd_result: " << cmd_result << endl; // test

	/* 3. parse device names from commandline output */
	
	/*
	vector<string> video_devices;
	vector<string> audio_devices;
	
	// write to batch file
	FILE * bat = _popen("path", "w"); // open batch file to write
	// "ffmpeg -f dshow -rtbufsize 100M -i video="720p HD Camera":audio="마이크 배열(Realtek(R) Audio)" -profile:v high -pix_fmt yuvj420p -level:v 4.1 -preset ultrafast -tune zerolatency -c:v h264 -r 10 -b:v 512k -s 640x360 -c:a aac -ac 2 -ab 32k -ar 44100 -f flv -flush_packets 0 rtmp://127.0.0.1:1935";
	fprintf(bat, "ffmpeg -f dshow ");
	fprintf(bat, "-rtbufsize 100M ");
	// fprintf(bat, "-i video=\"720p HD Camera\"");
	fprintf(bat, "-i video=\"720p HD Camera\"");
	fprintf(bat, ":audio=\"마이크 배열(Realtek(R) Audio)\" ");
	// fprintf(bat, ":audio=\"마이크 배열(Realtek(R) Audio)\" ");
	fprintf(bat, "-profile:v high -pix_fmt yuvj420p -level:v 4.1 -preset ultrafast -tune zerolatency -c:v h264 -r 10 -b:v 512k -s 640x360 ");
	fprintf(bat, "-c:a aac -ac 2 -ab 32k -ar 44100 ");
	fprintf(bat, "-f flv ");
	fprintf(bat, "-flush_packets 0 ");
	fprintf(bat, "rtmp://");
	fprintf(bat, "127.0.0.1"); // ip
	fprintf(bat, ":1935");
	// fprintf(bat, "first command\n");
	// fprintf(bat, "second command\n");
	_pclose(bat); // close batch file
	*/
	system("pause");
	return 0;
}