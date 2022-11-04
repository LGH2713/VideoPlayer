#include "XAudioThread.h"
#include "XDecode.h"
#include "XResample.h"
#include "XAudioPlay.h"

#include <iostream>
using namespace std;

XAudioThread::XAudioThread()
{
    if(!res) res = new XResample();
    if(!ap) ap = XAudioPlay::Get();
}

XAudioThread::~XAudioThread()
{

}

void XAudioThread::run()
{
    unsigned char *pcm = new  unsigned char[1024 * 1024 * 10];
    while(!isExit)
    {
        amux.lock();

        if(isPause)
        {
            amux.unlock();
            msleep(5);
            continue;
        }

        // 没有数据
        //        if(packs.empty() || !decode || !res || !ap)
        //        {
        //            amux.unlock();
        //            msleep(1);
        //            continue;
        //        }

        //        AVPacket *pkt = packs.front();
        //        packs.pop_front();

        AVPacket *pkt = Pop();
        bool ret = decode->Send(pkt);
        if(!ret)
        {
            amux.unlock();
            msleep(1);
            continue;
        }

        // 一次send 多次recv
        while(!isExit)
        {
            AVFrame *frame = decode->Recv();
            if(!frame)
                break;

            // 减去缓冲中未播放的时间
            pts = decode->pts - ap->GetNoPlayMs();
            cout << "audio thread pts = " << pts << endl;
            // 重采样
            int size = res->Resample(frame, pcm);
            // 播放音频
            while(!isExit)
            {
                if(size <= 0)
                    break;
                // 缓冲未播完，空间不够
                if(ap->GetFree() < size || isPause)
                {
                    msleep(1);
                    continue;
                }
                ap->Write(pcm, size);
                break;
            }
        }

        amux.unlock();
    }

    delete[] pcm;
}

bool XAudioThread::Open(AVCodecParameters *para)
{
    if(!para)
        return false;

    Clear();

    amux.lock();

    pts = 0;

    bool ret = true;
    if(!res->Open(para, false))
    {
        ret = false;
        cout << "XResample open failed !" << endl;
        //        amux.unlock();
        //        return false;
    }

    if(!ap->Open())
    {
        ret = false;
        cout << "XAudioPlay open failed !" << endl;
        //        amux.unlock();
        //        return false;
    }

    if(!decode->Open(para))
    {
        cout << "audio XDecode open failed !" << endl;
        ret = false;
    }

    cout << "XAudioThread::Open = " << ret << endl;

    amux.unlock();
    return ret;
}

void XAudioThread::Close()
{
    XDecodeThread::Close();
    if(res)
    {
        res->Close();
        amux.lock();
        res = nullptr;
        amux.unlock();
    }

    if(ap)
    {
        ap->Close();
        amux.lock();
        ap = nullptr;
        amux.unlock();
    }
}

void XAudioThread::Clear()
{
    XDecodeThread::Clear();
    mux.lock();
    if(ap) ap->Clear();
    mux.unlock();
}

void XAudioThread::SetPause(bool isPause)
{
    //    amux.lock();
    this->isPause = isPause;
    if(ap)
        ap->SetPause(isPause);
    //    amux.unlock();
}
