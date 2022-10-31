#include "mainwindow.h"

#include <QApplication>
#include <QAudioFormat>
#include <QAudioSink>
#include <QMediaDevices>
#include <QAudioDevice>
#include <QThread>

#include "XDemux.h"
#include "XDecode.h"
#include "XResample.h"
#include "XAudioPlay.h"
#include "ui_mainwindow.h"

class TestThread: public QThread
{
public:
    void Init()
    {
        const char* url = "E:/rainSun.mp4";

        demux.Open(url);
        demux.Read();
        demux.Clear();
        demux.Close();

        demux.Open(url);

        cout << "CopyVPara = " << demux.CopyVPara() << endl;
        cout << "CopyAPara = " << demux.CopyAPara() << endl;
//        cout << "seek = " << demux.Seek(0.95) << endl;

        cout << "vdecode.Open() = " << vdecode.Open(demux.CopyVPara()) << endl;
        //        vdecode.Clear();
        //        vdecode.Close();

        cout << "adecode.Open() = " << adecode.Open(demux.CopyAPara()) << endl;
        cout << "resample.Open() = " << resample.Open(demux.CopyAPara()) << endl;

        XAudioPlay::Get()->sampleRate = demux.sampleRate;
        XAudioPlay::Get()->channels = demux.channels;
        cout << "XAudioPlay::Get()->Open() = " << XAudioPlay::Get()->Open() << endl;
    }

    unsigned char *pcm = new unsigned char[1024 * 1024];
    void run()
    {
        for(;;)
        {
            AVPacket *pkt = demux.Read();
            if(demux.IsAudio(pkt))
            {
                adecode.Send(pkt);
                AVFrame *frame = adecode.Recv();
                cout << "Resample = " << resample.Resample(frame, pcm) << endl;
            }
            else
            {
                vdecode.Send(pkt);
                AVFrame* frame = vdecode.Recv();
                video->Repaint(frame);
                msleep(40);
            }
            if(!pkt)
                break;
        }
    }

    // 测试demux
    XDemux demux;
    // 解码测试
    XDecode adecode;
    XDecode vdecode;

    XResample resample;

    XVideoWidget *video;
};

int main(int argc, char *argv[])
{

    TestThread tt;
    tt.Init();

    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    w.ui->video->Init(tt.demux.width, tt.demux.height);
    tt.video = w.ui->video;
    tt.start();

    return a.exec();
}
