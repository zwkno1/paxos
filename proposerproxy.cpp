#include "proposerproxy.h"
#include "logger.h"

ProposerProxyI::ProposerProxyI(ServerMap & acceptors, ServerMap & proposers, asio::io_context & context)
    : acceptorMap_(acceptors)
    , proposerMap_(proposers)
    , context_(context)
{
}

void ProposerProxyI::addAcceptor(AcceptorPtr & acceptor)
{
    ServerId acceptorId = acceptor->id();
    acceptors_[acceptorId] = acceptor;

    udp::endpoint endpoint;
    if(!acceptorMap_.getEndpoint(acceptorId, endpoint))
        return;

    auto sock = std::make_shared<udp_socket>(context_, [this, acceptorId](std::shared_ptr<udp_socket> sock, const udp::endpoint & ep, uint8_t * data, size_t size)
    {
        onRecived(acceptorId, ep, data, size);
    },
    10240,
    1024);
    sock->bind(endpoint);
    sock->start();
    sockets_[acceptor->id()] = sock;
}

void ProposerProxyI::delAcceptor(ServerId id)
{
    acceptors_.erase(id);
    auto iter = sockets_.find(id);
    if(iter != sockets_.end())
    {
        iter->second->stop();
        sockets_.erase(iter);
    }
}

void ProposerProxyI::replyPrepare(const ServerId & from, const ServerId & to, const PrepareReply & reply)
{
    auto iter = sockets_.find(from);
    if(iter == sockets_.end())
    {
        Logger::debug() << "replyPrepare, socket not found, acceptor:" << from;
        return;
    }

    udp::endpoint endpoint;
    if(!proposerMap_.getEndpoint(to, endpoint))
    {
        Logger::debug() << "replyPrepare, endpoint not found, proposer:" << to;
        return;
    }
    std::string data = reply.toString();
    data.insert(data.begin(), MSG_PREPARE_REPLY);
    iter->second->send_to(endpoint, std::move(data));
}

void ProposerProxyI::replyPropose(const ServerId & from, const ServerId & to, const ProposeReply & reply)
{
    auto iter = sockets_.find(from);
    if(iter == sockets_.end())
    {
        Logger::debug() << "replyPropose, socket not found, acceptor:" << from;
        return;
    }

    udp::endpoint endpoint;
    if(!proposerMap_.getEndpoint(to, endpoint))
    {
        Logger::debug() << "replyPropose, endpoint not found, proposer:" << to;
        return;
    }
    std::string data = reply.toString();
    data.insert(data.begin(), MSG_PROPOSE_REPLY);
    iter->second->send_to(endpoint, std::move(data));
}

void ProposerProxyI::onRecived(ServerId acceptorId, const udp::endpoint & endpoint, uint8_t * data, size_t size)
{
    Logger::debug() << "onRecive, proposer: " << acceptorId << ", endpoint :" << endpoint.address().to_string() << ":" << endpoint.port() << ", type: " << (size > 0 ? (int)data[0] : 0) << ", size: " << size;
    if(size < 1)
        return;

    ServerId id;
    if(!proposerMap_.getId(endpoint, id))
    {
        Logger::debug() << "onRecive, endpoint not found";
        return;
    }

    auto iter2 = acceptors_.find(acceptorId);
    if(iter2 == acceptors_.end())
    {
        Logger::debug() << "onRecive, acceptor not found";
        return;
    }
    auto & acceptor = iter2->second;

    std::string str((const char *)data+1, size-1);
    switch (data[0])
    {
    case MSG_PREPARE_REQUEST:
    {
        PrepareRequest request;
        if(!request.fromString(str))
        {
            Logger::debug() << "onRecive, MSG_PREPARE_REQUEST parse error";
            return;
        }
        acceptor->onPrepare(id, request);
    }
        break;
    case MSG_PROPOSE_REQUEST:
    {
        ProposeRequest request;
        if(!request.fromString(str))
        {
            Logger::debug() << "onRecive, MSG_PROPOSE_REQUEST parse error";
            return;
        }
        acceptor->onPropose(id, request);
    }
        break;
    default:
        Logger::debug() << "onRecive, unknown message";
        break;
    }
}
