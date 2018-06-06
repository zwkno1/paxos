#pragma once

#include <optional>
#include <tuple>

#include "commdata.h"
#include "interface.h"

class AcceptorI : public Acceptor, private noncopyable
{
public:
    AcceptorI(DatabasePtr db, ProposerProxyPtr proposerProxy, ServerId & id);

    void onPrepare(const ServerId & proposer, const PrepareRequest & request) override;

    void onPropose(const ServerId & proposer, const ProposeRequest & request) override;

private:
    void load();

    void save();

    // db
    DatabasePtr db_;

    // proposer proxy
    ProposerProxyPtr proposerProxy_;

    // server id
    ServerId id_;

    // version & last accepted proposal
    AcceptorData data_;
};
