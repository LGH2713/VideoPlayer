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

    virtual long long GetNoPlayMs()
    {
        mux.lock();
        if(!output)
        {
            mux.unlock();
            return 0;
        }

        long long pts = 0;
        // 还未播放的字节数
        double size = output->bufferSize() - output->bytesFree();
        // 一毫秒音频字节大小
        double secSize = output->format().bytesForDuration(1000);
        if(secSize <= 0)
        {
            pts = 0;
        }
        else
        {
            pts = (size / secSize);
        }
        mux.unlock();
        return pts;
    }

    virtual void SetPause(bool isPause)
    {
        mux.lock();
        if(!output)
        {
            mux.unlock();
            return;
        }
        if(isPause)
        {
            output->suspend();
        }
        else
        {
            output->resume();
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



