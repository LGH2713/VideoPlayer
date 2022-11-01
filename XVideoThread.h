#ifndef XVIDEOTHREAD_H
#define XVIDEOTHREAD_H

#include "IVideoCall.h"

// 解码和显示视频
struct AVPacket;
class XDecode;
struct AVCodecParameters;

#include <list>
#include <mutex>
#include <QThread>

class XVideoThread: public QThread
{
public:
    XVideoThread();
    virtual ~XVideoThread();

    // 打开，不管成功与否都清理
    virtual bool Open(AVCodecParameters *para, IVideoCall *call, int width, int height);
    virtual void Push(AVPacket *pkt);
    void run();

    //最大队列
    int maxList = 100;
    bool isExit = false;
    // 同步时间由外部传入
    long long synpts = 0;

protected:
    std::list <AVPacket *> packs;
    std::mutex mux;
    XDecode *decode = 0;
    IVideoCall *call = 0;
};

#endif // XVIDEOTHREAD_H
