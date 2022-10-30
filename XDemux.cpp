#include "XDemux.h"

extern "C" {
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
}

#include <mutex>
#include <iostream>
using namespace std;

static double r2d(AVRational r) {
    return r.den == 0 ? 0 : static_cast<double>(r.num) / static_cast<double>(r.den);
}


XDemux::XDemux()
{
    static bool isFirst = true;
    static std::mutex dmux;
    dmux.lock();
    if(isFirst)
    {
        // 初始化网络库
        avformat_network_init();
        isFirst = false;
    }
    dmux.unlock();
}

bool XDemux::Open(const char *url)
{
    Close();

    //参数设置
    AVDictionary *opts = NULL;
    //设置rtsp流已tcp协议打开
    av_dict_set(&opts, "rtsp_transport", "tcp", 0);

    //网络延时时间
    av_dict_set(&opts, "max_delay", "500", 0);

    // 需要互斥访问ic
    mux.lock();

    int ret = avformat_open_input(
                &ic,
                url,
                0,  // 0表示自动选择解封器
                &opts //参数设置，比如rtsp的延时时间
                );
    if (ret != 0)
    {
        mux.unlock();
        char buf[1024] = { 0 };
        av_strerror(ret, buf, sizeof(buf) - 1);
        cout << "open " << url << " failed! :" << buf << endl;
        return false;
    }
    cout << "open " << url << " success! " << endl;

    //获取流信息
    ret = avformat_find_stream_info(ic, 0);

    //总时长 毫秒
    int totalMs = ic->duration / (AV_TIME_BASE / 1000);
    cout << "totalMs = " << totalMs << endl;

    //打印视频流详细信息
    av_dump_format(ic,0,url,0);


    // 获取视频流
    videoStream = av_find_best_stream(ic, AVMEDIA_TYPE_VIDEO, -1, -1, NULL, 0);
    AVStream *vs = ic->streams[videoStream];
    // 获取视频的宽高
    width = vs->codecpar->width;
    height = vs->codecpar->height;
    cout << videoStream << " video information" << endl;
    cout << "width = " << vs->codecpar->width << endl;
    cout << "height = " << vs->codecpar->height << endl;
    cout << "video fps = " << r2d(vs->avg_frame_rate) << endl;
    cout << "codec_id = " << vs->codecpar->codec_id << endl;
    cout << "frame_size = " << vs->codecpar->frame_size << endl;

    // 获取音频流
    audioStream = av_find_best_stream(ic, AVMEDIA_TYPE_AUDIO, -1, -1, NULL, 0);
    AVStream *as = ic->streams[audioStream];
    cout << audioStream << " audio information" << endl;
    cout << "sample_rate = " << as->codecpar->sample_rate << endl;
    cout << "format = " << as->codecpar->format << endl;
    cout << "nb_channels = " << as->codecpar->ch_layout.nb_channels << endl;
    cout << "codec_id = " << as->codecpar->codec_id << endl;
    cout << "frame_size = " << as->codecpar->frame_size << endl;

    mux.unlock();

    return true;
}

AVPacket *XDemux::Read()
{
    mux.lock();
    if(!ic)
    {
        mux.unlock();
        return nullptr;
    }

    // 读取一帧并分配空间
    AVPacket *pkt = av_packet_alloc();
    int ret = av_read_frame(ic, pkt);
    if(ret != 0)
    {
        mux.unlock();
        av_packet_free(&pkt);
        return nullptr;
    }

    // pts,dts转换为毫秒
    pkt->pts = pkt->pts * (1000 * (r2d(ic->streams[pkt->stream_index]->time_base)));
    pkt->dts = pkt->dts * (1000 * (r2d(ic->streams[pkt->stream_index]->time_base)));

    cout << "=================" << pkt->pts << flush;

    mux.unlock();

    return pkt;
}

AVCodecParameters *XDemux::CopyVPara()
{

    mux.lock();
    if(!ic)
    {
        mux.unlock();
        return nullptr;
    }
    mux.unlock();
    AVCodecParameters *pa = avcodec_parameters_alloc();
    avcodec_parameters_copy(pa, ic->streams[videoStream]->codecpar);

    mux.unlock();
    return pa;
    return nullptr;
}

AVCodecParameters *XDemux::CopyAPara()
{
    mux.lock();
    if(!ic)
    {
        mux.unlock();
        return nullptr;
    }

    AVCodecParameters *pa = avcodec_parameters_alloc();
    avcodec_parameters_copy(pa, ic->streams[audioStream]->codecpar);

    mux.unlock();
    return pa;
}

bool XDemux::Seek(double pos)
{
    mux.lock();
    if(!ic)
    {
        mux.unlock();
        return false;
    }

    // 清理读取缓冲
    avformat_flush(ic);

    long long seekPos = 0;
    seekPos = ic->streams[videoStream]->duration * pos;
    int ret = av_seek_frame(ic, videoStream, seekPos, AVSEEK_FLAG_BACKWARD | AVSEEK_FLAG_FRAME);
    mux.unlock();
    if(ret < 0)
        return false;

    return true;
}

void XDemux::Clear()
{
    mux.lock();
    if(!ic)
    {
        mux.unlock();
        return;
    }

    // 清理读取缓冲
    avformat_flush(ic);
    mux.unlock();
}

void XDemux::Close()
{
    mux.lock();
    if(!ic)
    {
        mux.unlock();
        return;
    }

    // 清理读取缓冲
    avformat_close_input(&ic);

    // 清空总时长
    totalMs = 0;

    mux.unlock();
}

bool XDemux::IsAudio(AVPacket *pkt)
{
    if(!pkt)
        return false;
    if(pkt->stream_index == videoStream)
        return false;
    return true;
}
