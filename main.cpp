#include "mainwindow.h"

#include <QApplication>
#include <QAudioFormat>
#include <QAudioOutput>
#include <QThread>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    //    w.show();

    QAudioFormat fmt;
    fmt.setSampleRate(44100);
    fmt.setChannelCount(2);

    QAudioOutput *out = new QAudioOutput();
//    QIODevice *io = out->start();

    return a.exec();
}
