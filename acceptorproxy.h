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
    AcceptorProxyI(ServerMap & acceptors, ServerMap & proposers, asio::io_context & context);

    void addProposer(ProposerPtr proposer);

    void delProposer(ServerId id);

    void prepare(const ServerId & from, const ServerId & to, const PrepareRequest & request) override;

    void propose(const ServerId & from, const ServerId & to, const ProposeRequest & request) override;

private:
    void onRecived(const ServerId & proposerId, const udp::endpoint & endpoint, uint8_t * data, size_t size);

    void startTimer();

    void onTimer(const error_code & ec);

    ServerMap & acceptorMap_;

    ServerMap & proposerMap_;

    std::map<ServerId, ProposerPtr> proposers_;

    asio::io_context & context_;

    std::map<ServerId, std::shared_ptr<udp_socket> > sockets_;

    asio::steady_timer timer_;
};
