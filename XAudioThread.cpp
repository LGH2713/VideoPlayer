#include "XAudioThread.h"
#include "XDecode.h"
#include "XResample.h"
#include "XAudioPlay.h"

#include <iostream>
using namespace std;

XAudioThread::XAudioThread()
{

}

XAudioThread::~XAudioThread()
{
    // 等待线程退出
    isExit = true;
    wait();
}

void XAudioThread::run()
{
    unsigned char *pcm = new  unsigned char[1024 * 1024 * 10];
    while(!isExit)
    {
        mux.lock();

        // 没有数据
        if(packs.empty() || !decode || !res || !ap)
        {
            mux.unlock();
            msleep(1);
            continue;
        }

        AVPacket *pkt = packs.front();
        packs.pop_front();
        bool ret = decode->Send(pkt);
        if(!ret)
        {
            mux.unlock();
            msleep(1);
            continue;
        }

        // 一次send 多次recv
        while(!isExit)
        {
            AVFrame *frame = decode->Recv();
            if(!frame)
                break;
            // 重采样
            int size = res->Resample(frame, pcm);
            // 播放音频
            while(!isExit)
            {
                if(size <= 0)
                    break;
                // 缓冲未播完，空间不够
                if(ap->GetFree() < size)
                {
                    msleep(1);
                    continue;
                }
                ap->Write(pcm, size);
                break;
            }
        }

        mux.unlock();
    }

    delete[] pcm;
}

bool XAudioThread::Open(AVCodecParameters *para)
{
    if(!para)
        return false;

    mux.lock();

    if(!decode) decode = new XDecode();
    if(!res) res = new XResample();
    if(!ap) ap = XAudioPlay::Get();

    bool ret = true;
    if(!res->Open(para, false))
    {
        ret = false;
        cout << "XResample open failed !" << endl;
        //        mux.unlock();
        //        return false;
    }

    if(!ap->Open())
    {
        ret = false;
        cout << "XAudioPlay open failed !" << endl;
        //        mux.unlock();
        //        return false;
    }

    if(!decode->Open(para))
    {
        cout << "audio XDecode open failed !" << endl;
        ret = false;
    }

    cout << "XAudioThread::Open = " << ret << endl;

    mux.unlock();
    return ret;
}

void XAudioThread::Push(AVPacket *pkt)
{
    if(!pkt)
        return;

    while(!isExit)
    {
        mux.lock();
        if(packs.size() < maxList)
        {
            packs.push_back(pkt);
            mux.unlock();
            break;
        }
        mux.unlock();
        msleep(1);
    }
}
