#include "acceptorproxy.h"
#include "logger.h"

AcceptorProxyI::AcceptorProxyI(ServerMap & acceptors, ServerMap & proposers, asio::io_context & context)
    : acceptorMap_(acceptors)
    , proposerMap_(proposers)
    , context_(context)
    , timer_(context)
{
    startTimer();
}

void AcceptorProxyI::addProposer(ProposerPtr & proposer)
{
    ServerId proposerId = proposer->id();
    proposers_[proposerId] = proposer;

    udp::endpoint endpoint;
    if(!proposerMap_.getEndpoint(proposerId, endpoint))
        return;
    auto sock = std::make_shared<udp_socket>(context_, [this, proposerId](std::shared_ptr<udp_socket> sock, const udp::endpoint & endpoint, uint8_t * data, size_t size)
    {
        onRecived(proposerId, endpoint, data, size);
    },
    10240,
    1024);
    sock->bind(endpoint);
    sock->start();
    sockets_[proposerId] = sock;
}

void AcceptorProxyI::delProposer(ServerId id)
{
    proposers_.erase(id);
}

void AcceptorProxyI::prepare(const ServerId & from, const ServerId & to, const PrepareRequest & request)
{
    auto iter = sockets_.find(from);
    if(iter == sockets_.end())
        return;

    udp::endpoint endpoint;
    if(!acceptorMap_.getEndpoint(to, endpoint))
        return;
    std::string data = request.toString();
    data.insert(data.begin(), MSG_PREPARE_REQUEST);
    iter->second->send_to(endpoint, std::move(data));
}

void AcceptorProxyI::propose(const ServerId & from, const ServerId & to, const ProposeRequest & request)
{
    auto iter = sockets_.find(from);
    if(iter == sockets_.end())
        return;

    udp::endpoint endpoint;
    if(!acceptorMap_.getEndpoint(to, endpoint))
        return;
    std::string data = request.toString();
    data.insert(data.begin(), MSG_PROPOSE_REQUEST);
    iter->second->send_to(endpoint, std::move(data));
}

void AcceptorProxyI::onRecived(const ServerId & proposerId, const udp::endpoint & endpoint, uint8_t * data, size_t size)
{
    Logger::debug() << "onRecive, proposer: " << proposerId << ", endpoint :" << endpoint.address().to_string() << ":" << endpoint.port() << ", type: " << (size > 0 ? (int)data[0] : 0) << ", size: " << size;
    if(size < 1)
        return;

    ServerId acceptorId;
    if(!acceptorMap_.getId(endpoint, acceptorId))
    {
        Logger::debug() << "onRecive, endpoint not found";
        return;
    }

    auto iter = proposers_.find(proposerId);
    if(iter == proposers_.end())
    {
        Logger::debug() << "onRecive, proposer not found";
        return;
    }
    auto & proposer = iter->second;

    std::string str((const char *)data+1, size-1);
    switch (data[0])
    {
    case MSG_PREPARE_REPLY:
    {
        PrepareReply reply;
        if(!reply.fromString(str))
        {
            Logger::debug() << "onRecive, MSG_PREPARE_REPLY parse error";
            return;
        }
        proposer->onPrepareReply(acceptorId, reply);
    }
        break;
    case MSG_PROPOSE_REPLY:
    {
        ProposeReply reply;
        if(!reply.fromString(str))
        {
            Logger::debug() << "onRecive, MSG_PROPOSE_REPLY parse error";
            return;
        }
        proposer->onProposeReply(acceptorId, reply);
    }
        break;
    default:
        Logger::debug() << "onRecive, unknown message";
        break;
    }
}

void AcceptorProxyI::startTimer()
{
    //10ms
    timer_.expires_from_now(std::chrono::milliseconds(10));
    timer_.async_wait([this](const error_code & ec)
    {
        onTimer(ec);
    });
}

void AcceptorProxyI::onTimer(const error_code & ec)
{
    if(!ec)
    {
        for(auto & i : proposers_)
        {
            i.second->onTick();
        }

        startTimer();
    }
}
