#include "proposerproxy.h"

ProposerProxyI::ProposerProxyI(ServerMap & acceptors, ServerMap & proposers, ServerId acceptorId, asio::io_context & context)
    : acceptorMap_(acceptors)
    , proposerMap_(proposers)
    , acceptorId_(acceptorId)
    , context_(context)
{
    udp::endpoint endpoint;
    if(!acceptorMap_.getEndpoint(acceptorId_, endpoint))
        return;

    socket_ = std::make_shared<udp_socket>(context_, [this](const udp::endpoint & ep, uint8_t * data, size_t size)
    {
        onRecived(ep, data, size);
    },
    10240,
    1024);
    socket_->bind(endpoint);
    socket_->start();
}

void ProposerProxyI::addAcceptor(ServerId id, AcceptorPtr & acceptor)
{
    acceptors_[id] = acceptor;
}

void ProposerProxyI::delAcceptor(ServerId id)
{
    acceptors_.erase(id);
}

void ProposerProxyI::replyPrepare(const ServerId & proposer, const PrepareReply & reply)
{
    if(!socket_)
        return;

    udp::endpoint endpoint;
    if(!proposerMap_.getEndpoint(proposer, endpoint))
        return;
    std::string data = reply.toString();
    data.insert(data.begin(), MSG_PREPARE_REQUEST);
    socket_->send_to(endpoint, std::move(data));
}

void ProposerProxyI::replyPropose(const ServerId & proposer, const ProposeReply & reply)
{
    if(!socket_)
        return;

    udp::endpoint endpoint;
    if(!proposerMap_.getEndpoint(proposer, endpoint))
        return;
    std::string data = reply.toString();
    data.insert(data.begin(), MSG_PROPOSE_REQUEST);
    socket_->send_to(endpoint, std::move(data));
}

void ProposerProxyI::onRecived(const udp::endpoint & endpoint, uint8_t * data, size_t size)
{
    if(size < 1)
        return;

    ServerId id;
    if(!proposerMap_.getId(endpoint, id))
        return;

    auto iter2 = acceptors_.find(acceptorId_);
    if(iter2 == acceptors_.end())
        return;
    auto & acceptor = iter2->second;

    std::string str((const char *)data+1, size-1);
    switch (data[0])
    {
    case MSG_PREPARE_REQUEST:
    {
        PrepareRequest request;
        if(!request.fromString(str))
            return;
        acceptor->onPrepare(id, request);
    }
        break;
    case MSG_PROPOSE_REQUEST:
    {
        ProposeRequest request;
        if(!request.fromString(str))
            return;
        acceptor->onPropose(id, request);
    }
        break;
    default:
        break;
    }
}
