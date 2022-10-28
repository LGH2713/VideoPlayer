#include "mainwindow.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

static double r2d(AVRational r) {
    return r.den == 0 ? 0 : static_cast<double>(r.num) / static_cast<double>(r.den);
}

void XSleep(int ms) {
    chrono::milliseconds du(ms);
    this_thread::sleep_for(du);
}

int running()
{
    cout << "Test Demux FFmpeg.club" << endl;

    const char *path = "E:\\me.mp4";

    //初始化网络库 （可以打开rtsp rtmp http 协议的流媒体视频）
    avformat_network_init();

    //参数设置
    AVDictionary *opts = NULL;
    //设置rtsp流已tcp协议打开
    av_dict_set(&opts, "rtsp_transport", "tcp", 0);

    //网络延时时间
    av_dict_set(&opts, "max_delay", "500", 0);


    //解封装上下文
    AVFormatContext *ic = NULL;
    int ret = avformat_open_input(
                &ic,
                path,
                0,  // 0表示自动选择解封器
                &opts //参数设置，比如rtsp的延时时间
                );
    if (ret != 0)
    {
        char buf[1024] = { 0 };
        av_strerror(ret, buf, sizeof(buf) - 1);
        cout << "open " << path << " failed! :" << buf << endl;
        getchar();
        return -1;
    }
    cout << "open " << path << " success! " << endl;

    //获取流信息
    ret = avformat_find_stream_info(ic, 0);

    //总时长 毫秒
    int totalMs = ic->duration / (AV_TIME_BASE / 1000);
    cout << "totalMs = " << totalMs << endl;

    //打印视频流详细信息
    av_dump_format(ic,0,path,0);

    int videoStream = -1;
    int audioStream = -1;

    for(int i = 0; i < ic->nb_streams; i++) {
        AVStream *as = ic->streams[i];
        if(as->codecpar->codec_type == AVMEDIA_TYPE_AUDIO) {
            audioStream = i;
            cout << i << " audio information" << endl;
            cout << "sample_rate = " << as->codecpar->sample_rate << endl;
            cout << "format = " << as->codecpar->format << endl;
            cout << "nb_channels = " << as->codecpar->ch_layout.nb_channels << endl;
            cout << "codec_id = " << as->codecpar->codec_id << endl;
            cout << "frame_size = " << as->codecpar->frame_size << endl;
        }
        else if(as->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
            videoStream = i;
            cout << i << " video information" << endl;
            cout << "width = " << as->codecpar->width << endl;
            cout << "height = " << as->codecpar->height << endl;
            cout << "video fps = " << r2d(as->avg_frame_rate) << endl;
        }
    }


    ///////////////////////////////////////////////////////////////////
    /// 寻找视频解码器
    AVCodec const *vcodec = avcodec_find_decoder(ic->streams[videoStream]->codecpar->codec_id);

    if(!vcodec) {
        cout << "can't find codec id" << endl;
        getchar();
        return -1;
    }

    cout << "Find codec id = " << ic->streams[videoStream]->codecpar->codec_id << endl;

    //创建解码器上下文
    AVCodecContext *vc = avcodec_alloc_context3(vcodec);
    // 配置解码器上下文参数
    avcodec_parameters_to_context(vc, ic->streams[videoStream]->codecpar);
    // 设置解码线程数
    vc->thread_count = 8;

    // 打开解码器上下文
    ret = avcodec_open2(vc, 0, 0);
    if(ret != 0) {
        char buf[1024] = {0};
        av_strerror(ret, buf, sizeof(buf) - 1);
        cout << "avcodec_open2 failed : " << buf << endl;
        getchar();
        return -1;
    }
    cout << "video avcodec_open2 success" << endl;


    ///////////////////////////////////////////////////////////////////
    /// 寻找音频解码器
    AVCodec const *acodec = avcodec_find_decoder(ic->streams[audioStream]->codecpar->codec_id);

    if(!acodec) {
        cout << "can't find codec id" << endl;
        getchar();
        return -1;
    }

    cout << "Find codec id = " << ic->streams[audioStream]->codecpar->codec_id << endl;

    //创建解码器上下文
    AVCodecContext *ac = avcodec_alloc_context3(acodec);
    // 配置解码器上下文参数
    avcodec_parameters_to_context(ac, ic->streams[audioStream]->codecpar);
    // 设置解码线程数
    ac->thread_count = 8;

    // 打开解码器上下文
    ret = avcodec_open2(ac, 0, 0);
    if(ret != 0) {
        char buf[1024] = {0};
        av_strerror(ret, buf, sizeof(buf) - 1);
        cout << "avcodec_open2 failed : " << buf << endl;
        getchar();
        return -1;
    }
    cout << "audio avcodec_open2 success" << endl;


    AVPacket *pkt = av_packet_alloc();
    AVFrame *frame = av_frame_alloc();
    // 像素格式转换上下文
    SwsContext *vctx = NULL;
    unsigned char *rgb = NULL;

    // 音频重采样 上下文初始化
    SwrContext *actx = swr_alloc();
    unsigned char *pcm = NULL;
    ret = swr_alloc_set_opts2(
                &actx,              // 重采样上下文
                &ac->ch_layout,     // 输出格式
                AV_SAMPLE_FMT_S16,  // 输出样本格式
                ac->sample_rate,    // 输出采样率
                &ac->ch_layout,     // 输入格式
                ac->sample_fmt,     // 输入样本格式
                ac->sample_rate,    // 输入采样率
                0, 0
                );
    if(ret != 0) {
        char buf[1024] = {0};
        av_strerror(ret, buf, sizeof(buf) - 1);
        cout << "swr_alloc_set_opts2 failed!" << endl;
        getchar();
        return -1;
    }

    cout << "swr_alloc_set_opts2 success " << ret << endl;

    ret = swr_init(actx);
    if(ret != 0) {
        char buf[1024] = {0};
        av_strerror(ret, buf, sizeof(buf) - 1);
        cout << "swr_init failed!" << endl;
        getchar();
        return -1;
    }


    for(;;) {
        int ret = av_read_frame(ic, pkt);
        if(ret != 0) {
            cout << "========================= end ==============================" << endl;
            int ms = 3000; // 三秒位置 根据时间基进行转换
            long long pos = static_cast<double>(ms) / static_cast<double>(1000) * r2d(ic->streams[pkt->stream_index]->time_base);
            av_seek_frame(ic, videoStream, pos, AVSEEK_FLAG_FRAME | AVSEEK_FLAG_BACKWARD);
            continue;
        }

        cout << "pkt->size = " << pkt->size << endl;
        // 显示的时间
        cout << "pkt->pts = " << pkt->pts << endl;

        // 转换成毫秒，方便做同步
        cout << "pkt->pts ms = " << pkt->pts * (r2d(ic->streams[pkt->stream_index]->time_base)) << endl;

        // 解码时间
        cout << "pkt->dts = " << pkt->dts << endl;
        AVCodecContext *cc = 0;
        if(pkt->stream_index == videoStream) {
            cout << "video: " << endl;
            cc = vc;
        }
        if(pkt->stream_index == audioStream) {
            cout << "audio: " << endl;
            cc = ac;
        }

        /// 解码线程
        // 发送packet到解码线程 send传NULL后多次调用receive取出所有缓冲帧
        ret = avcodec_send_packet(cc, pkt);
        // 释放，引用计数-1 为0释放空间
        av_packet_unref(pkt);

        if(ret != 0) {
            char buf[1024] = {0};
            av_strerror(ret, buf, sizeof(buf) - 1);
            cout << "avcodec_send_packet failed" << buf << endl;
            continue;
        }

        for(;;) {
            // 从线程中获取解码接口，一次send可能对应多次receive
            ret = avcodec_receive_frame(cc, frame);
            if(ret != 0)
                break;
            cout << "recv frame " << frame->format << " " << frame->linesize[0] << endl;

            // 视频
            if(cc == vc) {
                vctx = sws_getCachedContext(
                            vctx, // 传NULL会新创建
                            frame->width, frame->height, // 输入的宽高
                            static_cast<AVPixelFormat>(frame->format), // 输出格式 YUV420P
                            frame->width, frame->height, // 输出的宽高
                            AV_PIX_FMT_RGBA, // 输出的格式RGBA
                            SWS_BILINEAR, // 尺寸变化算法
                            0, 0, 0
                            );
                //                if(vctx != NULL) {
                //                    cout << "SwsContext create successfully" << endl;
                //                }

                if(vctx) {
                    // 像素格式转换代码
                    if(!rgb)
                        rgb = new unsigned char[frame->width * frame->height * 4];
                    uint8_t *data[2] = {0};
                    data[0] = rgb;
                    int lines[2] = {0};
                    lines[0] = frame->width * 4;
                    // 该函数消耗大
                    ret = sws_scale( // 该函数返回转换后高度
                                     vctx,
                                     frame->data,    // 输入数据
                                     frame->linesize, // 输入行大小
                                     0,
                                     frame->height, // 输入高度
                                     data,   // 输出数据和大小
                                     lines
                                     );
                    cout << "sws_scale = " << ret << endl;
                }
            }
            else {
                uint8_t *data[2] = {0};
                if(!pcm)
                    pcm = new uint8_t[frame->nb_samples * ac->ch_layout.nb_channels * 2];
                data[0] = pcm;
                ret = swr_convert(actx,
                                  data, frame->nb_samples,
                                  const_cast<const uint8_t**>(frame->data), frame->nb_samples
                                  );
                cout << "swr_convert = " << ret << endl;
            }
        }

        XSleep(500);
    }

    av_frame_free(&frame);
    av_packet_free(&pkt);


    if (ic)
    {
        //释放封装上下文，并且把ic置0
        avformat_close_input(&ic);
    }

    getchar();
}
