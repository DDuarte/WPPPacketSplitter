#include "File.h"
#include <cstdio>

namespace File
{
    bool Load(const char* fileName, char*& buffer, int& size)
    {
        if (!fileName)
            return false;

        FILE* file = fopen(fileName, "rb");
        if (!file)
            return false;

        // Get file size
        fseek(file, 0, SEEK_END);
        size = ftell(file);
        rewind(file);

        if (!size)
        {
            fclose(file);
            return false;
        }

        buffer = new char[size];
        if (!buffer)
        {
            fclose(file);
            return false;
        }

        size_t result = fread(buffer, sizeof(char), size, file);
        if (result != size)
        {
            fclose(file);
            return false;
        }

        fclose(file);
        return true;
    }

    bool Save(const char* fileName, const char* buffer, int size)
    {
        if (!fileName || !buffer || !size)
            return false;

        FILE* file = fopen(fileName, "wb");
        if (!file)
            return false;

        size_t result = fwrite(buffer, sizeof(char), size, file);
        if (result != size)
        {
            fclose(file);
            return false;
        }

        fclose(file);
        return true;
    }

    bool Append(const char* fileName, const char* buffer, int size)
    {
        if (!fileName || !buffer || !size)
            return false;

        FILE* file = fopen(fileName, "ab");
        if (!file)
            return false;

        size_t result = fwrite(buffer, sizeof(char), size, file);
        if (result != size)
        {
            fclose(file);
            return false;
        }

        fclose(file);
        return true;
    }
}
