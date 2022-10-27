#include "mainwindow.h"

#include <QApplication>

#include <iostream>
#include <thread>
using namespace std;

extern "C" {
#include "libavformat/avformat.h"
#include "libavcodec/avcodec.h"
}

static double r2d(AVRational r) {
    return r.den == 0 ? 0 : static_cast<double>(r.num) / static_cast<double>(r.den);
}

void XSleep(int ms) {
    chrono::milliseconds du(ms);
    this_thread::sleep_for(du);
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
//    w.show();

    cout << "Test Demux FFmpeg.club" << endl;

    const char *path = "E:\\me.mp4";

    //初始化网络库 （可以打开rtsp rtmp http 协议的流媒体视频）
    avformat_network_init();

    //参数设置
    AVDictionary *opts = NULL;
    //设置rtsp流已tcp协议打开
    av_dict_set(&opts, "rtsp_transport", "tcp", 0);

    //网络延时时间
    av_dict_set(&opts, "max_delay", "500", 0);


    //解封装上下文
    AVFormatContext *ic = NULL;
    int re = avformat_open_input(
                &ic,
                path,
                0,  // 0表示自动选择解封器
                &opts //参数设置，比如rtsp的延时时间
                );
    if (re != 0)
    {
        char buf[1024] = { 0 };
        av_strerror(re, buf, sizeof(buf) - 1);
        cout << "open " << path << " failed! :" << buf << endl;
        getchar();
        return -1;
    }
    cout << "open " << path << " success! " << endl;

    //获取流信息
    re = avformat_find_stream_info(ic, 0);

    //总时长 毫秒
    int totalMs = ic->duration / (AV_TIME_BASE / 1000);
    cout << "totalMs = " << totalMs << endl;

    //打印视频流详细信息
    av_dump_format(ic,0,path,0);

    int videoStream = -1;
    int audioStream = -1;

    for(int i = 0; i < ic->nb_streams; i++) {
        AVStream *as = ic->streams[i];
        if(as->codecpar->codec_type == AVMEDIA_TYPE_AUDIO) {
            audioStream = i;
            cout << i << " audio information" << endl;
            cout << "sample_rate = " << as->codecpar->sample_rate << endl;
            cout << "format = " << as->codecpar->format << endl;
            cout << "nb_channels = " << as->codecpar->ch_layout.nb_channels << endl;
            cout << "codec_id = " << as->codecpar->codec_id << endl;
            cout << "frame_size = " << as->codecpar->frame_size << endl;
        }
        else if(as->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
            videoStream = i;
            cout << i << " video information" << endl;
            cout << "width = " << as->codecpar->width << endl;
            cout << "height = " << as->codecpar->height << endl;
            cout << "video fps = " << r2d(as->avg_frame_rate) << endl;
        }
    }

    AVPacket *pkt = av_packet_alloc();
    for(;;) {
        int ret = av_read_frame(ic, pkt);
        if(ret != 0) {
            cout << "========================= end ==============================" << endl;
            int ms = 3000; // 三秒位置 根据时间基进行转换
            long long pos = static_cast<double>(ms) / static_cast<double>(1000) * r2d(ic->streams[pkt->stream_index]->time_base);
            av_seek_frame(ic, videoStream, pos, AVSEEK_FLAG_FRAME | AVSEEK_FLAG_BACKWARD);
            continue;
        }

        cout << "pkt->size = " << pkt->size << endl;
        // 显示的时间
        cout << "pkt->pts = " << pkt->pts << endl;

        // 转换成毫秒，方便做同步
        cout << "pkt->pts ms = " << pkt->pts * (r2d(ic->streams[pkt->stream_index]->time_base)) << endl;

        // 解码时间
        cout << "pkt->dts = " << pkt->dts << endl;
        if(pkt->stream_index == videoStream) {
            cout << "video: " << endl;
        }
        if(pkt->stream_index == audioStream) {
            cout << "audio: " << endl;
        }

        av_packet_unref(pkt);

//        XSleep(500);
    }

    av_packet_free(&pkt);


    if (ic)
    {
        //释放封装上下文，并且把ic置0
        avformat_close_input(&ic);
    }

    getchar();
    return a.exec();
}
