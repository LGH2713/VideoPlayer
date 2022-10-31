#ifndef XDEMUXTHREAD_H
#define XDEMUXTHREAD_H

#include <QThread>
#include "IVideoCall.h"

class XDemuxThread: public QThread
{
public:
    XDemuxThread();
    ~XDemuxThread();
    bool Open(const char *url, IVideoCall *call);

};

#endif // XDEMUXTHREAD_H
