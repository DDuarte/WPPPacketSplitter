#ifndef PACKET_H
#define PACKET_H

#include "ReadBuffer.h"
#include "Types.h"
#include <boost/date_time/posix_time/posix_time.hpp>

using boost::posix_time::ptime;

enum Direction
{
    DIRECTION_CLIENT_TO_SERVER = 0,
    DIRECTION_SERVER_TO_CLIENT = 1
};

class Packet : public ReadBuffer
{
protected:
    uint32 _opcode;
    uint64 _time;
    Direction _direction;

public:
    Packet(uint8* data, uint32 length, uint32 opcode, uint64 time, Direction direction)
        : ReadBuffer(data, length), _opcode(opcode), _time(time), _direction(direction) {}
    Packet(std::vector<uint8>& data, uint32 opcode, uint64 time, Direction direction)
        : ReadBuffer(data), _opcode(opcode), _time(time), _direction(direction) {}

    uint32 GetOpcode() const { return _opcode; }
    uint64 GetDateTime() const { return _time; }
    Direction GetDirection() const { return _direction; }
};

#endif // PACKET_H
