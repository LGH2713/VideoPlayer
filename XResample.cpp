#include "XResample.h"
extern "C" {
#include <libswresample/swresample.h>
#include <libavcodec//avcodec.h>
}

using namespace std;

XResample::XResample()
{

}

bool XResample::Open(AVCodecParameters *para, bool isClearPara)
{
    if(!para)
        return false;

    mux.lock();

    int ret = -1;
    // 音频重采样 上下文初始化
    actx = swr_alloc();
    unsigned char *pcm = NULL;
    ret = swr_alloc_set_opts2(
                &actx,              // 重采样上下文
                &para->ch_layout,     // 输出格式
                static_cast<AVSampleFormat>(outFormat),  // 输出样本格式
                para->sample_rate,    // 输出采样率
                &para->ch_layout,     // 输入格式
                static_cast<AVSampleFormat>(para->format),     // 输入样本格式
                para->sample_rate,    // 输入采样率
                0, 0
                );

    if(isClearPara)
        avcodec_parameters_free(&para);

    if(ret != 0) {
        mux.unlock();
        char buf[1024] = {0};
        av_strerror(ret, buf, sizeof(buf) - 1);
        cout << "swr_alloc_set_opts2 failed!" << endl;
        return false;
    }

    cout << "swr_alloc_set_opts2 success " << ret << endl;

    ret = swr_init(actx);
    mux.unlock();

    if(ret != 0) {
        char buf[1024] = {0};
        av_strerror(ret, buf, sizeof(buf) - 1);
        cout << "swr_init failed!" << endl;
        getchar();
        return -1;
    }

    return true;
}

void XResample::Close()
{
    mux.lock();
    if(actx)
        swr_free(&actx);

    mux.unlock();
}

int XResample::Resample(AVFrame *indata, unsigned char *d)
{
    if(!indata)
        return 0;
    if(!d)
    {
        av_frame_free(&indata);
        return 0;
    }

    uint8_t *data[2] = {0};
    data[0] = d;
    int ret = swr_convert(actx,
                          data, indata->nb_samples,
                          const_cast<const uint8_t**>(indata->data), indata->nb_samples
                          );

    int outSize = ret * indata->ch_layout.nb_channels * av_get_bytes_per_sample(static_cast<AVSampleFormat>(outFormat));

    av_frame_free(&indata);
    if(ret < 0)
        return ret;

    return outSize;
}
