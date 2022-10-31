#ifndef XDEMUX_H
#define XDEMUX_H

struct AVFormatContext;
struct AVPacket;
struct AVCodecParameters;
#include <mutex>


class XDemux
{
public:
    XDemux();

    // 打开媒体文件，或者流媒体 rtmp http rstp
    virtual bool Open(const char *url);

    // 空间需要调用者释放，释放AVPacket对象空间和数据空间 av_packet_free;
    virtual AVPacket *Read();

    // 获取视频参数 返回的空间需要清理
    virtual AVCodecParameters *CopyVPara();

    // 获取音频参数 返回的空间需要清理
    virtual AVCodecParameters *CopyAPara();

    // seek 位置 pos 0.0~1.0
    virtual bool Seek(double pos);

    virtual bool IsAudio(AVPacket *pkt);

    // 清理和关闭函数
    virtual void Clear();
    virtual void Close();

    // 媒体总时长（毫秒）
    int totalMs = 0;

    int width = 0;
    int height = 0;

    int sampleRate = 0;
    int channels = 0;

protected:
    std::mutex mux;

    //解封装上下文
    AVFormatContext *ic = nullptr;

    // 音视频索引，读取时区分音视频
    int videoStream = -1;
    int audioStream = -1;
};

#endif // XDEMUX_H
