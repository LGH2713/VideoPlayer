#include "XDecode.h"
#include <iostream>
using namespace std;

extern "C" {
#include <libavcodec/avcodec.h>
}

void XFreePacket(AVPacket **pkt)
{
    if(!pkt || !(*pkt))
        return;
    av_packet_free(pkt);
}


XDecode::XDecode()
{

}

bool XDecode::Open(AVCodecParameters *para)
{
    if(!para)
        return false;

    Close();

    int ret = -1;

    ///////////////////////////////////////////////////////////////////
    /// 寻找解码器
    AVCodec const *vcodec = avcodec_find_decoder(para->codec_id);

    if(!vcodec) {
        avcodec_parameters_free(&para);
        cout << "can't find codec id" << endl;
        return false;
    }

    cout << "Find codec id = " << para->codec_id << endl;

    //创建解码器上下文
    mux.lock();
    codec = avcodec_alloc_context3(vcodec);

    // 配置解码器上下文参数
    avcodec_parameters_to_context(codec, para);
    avcodec_parameters_free(&para);

    // 设置解码线程数
    codec->thread_count = 8;

    // 打开解码器上下文
    ret = avcodec_open2(codec, 0, 0);
    if(ret != 0) {
        avcodec_free_context(&codec);
        mux.unlock();
        char buf[1024] = {0};
        av_strerror(ret, buf, sizeof(buf) - 1);
        cout << "avcodec_open2 failed : " << buf << endl;
        return false;
    }
    cout << "video avcodec_open2 success" << endl;

    avcodec_parameters_free(&para);
    mux.unlock();
    return true;
}

bool XDecode::Send(AVPacket *pkt)
{
    // 容错处理
    if(!pkt || pkt->size <= 0 || !pkt->data)
        return false;
    mux.lock();
    if(!codec)
    {
        mux.unlock();
        return false;
    }
    int ret = avcodec_send_packet(codec, pkt);
    mux.unlock();
    av_packet_free(&pkt);

    if(ret != 0)
        return false;

    return true;
}

AVFrame *XDecode::Recv()
{
    mux.lock();
    if(!codec)
    {
        mux.unlock();
        return nullptr;
    }

    AVFrame *frame = av_frame_alloc();
    int ret = avcodec_receive_frame(codec, frame);
    mux.unlock();
    if(ret != 0)
    {
        av_frame_free(&frame);
        return nullptr;
    }

//    cout << "[" << frame->linesize[0] << "]" << endl;

    // 记录取到的pts
    pts = frame->pts;
    return frame;
}

void XDecode::Close()
{
    mux.lock();
    if(codec)
    {
        avcodec_close(codec);
        avcodec_free_context(&codec);
    }
    pts = 0;
    mux.unlock();
}

void XDecode::Clear()
{
    mux.lock();
    if(codec)
        avcodec_flush_buffers(codec);
    mux.unlock();
}
