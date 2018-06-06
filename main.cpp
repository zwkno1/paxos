#include <iostream>

#include "paxos.h"
#include "asio.h"

int main(int argc, char *argv[])
{
    // a/p id db
    asio::io_context context;

    std::map<ServerId, udp::endpoint> acceptorsEndpoints =
    {
        { 1, udp::endpoint{asio::ip::address::from_string("127.0.0.1"), 50000 } },
        { 2, udp::endpoint{asio::ip::address::from_string("127.0.0.1"), 50001 } },
        { 3, udp::endpoint{asio::ip::address::from_string("127.0.0.1"), 50002 } },
    };

    std::map<ServerId, udp::endpoint> proposersEndpoints =
    {
        { 1, udp::endpoint{asio::ip::address::from_string("127.0.0.1"), 60001 } },
        { 2, udp::endpoint{asio::ip::address::from_string("127.0.0.1"), 60002 } },
        { 3, udp::endpoint{asio::ip::address::from_string("127.0.0.1"), 60003 } },
    };

    std::set<ServerId> acceptorIds = {1,2,3};

    ServerMap acceptors(acceptorsEndpoints);
    ServerMap proposers(proposersEndpoints);

    //server type acceptor/proposer
    std::string type = argv[1];
    //server id
    ServerId id = atoi(argv[2]);
    //db path
    std::string dbPath = argv[3];

    DatabasePtr db = std::make_shared<LevelDB>(dbPath);

    try
    {

        if(type == "proposer")
        {
            auto acceptorProxy = std::make_shared<AcceptorProxyI>(acceptors, proposers, id, context);
            ProposerPtr proposer = std::make_shared<ProposerI>(db, acceptorProxy, id, acceptorIds);
            acceptorProxy->addProposer(id, proposer);
            context.run();
        }
        else if(type == "acceptor")
        {
            auto proposerProxy = std::make_shared<ProposerProxyI>(acceptors, proposers, id, context);
            AcceptorPtr acceptor = std::make_shared<AcceptorI>(db, proposerProxy, id);
            proposerProxy->addAcceptor(id, acceptor);
            context.run();
        }
    }
    catch(const char * err)
    {
        std::cout << err << std::endl;
    }

    return 0;
}
