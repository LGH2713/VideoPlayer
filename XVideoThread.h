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

#include "XDecodeThread.h"

class XVideoThread: public XDecodeThread
{
public:
    XVideoThread();
    virtual ~XVideoThread();

    // 打开，不管成功与否都清理
    virtual bool Open(AVCodecParameters *para, IVideoCall *call, int width, int height);
    void run();

    // 解码pts，如果接收到的解码数据pts >= seekpts return true 并且显示画面
    virtual bool RepaintPts(AVPacket *pkt, long long seekpts);

    //最大队列
    int maxList = 100;
    bool isExit = false;
    // 同步时间由外部传入
    long long synpts = 0;

    void SetPause(bool isPause);
    bool isPause = false;

protected:
    std::mutex vmux;
    IVideoCall *call = 0;

};

#endif // XVIDEOTHREAD_H
