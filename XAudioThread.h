#ifndef XAUDIOTHREAD_H
#define XAUDIOTHREAD_H

#include <QThread>
#include <mutex>
#include <list>
struct AVCodecParameters;
class XDecode;
class XAudioPlay;
class XResample;
struct AVPacket;

class XAudioThread : public QThread
{
public:
    XAudioThread();
    ~XAudioThread();

    void run();
    virtual bool Open(AVCodecParameters *para);
    virtual void Push(AVPacket *pkt);

    // 最大队列
    int maxList = 100;
    bool isExit = false;

protected:
    std::list <AVPacket *> packs;
    XDecode *decode = 0;
    XAudioPlay *ap = 0;
    XResample *res = 0;
    std::mutex mux;
};

#endif // XAUDIOTHREAD_H
