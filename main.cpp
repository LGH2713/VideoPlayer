#include "mainwindow.h"

#include <QApplication>
#include <QAudioFormat>
#include <QAudioSink>
#include <QMediaDevices>
#include <QAudioDevice>
#include <QThread>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    /*
    QMediaDevices *devices = new QMediaDevices();
    QAudioDevice device = devices->defaultAudioOutput();

    QAudioFormat fmt = device.preferredFormat();

    QAudioSink *audioSink = new QAudioSink(device, fmt);

    QIODevice *io = audioSink->start();

    int size = 24;
    char *buf = new char[size];

    cout << size << endl;

    FILE *fp = fopen("E:/out.pcm", "rb");
    while(!feof(fp))
    {
        if(audioSink->bufferSize() < size)
        {
            QThread::msleep(1);
            continue;
        }
        int len = fread(buf, 1, size, fp);
        if(len <= 0)
            break;
        io->write(buf,len);
    }

    fclose(fp);
    delete[] buf;
    buf = 0;

    */
    return a.exec();
}
