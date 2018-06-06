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
    ProposerProxyI(ServerMap & acceptors, ServerMap & proposers, ServerId acceptorId, asio::io_context & context);

    void addAcceptor(ServerId id, AcceptorPtr & proposer);

    void delAcceptor(ServerId id);

    void replyPrepare(const ServerId & proposer, const PrepareReply & reply) override;

    void replyPropose(const ServerId & proposer, const ProposeReply & reply) override;

private:
    void onRecived(const udp::endpoint & ep, uint8_t * data, size_t size);

    ServerMap & acceptorMap_;

    ServerMap & proposerMap_;

    ServerId acceptorId_;

    std::map<ServerId, AcceptorPtr> acceptors_;

    asio::io_context & context_;

    std::shared_ptr<udp_socket> socket_;

};
