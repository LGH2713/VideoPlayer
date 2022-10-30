#include "mainwindow.h"

#include <QApplication>
#include <QAudioFormat>
#include <QAudioSink>
#include <QMediaDevices>
#include <QAudioDevice>
#include <QThread>

#include "XDemux.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();


    XDemux demux;

    const char* url = "E:/me.mp4";

    demux.Open(url);

    cout << "CopyVPara = " << demux.CopyVPara() << endl;
    cout << "CopyAPara = " << demux.CopyAPara() << endl;

    //    for(;;)
    //    {
    //        AVPacket *pkt = demux.Read();
    //        if(!pkt)
    //            break;
    //    }

    return a.exec();
}
