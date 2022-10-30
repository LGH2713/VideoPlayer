#include "mainwindow.h"

#include <QApplication>
#include <QAudioFormat>
#include <QAudioSink>
#include <QMediaDevices>
#include <QAudioDevice>
#include <QThread>

#include "XDemux.h"
#include "XDecode.h"
#include "ui_mainwindow.h"

int main(int argc, char *argv[])
{
    XDemux demux;

    const char* url = "E:/me.mp4";

    demux.Open(url);
    demux.Read();
    demux.Clear();
    demux.Close();

    demux.Open(url);

    cout << "CopyVPara = " << demux.CopyVPara() << endl;
    cout << "CopyAPara = " << demux.CopyAPara() << endl;
    cout << "seek = " << demux.Seek(0.95) << endl;

    XDecode vdecode;
    cout << "vdecode.Open() = " << vdecode.Open(demux.CopyVPara()) << endl;
    vdecode.Clear();
    vdecode.Close();
    XDecode adecode;
    cout << "adecode.Open() = " << adecode.Open(demux.CopyAPara()) << endl;

    //    for(;;)
    //    {
    //        AVPacket *pkt = demux.Read();
    //        if(demux.IsAudio(pkt))
    //        {
    //            adecode.Send(pkt);
    //            AVFrame *frame = adecode.Recv();
    //        }
    //        else
    //        {
    //            vdecode.Send(pkt);
    //            AVFrame* frame = vdecode.Recv();
    //        }
    //        if(!pkt)
    //            break;
    //    }

    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    w.ui->video->Init(demux.width, demux.height);

    return a.exec();
}
