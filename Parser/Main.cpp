#include "BinaryPacketReader.h"
#include "Packet.h"

#include <list>
#include <stdlib.h>
#include <cstring>
#include "File.h"
#include <map>
#include <forward_list>
#include <fstream>
#include <boost/timer/timer.hpp>
#include <sys/types.h>
#include "dirent.h"
#include <errno.h>
#include <vector>
#include <string>
#include <iostream>

#include "Opcodes.h"

int getdir (std::string dir, std::vector<std::string> &files)
{
    DIR *dp;
    struct dirent *dirp;
    if((dp  = opendir(dir.c_str())) == NULL) {
        std::cout << "Error(" << errno << ") opening " << dir << std::endl;
        return errno;
    }

    while ((dirp = readdir(dp)) != NULL) {
        files.push_back(std::string(dirp->d_name));
    }
    closedir(dp);
    return 0;
}

int main(int argc, char* argv[])
{
    std::vector<std::string> files;
    //files.reserve(650);

    int a = getdir("D:/wow/sniffsRat/434", files);

    Opcodes opcs;

    for (uint32 i = 1; i < files.size(); ++i)
    {
        if (files[i].size() < 4)
            continue;

        if (files[i].size() < 4 || files[i].substr(files[i].size() - 3, 3) != "pkt")
            continue;

        boost::timer::auto_cpu_timer t;
        printf("%i/%i (%s)\n", i, files.size(), files[i]);

        std::map<uint32, std::forward_list<Packet*>> groups;

        {
            BinaryPacketReader reader(files[i].c_str());

            while (reader.CanRead())
            {
                Packet* packet = reader.Read();
                if (packet != NULL)
                    groups[packet->GetOpcode()].push_front(packet);
            }
        } // it's important that reader is deallocated asap

        for (auto itr = groups.begin(); itr != groups.end(); ++itr)
        {
            std::string file = "split/" + opcs.GetOpcodeName(15595, itr->first) + ".pkt";

            std::ofstream os(file, std::ios::binary | std::ios::app | std::ios::out);

            for (auto itrIn = itr->second.begin(); itrIn != itr->second.end(); ++itrIn)
            {
                uint16 opcode = (*itrIn)->GetOpcode();
                uint32 size = (*itrIn)->Size();
                uint8 direction = (*itrIn)->GetDirection();
                uint64 time = (*itrIn)->GetDateTime();

                os.write((const char*)(&opcode), sizeof(opcode));
                os.write((const char*)(&size), sizeof(size));
                os.write((const char*)(&direction), sizeof(direction));
                os.write((const char*)(&time), sizeof(time));
                os.write((const char*)(*itrIn)->GetBuffer(), (*itrIn)->Size());

                delete (*itrIn);
            }

            os.close();
        }
    }

    system("PAUSE");

    return 0;
}