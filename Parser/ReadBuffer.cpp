#include "ReadBuffer.h"
#include <cstring>
#include <string>
#include "Types.h"

ReadBuffer::ReadBuffer(uint8* data, uint32 length)
    : _readPos(0)
{
    _size = length;
    _buffer = data;
    //memcpy(_buffer, data, _size);
}

ReadBuffer::ReadBuffer(const std::vector<uint8>& data)
    : _readPos(0)
{
    _size = data.size();
    _buffer = new uint8[_size];
    memcpy(_buffer, &data[0], _size);
}

std::string ReadBuffer::ReadCString()
{
    std::string str;

    while (_readPos < _size)
    {
        char c = Read<char>();
        if (c == 0)
            break;
        str += c;
    }

    return str;
}

std::string ReadBuffer::ReadString(uint32 length)
{
    char* c = new char[length];
    ReadBytes((uint8*)c, length);
    std::string str(c, 3);
    delete[] c;

    return str;
}
