#include "BinaryPacketReader.h"
#include "File.h"
#include "Packet.h"
#include <iostream>
#include <cstring>
#include <boost/date_time/posix_time/posix_time.hpp>

BinaryPacketReader::BinaryPacketReader(char const* fileName) : _pktVersion(PKT_VERSION_NONE), _build(0), _startTime(0), _startTickCount(0)
{
    const char* found = strstr(fileName, ".pkt");
    _sniffType = found != NULL ? SNIFF_TYPE_PKT : SNIFF_TYPE_BIN;

    char* buffer;
    int length;
    if (!File::Load(fileName, buffer, length))
    {
        printf("Error loading sniff %s.\n", fileName);
        return;
    }

    _buffer = new ReadBuffer((uint8*)buffer, length);

    ReadHeader();
}

BinaryPacketReader::~BinaryPacketReader()
{
    delete _buffer;
}

void BinaryPacketReader::ReadHeader()
{
    std::string headerStart = _buffer->ReadString(3);

    if (headerStart != "PKT")
    {
        // pkt does not have a header
        _buffer->SetReadPosition(0);
        return;
    }

    _pktVersion = (PktVersion)_buffer->ReadUInt16();

    switch (_pktVersion)
    {
        case PKT_VERSION_V2_1:
        {
            _build = _buffer->ReadUInt16();
            _buffer->ReadBytes(40); // session key
            break;
        }
        case PKT_VERSION_V2_2:
        {
            _buffer->ReadUInt8();                       // sniffer id
            _build = _buffer->ReadUInt16();             // client build
            _buffer->ReadBytes(4);                      // client locale
            _buffer->ReadBytes(20);                     // packet key
            _buffer->ReadBytes(64);                     // realm name
            break;
        }
        case PKT_VERSION_V3_0:
        {
            uint8 snifferId = _buffer->ReadUInt8();      // sniffer id
            _build = _buffer->ReadUInt32();              // client build
            _buffer->ReadBytes(4);                       // client locale
            _buffer->ReadBytes(40);                      // session key
            int additionalLength = _buffer->ReadInt32();
            uint32 preAdditionalPos = _buffer->GetReadPosition();
            _buffer->ReadBytes(additionalLength);
            uint32 postAdditionalPos = _buffer->GetReadPosition();
            if (snifferId == 10)                         // xyla
            {
                _buffer->SetReadPosition(preAdditionalPos);

                _startTime = _buffer->ReadUInt32();      // start time
                _startTickCount = _buffer->ReadUInt32(); // start tick count

                _buffer->SetReadPosition(postAdditionalPos);
            }
            break;
        }
        case PKT_VERSION_V3_1:
        {
            _buffer->ReadUInt8();                         // sniffer id
            _build = _buffer->ReadUInt32();               // client build
            _buffer->ReadBytes(4);                        // client locale
            _buffer->ReadBytes(40);                       // session key
            _startTime = _buffer->ReadUInt32();           // start time
            _startTickCount = _buffer->ReadUInt32();      // start tick count
            int additionalLength = _buffer->ReadInt32();
            _buffer->ReadBytes(additionalLength);
            break;
        }
    }
}

Packet* BinaryPacketReader::Read()
{
    uint32 opcode;
    uint32 length;
    uint64 time;
    Direction direction;
    uint8* data;

    try
    {
        if (_sniffType == SNIFF_TYPE_PKT)
        {
            switch (_pktVersion)
            {
            case PKT_VERSION_V2_1:
            case PKT_VERSION_V2_2:
                {
                    direction = (_buffer->ReadUInt8() == 0xff) ? DIRECTION_SERVER_TO_CLIENT : DIRECTION_CLIENT_TO_SERVER;
                    time = _buffer->ReadInt32();
                    _buffer->ReadInt32(); // tick count
                    length = _buffer->ReadInt32();

                    if (direction == DIRECTION_SERVER_TO_CLIENT)
                    {
                        opcode = _buffer->ReadInt16();
                        data = new uint8[length - 2];
                        _buffer->ReadBytes(data, length - 2);
                        length -= 2;
                    }
                    else
                    {
                        opcode = _buffer->ReadInt32();
                        data = new uint8[length - 4];
                        _buffer->ReadBytes(data, length - 4);
                        length -= 4;
                    }

                    break;
                }
            case PKT_VERSION_V3_0:
            case PKT_VERSION_V3_1:
                {
                    uint32 dir = _buffer->ReadUInt32();

                    direction = (dir == 0x47534d53) ? DIRECTION_SERVER_TO_CLIENT : DIRECTION_CLIENT_TO_SERVER;

                    if (_pktVersion == PKT_VERSION_V3_0)
                    {
                        time = _buffer->ReadInt32();
                        uint32 tickCount = _buffer->ReadUInt32();
                        if (_startTickCount != 0)
                        {
                            _startTime = tickCount - _startTickCount;
                            time = _startTime;
                        }
                    }
                    else
                    {
                        _buffer->ReadUInt32(); // session id
                        uint32 tickCount = _buffer->ReadUInt32();
                        _startTime = tickCount - _startTickCount;
                        time = _startTime;
                    }

                    int additionalSize = _buffer->ReadInt32();
                    length = _buffer->ReadInt32();
                    _buffer->ReadBytes(additionalSize);
                    opcode = _buffer->ReadInt32();
                    data = new uint8[length-4];
                    _buffer->ReadBytes(data, length - 4);
                    length -= 4;
                    break;
                }
            default:
                {
                    opcode = _buffer->ReadUInt16();
                    length = _buffer->ReadInt32();
                    direction = (Direction)_buffer->ReadUInt8();
                    time = _buffer->ReadInt64();
                    data = new uint8[length];
                    _buffer->ReadBytes(data, length);
                    break;
                }
            }
        }
        else // bin
        {
            opcode = _buffer->ReadInt32();
            length = _buffer->ReadInt32();
            time = _buffer->ReadInt32();
            direction = (Direction)_buffer->ReadUInt8();
            data = new uint8[length];
            _buffer->ReadBytes(data, length);
        }

        Packet* packet = new Packet(data, length, opcode, time, direction);
        return packet;
    }
    catch (std::exception* /*e*/)
    {
    	return NULL;
    }
}
