#pragma once

#include <set>
#include <list>
#include <chrono>
#include <memory>

#include "commdata.h"
#include "interface.h"
#include "udp_socket.h"
#include "servermap.h"

class ProposerProxyI : public ProposerProxy, private noncopyable
{
public:
    ProposerProxyI(ServerMap & acceptors, ServerMap & proposers, asio::io_context & context);

    void addAcceptor(AcceptorPtr & proposer);

    void delAcceptor(ServerId id);

    void replyPrepare(const ServerId & from, const ServerId & to, const PrepareReply & reply) override;

    void replyPropose(const ServerId & from, const ServerId & to, const ProposeReply & reply) override;

private:
    void onRecived(ServerId acceptorId, const udp::endpoint & ep, uint8_t * data, size_t size);

    ServerMap & acceptorMap_;

    ServerMap & proposerMap_;

    std::map<ServerId, AcceptorPtr> acceptors_;

    asio::io_context & context_;

    std::map<ServerId, std::shared_ptr<udp_socket> > sockets_;

};
