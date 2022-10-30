#ifndef XRESAMPLE_H
#define XRESAMPLE_H
struct AVCodecParameters;

#include <mutex>
#include <iostream>

extern "C" {
#include <libswresample/swresample.h>
#include <libavcodec//avcodec.h>
}

class XResample
{
public:
    XResample();

    // 输出参数和输入参数一致除了采样格式，输出为S16
    virtual bool Open(AVCodecParameters *para);

    virtual void Close();

    // 返回重采样后大小
    virtual int Resample(AVFrame *indata, unsigned char *d);

    // AV_SAMPLE_FMT_S16
    int outFormat = 1;

protected:
    std::mutex mux;

    SwrContext *actx;
};

#endif // XRESAMPLE_H
