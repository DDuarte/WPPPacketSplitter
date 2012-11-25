#ifndef FILE_H
#define FILE_H

#define _CRT_SECURE_NO_WARNINGS 1

namespace File
{
    bool Load(const char* fileName, char*& buffer, int& size);
    bool Save(const char* fileName, const char* buffer, int size);
    bool Append(const char* fileName, const char* buffer, int size);
}

#endif // FILE_H
