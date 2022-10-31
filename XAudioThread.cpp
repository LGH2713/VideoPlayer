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

}

void XAudioThread::run()
{

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
