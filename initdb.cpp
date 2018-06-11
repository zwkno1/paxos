#include <iostream>

#include "paxos.h"

int main(int argc, char *argv[])
{
    if(argc < 4)
    {
        std::cout << argv[0] << " dbPath acceptor/proposer serverId sequence" << std::endl;
        return -1;
    }

    std::string dbPath = argv[1];
    std::string role = argv[2];
    ServerId serverId = atoi(argv[3]);
    uint64_t sequence = 0;
    if(argc >= 4)
        sequence = atoi(argv[4]);
    try
    {
        DatabasePtr db = makeDatabase(dbPath);
        if(role == "acceptor")
        {
            AcceptorData data;
            data.id_ = serverId;
            data.version_ = Version(serverId, sequence);
            std::cout << role << " data: " << data.toString() << std::endl;
            db->save(data);
        }
        else if(role == "proposer")
        {
            ProposerData data;
            data.id_ = serverId;
            data.version_ = Version(serverId, sequence);
            std::cout << role << " data: " << data.toString() << std::endl;
            db->save(data);
        }
    }
    catch(const Exception & e)
    {
        std::cout << e.where() << ":" << e.what() << std::endl;
    }
    return 0;
}
