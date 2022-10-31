#ifndef XAUDIOTHREAD_H
#define XAUDIOTHREAD_H

#include <QThread>
#include <mutex>
struct AVCodecParameters;
class XDecode;
class XAudioPlay;
class XResample;

class XAudioThread : public QThread
{
public:
    XAudioThread();
    ~XAudioThread();

    void run();
    virtual bool Open(AVCodecParameters *para);

protected:
    XDecode *decode = 0;
    XAudioPlay *ap = 0;
    XResample *res = 0;
    std::mutex mux;
};

#endif // XAUDIOTHREAD_H
