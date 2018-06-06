#include "acceptorproxy.h"

AcceptorProxyI::AcceptorProxyI(ServerMap & acceptors, ServerMap & proposers, ServerId proposerId, asio::io_context & context)
    : acceptorMap_(acceptors)
    , proposerMap_(proposers)
    , id_(proposerId)
    , context_(context)
    , timer_(context)
{
    udp::endpoint endpoint;
    if(!proposerMap_.getEndpoint(proposerId, endpoint))
        return;

    socket_ = std::make_shared<udp_socket>(context_, [this](const udp::endpoint & ep, uint8_t * data, size_t size)
    {
        onRecived(ep, data, size);
    },
    10240,
    1024);
    socket_->bind(endpoint);
    socket_->start();

    startTimer();
}

void AcceptorProxyI::addProposer(ServerId id, ProposerPtr & proposer)
{
    proposers_[id] = proposer;
}

void AcceptorProxyI::delProposer(ServerId id)
{
    proposers_.erase(id);
}

void AcceptorProxyI::prepare(const ServerId & acceptor, const PrepareRequest & request)
{
    udp::endpoint endpoint;
    if(!acceptorMap_.getEndpoint(acceptor, endpoint))
        return;
    std::string data = request.toString();
    data.insert(data.begin(), MSG_PREPARE_REQUEST);
    socket_->send_to(endpoint, std::move(data));
}

void AcceptorProxyI::propose(const ServerId & acceptor, const ProposeRequest & request)
{
    udp::endpoint endpoint;
    if(!acceptorMap_.getEndpoint(acceptor, endpoint))
        return;
    std::string data = request.toString();
    data.insert(data.begin(), MSG_PROPOSE_REQUEST);
    socket_->send_to(endpoint, std::move(data));
}

void AcceptorProxyI::onRecived(const udp::endpoint & endpoint, uint8_t * data, size_t size)
{
    if(size < 1)
        return;

    ServerId id;
    if(!acceptorMap_.getId(endpoint, id))
        return;

    auto iter2 = proposers_.find(id_);
    if(iter2 == proposers_.end())
        return;
    auto & proposer = iter2->second;

    std::string str((const char *)data+1, size-1);
    switch (data[0])
    {
    case MSG_PREPARE_REPLY:
    {
        PrepareReply reply;
        if(!reply.fromString(str))
            return;
        proposer->onPrepareReply(id, reply);
    }
        break;
    case MSG_PROPOSE_REPLY:
    {
        ProposeReply reply;
        if(!reply.fromString(str))
            return;
        proposer->onProposeReply(id, reply);
    }
        break;
    default:
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
