#ifndef XAUDIOTHREAD_H
#define XAUDIOTHREAD_H

#include <QThread>
#include <mutex>
#include <list>
#include "XDecodeThread.h"

struct AVCodecParameters;
class XAudioPlay;
class XResample;

class XAudioThread : public XDecodeThread
{
public:
    XAudioThread();
    ~XAudioThread();

    void run();
    virtual bool Open(AVCodecParameters *para);

    // 停止线程，清理资源
    virtual void Close();

    // 当前音频播放的pts
    long long pts = 0;

    void SetPause(bool isPause);
    bool isPause = false;

protected:
    std::mutex amux;
    XAudioPlay *ap = 0;
    XResample *res = 0;
};

#endif // XAUDIOTHREAD_H
