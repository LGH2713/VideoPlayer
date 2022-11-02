#include "XVideoThread.h"

#include "XDecode.h"
#include <iostream>
using namespace std;

XVideoThread::XVideoThread()
{

}

XVideoThread::~XVideoThread()
{

}

bool XVideoThread::Open(AVCodecParameters *para, IVideoCall *call, int width, int height)
{
    if(!para)
        return false;

    Clear();

    vmux.lock();
    synpts = 0;

    // 初始化显示窗口
    this->call = call;
    if(call)
    {
        call->Init(width, height);
    }

    vmux.unlock();

    bool ret = true;
    if(!decode->Open(para))
    {
        cout << "audio XDecode open failed !" << endl;
        ret = false;
    }

    cout << "XAudioThread::Open = " << ret << endl;
    return ret;
}

void XVideoThread::run()
{
    while(!isExit)
    {
        vmux.lock();
        if(this->isPause)
        {
            vmux.unlock();
            msleep(5);
            continue;
        }

        // 音视频同步
        if(synpts > 0 && synpts < decode->pts)
        {
            vmux.unlock();
            msleep(1);
            continue;
        }

//        // 没有数据
//        if(packs.empty() || !decode)
//        {
//            vmux.unlock();
//            msleep(1);
//            continue;
//        }

//        AVPacket *pkt = packs.front();
//        packs.pop_front();
        AVPacket *pkt = Pop();
        bool ret = decode->Send(pkt);
        if(!ret)
        {
            vmux.unlock();
            msleep(1);
            continue;
        }

        // 一次send 多次recv
        while(!isExit)
        {
            AVFrame *frame = decode->Recv();
            if(!frame)
                break;

            // 显示视频
            if(call)
            {
                call->Repaint(frame);
            }

        }
        vmux.unlock();
    }
}

void XVideoThread::SetPause(bool isPause)
{
    vmux.lock();
    this->isPause = isPause;
    vmux.unlock();
}
