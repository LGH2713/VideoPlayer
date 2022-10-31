#include "XDemuxThread.h"
#include "XDemux.h"
#include "XVideoThread.h"
#include "XAudioThread.h"

#include <iostream>
using namespace std;

XDemuxThread::XDemuxThread()
{

}

XDemuxThread::~XDemuxThread()
{

}

bool XDemuxThread::Open(const char *url, IVideoCall *call)
{
    // 容错处理
    if(url == 0 || url[0] == '\0')
        return false;

    mux.lock();

    if(!demux) demux = new XDemux();
    if(!vt) vt = new XVideoThread();
    if(!at) at = new XAudioThread();

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

    mux.unlock();
    cout << "XDemucThread::Open seccess!" << endl;
    return ret;
}
