#ifndef XDECODE_H
#define XDECODE_H

struct AVCodecParameters;
struct AVCodecContext;
struct AVPacket;
struct AVFrame;

#include <mutex>

extern void XFreePacket(AVPacket **pkt);
extern void XFreeFrame(AVFrame **frame);

class XDecode
{
public:
    XDecode();

    bool isAudio = false;

    // 当前解码到的pts
    long long pts = 0;

    // 打开解码器 并释放para空间
    virtual bool Open(AVCodecParameters *para);

    // 发送到解码线程，不管成功与否都要释放pkt空间（对象和媒体内容）
    virtual bool Send(AVPacket *pkt);

    // 获取解码数据，一次send可能需要多次Recv，获取缓冲中的数据Send NULL在Recv多次
    // 每复制一份，由调用者释放av_frame_free
    virtual AVFrame* Recv();

    virtual void Close();
    virtual void Clear();

protected:
    AVCodecContext *codec = 0;
    std::mutex mux;
};

#endif // XDECODE_H
