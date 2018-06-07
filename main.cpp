#include <iostream>

#include "paxos.h"
#include "asio.h"

int main(int argc, char *argv[])
{
    if(argc < 3)
    {
        std::cout << argv[0] << " acceptor/proposer dbPath (proposalValue)" << std::endl;
        return -1;
    }

    asio::io_context context;

    std::map<ServerId, udp::endpoint> acceptorsEndpoints =
    {
        { 1, udp::endpoint{asio::ip::address::from_string("127.0.0.1"), 50000 } },
        { 2, udp::endpoint{asio::ip::address::from_string("127.0.0.1"), 50001 } },
        { 3, udp::endpoint{asio::ip::address::from_string("127.0.0.1"), 50002 } },
    };

    std::map<ServerId, udp::endpoint> proposersEndpoints =
    {
        { 10001, udp::endpoint{asio::ip::address::from_string("127.0.0.1"), 60001 } },
        { 10002, udp::endpoint{asio::ip::address::from_string("127.0.0.1"), 60002 } },
        { 10003, udp::endpoint{asio::ip::address::from_string("127.0.0.1"), 60003 } },
    };

    std::set<ServerId> acceptorIds = {1,2,3};

    ServerMap acceptors(acceptorsEndpoints);
    ServerMap proposers(proposersEndpoints);

    //server type acceptor/proposer
    std::string type = argv[1];
    //db path
    std::string dbPath = argv[2];

    DatabasePtr db = std::make_shared<LevelDB>(dbPath);

    try
    {
        if(type == "proposer")
        {
            auto acceptorProxy = std::make_shared<AcceptorProxyI>(acceptors, proposers, context);
            auto proposer = std::make_shared<ProposerI>(db, acceptorProxy, acceptorIds);
            if(argc > 3)
                proposer->setProposalValue(argv[3]);

            acceptorProxy->addProposer(proposer);
            context.run();
        }
        else if(type == "acceptor")
        {
            auto proposerProxy = std::make_shared<ProposerProxyI>(acceptors, proposers, context);
            AcceptorPtr acceptor = std::make_shared<AcceptorI>(db, proposerProxy);
            proposerProxy->addAcceptor(acceptor);
            context.run();
        }
    }
    catch(const char * err)
    {
        std::cout << err << std::endl;
    }

    return 0;
}
