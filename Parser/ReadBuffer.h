#ifndef READBUFFER_H
#define READBUFFER_H

#include <vector>
#include <string>
#include <stdexcept>
#include "Types.h"

class ReadBuffer
{
    class EndOfStreamException : public std::runtime_error
    {
    public:
        EndOfStreamException() : runtime_error("Unable to read beyond the end of the stream.") {}
    };

private:
    uint8* _buffer;
    uint32 _size;
    uint32 _readPos;

public:
    ReadBuffer(uint8* data, uint32 length);
    ReadBuffer(const std::vector<uint8>& data);
    ~ReadBuffer() { delete[] _buffer; }

    bool ReadBool() { return Read<bool>(); }
    int8 ReadInt8() { return Read<int8>(); }
    int16 ReadInt16() { return Read<int16>(); }
    int32 ReadInt32() { return Read<int32>(); }
    int64 ReadInt64() { return Read<int64>(); }
    uint8 ReadUInt8() { return Read<uint8>(); }
    uint16 ReadUInt16() { return Read<uint16>(); }
    uint32 ReadUInt32() { return Read<uint32>(); }
    uint64 ReadUInt64() { return Read<uint64>(); }
    void ReadBytes(uint8* dest, uint32 count) { memcpy(dest, _buffer+_readPos, count); _readPos += count; }
    void ReadBytes(uint32 count) { _readPos += count; }
    std::string ReadCString(); // null terminated
    std::string ReadString(uint32 length); // not null terminated

    void SkipToEnd() { _readPos = _size/* - 1*/; }

    void SetReadPosition(uint32 pos) { _readPos = pos; }
    uint32 GetReadPosition() const { return _readPos; }
    uint32 Size() const { return _size; }
    bool IsEmpty() const { return _size == 0; }
    bool CanRead() const { return GetReadPosition() != Size(); }

    uint8* GetBuffer() const { return _buffer; }

protected:
    template <typename T> T Read()
    {
        T val = Read<T>(_readPos);
        _readPos += sizeof(T);
        return val;
    }

private:
    template <typename T> T Read(uint32 position) const
    {
        if (position + sizeof(T) > Size())
            throw EndOfStreamException();
        T value = *((T const*)&_buffer[position]);
        return value;
    }
};

#endif // READBUFFER_H
