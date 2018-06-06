#include "proposerproxy.h"

ProposerProxyI::ProposerProxyI(ServerId id, asio::io_context & context, const udp::endpoint & endpoint, const std::map<ServerId, udp::endpoint> & acceptors)
    : id_(id)
    , proposers_(acceptors)
    , context_(context)
{
    for(auto const &  i : acceptors)
    {
        acceptorIds_[i.second] = i.first;
    }

    socket_ = std::make_shared<udp_socket>(context_, [this](const udp::endpoint & ep, uint8_t * data, size_t size)
    {
        onRecived(ep, data, size);
    },
    10240,
    1024);
    socket_->bind(endpoint);
}

void ProposerProxyI::addAcceptor(ServerId id, std::shared_ptr<Proposer> & proposer)
{
    proposers_[id] = proposer;
}

void ProposerProxyI::delAcceptor(ServerId id)
{
    proposers_.erase(id);
}

void ProposerProxyI::replyPrepare(const ServerId & proposer, const PrepareReply & reply)
{
    auto iter = proposers_.find(proposer);
    if(iter == proposers_.end())
        return;
    std::string data = reply.toString();
    data.insert(data.begin(), MSG_PREPARE_REQUEST);
    socket_->send_to(iter->second, std::move(data));
}

void ProposerProxyI::replyPropose(const ServerId & proposer, const ProposeReply & reply)
{
    auto iter = proposers_.find(proposer);
    if(iter == proposers_.end())
        return;
    std::string data = reply.toString();
    data.insert(data.begin(), MSG_PROPOSE_REQUEST);
    socket_->send_to(iter->second, std::move(data));
}

void ProposerProxyI::onRecived(const udp::endpoint & ep, uint8_t * data, size_t size)
{
    if(size < 1)
        return;
    ServerId id;
    auto iter = acceptorIds_.find(ep);
    if(iter == acceptorIds_.end())
        return;
    id = iter->second;

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
