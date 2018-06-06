#pragma once

#include <set>
#include <list>
#include <chrono>
#include <memory>

#include "commdata.h"
#include "interface.h"
#include "udp_socket.h"
#include "servermap.h"

class AcceptorProxyI : public AcceptorProxy, private noncopyable
{
public:
    AcceptorProxyI(ServerMap & acceptors, ServerMap & proposers, ServerId proposerId, asio::io_context & context);

    void addProposer(ServerId id, ProposerPtr & proposer);

    void delProposer(ServerId id);

    void prepare(const ServerId & acceptor, const PrepareRequest & request) override;

    void propose(const ServerId & acceptor, const ProposeRequest & request) override;

private:
    void onRecived(const udp::endpoint & ep, uint8_t * data, size_t size);

    void startTimer();

    void onTimer(const error_code & ec);

    ServerMap & acceptorMap_;

    ServerMap & proposerMap_;

    ServerId id_;

    std::map<ServerId, ProposerPtr> proposers_;

    asio::io_context & context_;

    std::shared_ptr<udp_socket> socket_;

    asio::steady_timer timer_;
};
