#include "mainwindow.h"

#include <QApplication>
#include <QAudioFormat>
#include <QAudioSink>
#include <QMediaDevices>
#include <QAudioDevice>
#include <QThread>

#include "ui_mainwindow.h"
#include "XDemux.h"
#include "XAudioThread.h"
#include "XVideoThread.h"
#include "XDemuxThread.h"

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

        //        cout << "CopyVPara = " << demux.CopyVPara() << endl;
        //        cout << "CopyAPara = " << demux.CopyAPara() << endl;
        //        cout << "seek = " << demux.Seek(0.95) << endl;

        //        cout << "vdecode.Open() = " << vdecode.Open(demux.CopyVPara()) << endl;
        //        vdecode.Clear();
        //        vdecode.Close();

        //        cout << "adecode.Open() = " << adecode.Open(demux.CopyAPara()) << endl;
        //        cout << "resample.Open() = " << resample.Open(demux.CopyAPara()) << endl;

        //        XAudioPlay::Get()->sampleRate = demux.sampleRate;
        //        XAudioPlay::Get()->channels = demux.channels;
        //        cout << "XAudioPlay::Get()->Open() = " << XAudioPlay::Get()->Open() << endl;

        cout << "at.Open() = " << at.Open(demux.CopyAPara()) << endl;
        cout << "vt.Open() = " << vt.Open(demux.CopyVPara(), video, demux.width, demux.height);
        at.start();
        vt.start();
    }

    unsigned char *pcm = new unsigned char[1024 * 1024];
    void run()
    {
        for(;;)
        {
            AVPacket *pkt = demux.Read();
            if(demux.IsAudio(pkt))
            {
                at.Push(pkt);
                /*
                adecode.Send(pkt);
                AVFrame *frame = adecode.Recv();
                int len = resample.Resample(frame, pcm);
                cout << "Resample = " << len << endl;
                while(len > 0)
                {
                    if(XAudioPlay::Get()->GetFree() >= len)
                    {
                        XAudioPlay::Get()->Write(pcm, len);
                        break;
                    }
                    msleep(1);
                }
                */

            }
            else
            {
                vt.Push(pkt);
                //          vdecode.Send(pkt);
                //            AVFrame* frame = vdecode.Recv();
                //              video->Repaint(frame);
                 msleep(1);
            }
            if(!pkt)
                break;
        }
    }

    // 测试demux
    XDemux demux;
    // 解码测试
    // XDecode adecode;
    // XDecode vdecode;

    // XResample resample;

    XVideoWidget *video = 0;

    XAudioThread at;
    XVideoThread vt;
};

int main(int argc, char *argv[])
{

    // TestThread tt;

    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    //tt.video = w.ui->video;
    //tt.Init();
    //tt.start();

    XDemuxThread dt;
    dt.Open("E:/loneyRock.mp4", w.ui->video);
    dt.Start();

    return a.exec();
}
