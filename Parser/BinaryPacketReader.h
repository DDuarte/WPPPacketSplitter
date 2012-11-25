#ifndef BINARYPACKETREADER_H
#define BINARYPACKETREADER_H

#include "ReadBuffer.h"

class Packet;

enum SniffType
{
    SNIFF_TYPE_BIN,
    SNIFF_TYPE_PKT
};

enum PktVersion
{
    PKT_VERSION_NONE = 0,
    PKT_VERSION_V2_1 = 0x201,
    PKT_VERSION_V2_2 = 0x202,
    PKT_VERSION_V3_0 = 0x300,
    PKT_VERSION_V3_1 = 0x301,
};

class BinaryPacketReader
{
public:
    BinaryPacketReader(char const* fileName);
    ~BinaryPacketReader();

    Packet* Read();

        bool CanRead() const { return _buffer->CanRead(); }

private:
    SniffType _sniffType;
    PktVersion _pktVersion;
    uint32 _build;

    uint32 _startTime;
    uint32 _startTickCount;

    ReadBuffer* _buffer;
    void ReadHeader();
};

#endif // BINARYPACKETREADER_H
