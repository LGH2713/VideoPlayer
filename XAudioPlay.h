#ifndef XAUDIOPLAY_H
#define XAUDIOPLAY_H


class XAudioPlay
{
public:
    XAudioPlay();
    virtual ~XAudioPlay();

    static XAudioPlay *Get();

    // 打开音频播放
    virtual bool Open() = 0;
    // 关闭音频播放
    virtual void Close() = 0;
    virtual void Clear() = 0;

    // 返回缓冲中还没有播放的时间（毫秒）
    virtual long long GetNoPlayMs() = 0;
    // 播放音频
    virtual bool Write(const unsigned char *data, int datasize) = 0;
    virtual int GetFree() = 0;

    virtual  void SetPause(bool isPause) = 0;

    int sampleRate = 44100;
    int channels = 2;
};

#endif // XAUDIOPLAY_H
