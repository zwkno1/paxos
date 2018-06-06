#pragma once

#include <set>
#include <list>
#include <chrono>
#include <memory>

#include "commdata.h"
#include "interface.h"
#include "udp_socket.h"

class ProposerProxyI : public ProposerProxy
{
public:
    ProposerProxyI(ServerId id, asio::io_context & context, const udp::endpoint & endpoint, const std::map<ServerId, udp::endpoint> & acceptors);

    void addAcceptor(ServerId id, std::shared_ptr<Proposer> & proposer);

    void delAcceptor(ServerId id);

    void replyPrepare(const ServerId & proposer, const PrepareReply & reply) override;

    void replyPropose(const ServerId & proposer, const ProposeReply & reply) override;

private:
    void onRecived(const udp::endpoint & ep, uint8_t * data, size_t size);

    ServerId id_;

    std::map<ServerId, std::shared_ptr<Acceptor> > proposers_;

    asio::io_context & context_;

    std::shared_ptr<udp_socket> socket_;

    std::map<ServerId, udp::endpoint> proposers_;

    std::map<udp::endpoint, ServerId> acceptorIds_;
};
