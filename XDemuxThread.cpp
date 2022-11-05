#include "XDemuxThread.h"
#include "XDemux.h"
#include "XVideoThread.h"
#include "XAudioThread.h"
#include "XDecode.h"

extern "C" {
#include <libavformat/avformat.h>
}

#include <iostream>
using namespace std;

XDemuxThread::XDemuxThread()
{

}

XDemuxThread::~XDemuxThread()
{
    isExit = true;
    wait();
}

bool XDemuxThread::Open(const char *url, IVideoCall *call)
{
    // 容错处理
    if(url == 0 || url[0] == '\0')
        return false;

    mux.lock();

    // 打开解封装
    bool ret = demux->Open(url);
    if(!ret)
    {
        mux.unlock();
        cout << "demux->Open(url) failed!" << endl;
        return false;
    }

    // 打开视频解码器和处理线程
    if(!vt->Open(demux->CopyVPara(), call, demux->width, demux->height))
    {
        ret = false;
        cout << "vt->Open() failed!" << endl;
    }

    // 打开音频解码器和处理线程
    if(!at->Open(demux->CopyAPara()))
    {
        ret = false;
        cout << "at->Open() failed!" << endl;
    }

    totalMs = demux->totalMs;
    mux.unlock();
    cout << "XDemucThread::Open seccess!" << endl;
    return ret;
}

void XDemuxThread::Start()
{
    mux.lock();

    if(!demux) demux = new XDemux();
    if(!vt) vt = new XVideoThread();
    if(!at) at = new XAudioThread();


    QThread::start();
    if(vt) vt->start();
    if(at) at->start();

    mux.unlock();
}

void XDemuxThread::Close()
{
    isExit = true;
    wait();
    if(vt) vt->Close();
    if(at) at->Close();
    mux.lock();
    delete vt;
    delete at;
    vt = nullptr;
    at = nullptr;
    mux.unlock();
}

void XDemuxThread::Clear()
{
    mux.lock();
    if(demux)
        demux->Clear();
    if(vt)
        vt->Clear();
    if(at)
        at->Clear();
    mux.unlock();
}

void XDemuxThread::SetPause(bool isPause)
{
    mux.lock();
    this->isPause = isPause;
    if(at) at->SetPause(isPause);
    if(vt) vt->SetPause(isPause);
    mux.unlock();
}

void XDemuxThread::run()
{
    while(!isExit)
    {
        mux.lock();

        if(isPause)
        {
            mux.unlock();
            msleep(5);
            continue;
        }

        if(!demux)
        {
            mux.unlock();
            msleep(5);
            continue;
        }

        // 音视频同步
        if(vt && at)
        {
            pts = at->pts;
            vt->synpts = at->pts;
        }

        AVPacket *pkt = demux->Read();
        if (!pkt)
        {
            mux.unlock();
            msleep(5);
            continue;
        }

        // 判断数据是音频
        if(demux->IsAudio(pkt))
        {
            if(at) at->Push(pkt);
        }
        else // 视频
        {
            if(vt) vt->Push(pkt);
        }

        mux.unlock();
        msleep(1);
    }
}

void XDemuxThread::Seek(double pos)
{
    // 清理缓冲
    Clear();

    mux.lock();
    bool status = this->isPause;
    mux.unlock();

    // 暂停
    SetPause(true);

    mux.lock();
    if(demux)
        demux->Seek(pos);

    // 实际要显示的位置pts
    long long seekPts = pos * demux->totalMs;
    while(!isExit)
    {
        AVPacket *pkt = demux->ReadVideo();
        if(!pkt)
            break;

        // 如果解码到pts
        if(vt->RepaintPts(pkt, seekPts))
        {
            this->pts = seekPts;
            break;
        }

    }
    mux.unlock();

    // seek是非暂停状态
    if(!status)
        SetPause(false);
}


