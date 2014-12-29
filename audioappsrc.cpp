#include "audioappsrc.h"

AudioAppSrc::AudioAppSrc(QObject *parent) :
    QObject(parent), QGst::Utils::ApplicationSource()
{
    enableBlock(false);
    setStreamType(QGst::AppStreamTypeStream);
    setLive(true);
    preAlloc = false;
}

void AudioAppSrc::needData(uint length)
{
    //qDebug() << "SOURCE NEED DATA. Length:" << length;

    if (preAlloc)
    {
        // Create and allocate the buffer here so we don't need to malloc the space twice and shuffle the data between threads too often
        buffer = QGst::Buffer::create(length);
        buffer->map(mapInfo, QGst::MapWrite);

        emit sigNeedData(length, (char*)mapInfo.data());
    }
    else
    {
        emit sigNeedData(length, 0);
    }
}

void AudioAppSrc::enoughData()
{
    qDebug() << "SOURCE ENOUGH DATA";
}

void AudioAppSrc::pushAudioBuffer()
{
    if ((!preAlloc) || (buffer.isNull()))
    {
        return;
    }

    buffer->unmap(mapInfo);

    pushBuffer(buffer);
}

void AudioAppSrc::pushAudioBuffer(QByteArray data)
{
    if (preAlloc)
    {
        return;
    }

    QGst::BufferPtr buf = QGst::Buffer::create(data.size());
    QGst::MapInfo map;
    buf->map(map, QGst::MapWrite);
    memcpy(map.data(), data.data(), map.size());
    buf->unmap(map);

    pushBuffer(buf);
}