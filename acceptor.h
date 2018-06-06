#pragma once

#include <optional>
#include <tuple>

#include "noncopyable.h"
#include "commdata.h"
#include "interface.h"

class AcceptorI : public Acceptor, private noncopyable
{
public:
    AcceptorI(Database & db, ProposerProxy & proposer, ServerId & id);

    void onPrepare(const ServerId & proposer, const PrepareRequest & request) override;

    void onPropose(const ServerId & proposer, const ProposeRequest & request) override;

private:
    void load();

    void save();

    // db
    Database & db_;

    // proposer proxy
    ProposerProxy & proposer_;

    // server id
    ServerId id_;

    // version & last accepted proposal
    AcceptorData data_;
};
