#include "acceptorproxy.h"

AcceptorProxyI::AcceptorProxyI(ServerId id, asio::io_context & context, const udp::endpoint & endpoint, const std::map<ServerId, udp::endpoint> & acceptors)
    : id_(id)
    , acceptors_(acceptors)
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

void AcceptorProxyI::addProposer(ServerId id, std::shared_ptr<Proposer> & proposer)
{
    proposers_[id] = proposer;
}

void AcceptorProxyI::delProposer(ServerId id)
{
    proposers_.erase(id);
}

void AcceptorProxyI::prepare(const ServerId & acceptor, const PrepareRequest & request)
{
    auto iter = acceptors_.find(acceptor);
    if(iter == acceptors_.end())
        return;
    std::string data = request.toString();
    data.insert(data.begin(), MSG_PREPARE_REQUEST);
    socket_->send_to(iter->second, std::move(data));
}

void AcceptorProxyI::propose(const ServerId & acceptor, const ProposeRequest & request)
{
    auto iter = acceptors_.find(acceptor);
    if(iter == acceptors_.end())
        return;
    std::string data = request.toString();
    data.insert(data.begin(), MSG_PROPOSE_REQUEST);
    socket_->send_to(iter->second, std::move(data));
}

void AcceptorProxyI::onRecived(const udp::endpoint & ep, uint8_t * data, size_t size)
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
