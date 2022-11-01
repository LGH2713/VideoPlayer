#ifndef XDEMUXTHREAD_H
#define XDEMUXTHREAD_H

#include <QThread>
#include <mutex>
#include "IVideoCall.h"

class XDemux;
class XVideoThread;
class XAudioThread;

class XDemuxThread: public QThread
{
public:
    XDemuxThread();
    ~XDemuxThread();

    // 创建对象并打开
     virtual bool Open(const char *url, IVideoCall *call);

    // 开启所有线程
    virtual void Start();

    // 关闭线程，清理资源
    virtual void Close();

    void run();

    bool isExit = false;

protected:
    std::mutex mux;
    XDemux *demux = 0;
    XVideoThread *vt = 0;
    XAudioThread *at = 0;
};

#endif // XDEMUXTHREAD_H
