#ifndef OPCODES_H
#define OPCODES_H

#include <map>
#include <string>
#include "Types.h"
#include <boost/lexical_cast.hpp>

class Opcodes
{
public:
    Opcodes();
    inline std::string GetOpcodeName(uint32 build, int32 id)
    {
        if (_opcodes.find(build) == _opcodes.end())
            if (!LoadOpcodes(build))
                return boost::lexical_cast<std::string>(id);

        std::string name = _opcodes[build][id];
        return name.empty() ? boost::lexical_cast<std::string>(id) : name;
    }

private:
    //       build          opcode id     name
    std::map<uint32, std::map<int32, std::string>> _opcodes;

    bool LoadOpcodes(uint32 build);

    std::map<uint32, std::string> _opcodePaths;
    std::string _versionPath;
};
#endif // OPCODES_H
