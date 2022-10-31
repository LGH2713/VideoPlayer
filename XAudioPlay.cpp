#include "XAudioPlay.h"

#include <QMediaDevices>
#include <QAudioFormat>
#include <QAudioSink>
#include <mutex>


class CXAudioPlay : public XAudioPlay
{
public:
    QAudioSink *output  = nullptr;
    QIODevice *io = nullptr;
    std::mutex mux;
    virtual bool Open()
    {
        Close();
        QMediaDevices *devices = new QMediaDevices();
        QAudioDevice device = devices->defaultAudioOutput();

        QAudioFormat fmt = device.preferredFormat();

        mux.lock();
        output = new QAudioSink(device, fmt);
        io = output->start();
        mux.unlock();

        if(io)
            return true;
        return false;
    }

    virtual void Close()
    {
        mux.lock();
        if(io)
        {
            io->close();
            io = nullptr;
        }
        if(output)
        {
            output->stop();
            delete output;
            output = 0;
        }
        mux.unlock();
    }
};

XAudioPlay::XAudioPlay()
{

}

XAudioPlay::~XAudioPlay()
{

}

XAudioPlay *XAudioPlay::Get()
{
    static CXAudioPlay play;
    return &play;
}


