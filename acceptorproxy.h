#pragma once

#include <set>
#include <list>
#include <chrono>
#include <memory>

#include "commdata.h"
#include "interface.h"
#include "udp_socket.h"
#include "servermap.h"

class AcceptorProxyI : public AcceptorProxy
{
public:
    AcceptorProxyI(ServerId id, asio::io_context & context, const udp::endpoint & endpoint, const std::map<ServerId, udp::endpoint> & acceptors);

    void addProposer(ServerId id, std::shared_ptr<Proposer> & proposer);

    void delProposer(ServerId id);

    void prepare(const ServerId & acceptor, const PrepareRequest & request) override;

    void propose(const ServerId & acceptor, const ProposeRequest & request) override;

private:
    void onRecived(const udp::endpoint & ep, uint8_t * data, size_t size);

    ServerId id_;

    std::map<ServerId, std::shared_ptr<Proposer> > proposers_;

    asio::io_context & context_;

    std::shared_ptr<udp_socket> socket_;

    ServerMap & acceptors_;
};
