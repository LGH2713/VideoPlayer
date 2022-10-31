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

    virtual bool Write(const unsigned char *data, int datasize)
    {
        if(!data || datasize == 0)
            return false;
        mux.lock();
        if(!output || !io)
        {
            mux.unlock();
            return false;
        }

        int size = io->write((char *)data, datasize);

        mux.unlock();

        if(size != datasize)
            return false;

        return true;
    }

    virtual int GetFree()
    {
        mux.lock();
        if(!output)
        {
            mux.unlock();
            return 0;
        }
        int freeSize = output->bytesFree();
        mux.unlock();
        return freeSize;
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


