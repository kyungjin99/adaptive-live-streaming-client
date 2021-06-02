#include "write-batch.h"

void WriteBatchFile(FILE **bat, string stream_key, string addr)
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
    fprintf(*bat, addr.c_str()); // server 
    fprintf(*bat, ":1935/live/");
    fprintf(*bat, stream_key.c_str()); // stream key
    fclose(*bat);
}