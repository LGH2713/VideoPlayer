#ifndef XDECODETHREAD_H
#define XDECODETHREAD_H

// 解码和显示视频
struct AVPacket;
class XDecode;

#include <list>
#include <mutex>
#include <QThread>

class XDecodeThread : public QThread
{
public:
    XDecodeThread();
    virtual ~XDecodeThread();
    virtual void Push(AVPacket *pkt);
    // 取出一帧数据，并出栈，如果没有则返回nullptr
    virtual AVPacket *Pop();
    // 清理队列
    virtual void Clear();
    // 清理资源，停止线程
    virtual void Close();

    //最大队列
    int maxList = 100;
    bool isExit = false;

    // 解码器
    XDecode *decode = 0;

protected:
    std::list<AVPacket *> packs;
    std::mutex mux;
};

#endif // XDECODETHREAD_H
