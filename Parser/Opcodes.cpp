#include "Opcodes.h"
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <boost/lexical_cast.hpp>

Opcodes::Opcodes()
{
    _versionPath = "D:\\wow\\WowPacketParser\\WowPacketParser\\Enums\\Version\\";
    _opcodePaths[12340] = "V3_3_5a_12340\\Opcodes.cs";
    _opcodePaths[13329] = "V4_0_3_13329\\Opcodes.cs";
    _opcodePaths[13596] = "V4_0_6_13596\\Opcodes.cs";
    _opcodePaths[13914] = "V4_1_0_13914\\Opcodes.cs";
    _opcodePaths[14480] = "V4_2_0_14480\\Opcodes.cs";
    _opcodePaths[14545] = "V4_2_2_14545\\Opcodes.cs";
    _opcodePaths[15005] = "V4_3_0_15005\\Opcodes.cs";
    _opcodePaths[15211] = "V4_3_2_15211\\Opcodes.cs";
    _opcodePaths[15354] = "V4_3_3_15354\\Opcodes.cs";
    _opcodePaths[15595] = "V4_3_4_15595\\Opcodes.cs";
}

bool Opcodes::LoadOpcodes(uint32 build)
{
    if (_opcodePaths.find(build) == _opcodePaths.end())
        return false;

    std::string path = _versionPath + _opcodePaths[build];

    std::ifstream file(path);

    if (!file.is_open())
        return false;

    std::string line, opcodeName, numberStr;
    for (int i = 0; i < 13; ++i) // skip the first 13 lines
        std::getline(file, line);

    while (file.good() && line.find(';') == std::string::npos)
    {
        std::getline(file, line);

        std::string::size_type commaPos = line.find(',');
        std::string::size_type bracketR = line.find('{');
        std::string::size_type bracketL = line.find('}');

        if (commaPos == std::string::npos ||
              bracketR == std::string::npos ||
              bracketL == std::string::npos)
            continue;

        opcodeName = line.substr(bracketR + 8, commaPos - (bracketR + 8));
        numberStr = line.substr(commaPos + 2, bracketL - (commaPos + 2));

        int opcode;
        if (numberStr.size() > 1 && numberStr[1] == 'x') // hex
            opcode = strtoul(numberStr.substr(2).c_str(), NULL, 16);
        else // dec
            opcode = strtoul(numberStr.c_str(), NULL, 10);

        _opcodes[build][opcode] = opcodeName;
    }

    file.close();
    return true;
}
