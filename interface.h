#pragma once

#include <optional>
#include "commdata.h"

class Acceptor
{
public:
    virtual void onPrepare(const ServerId & proposer, const PrepareRequest & request) = 0;

    virtual void onPropose(const ServerId & proposer, const ProposeRequest & request) = 0;

    virtual ~Acceptor() {}
};

class Proposer
{
public:
    virtual void onPrepareReply(const ServerId & acceptor, const PrepareReply & reply) = 0;

    virtual void onProposeReply(const ServerId & acceptor, const ProposeReply & reply) = 0;

    virtual ~Proposer() {}
};

class AcceptorProxy
{
public:
    virtual void prepare(const ServerId & acceptor, const PrepareRequest & request) = 0;

    virtual void propose(const ServerId & acceptor, const ProposeRequest & request) = 0;

    virtual ~AcceptorProxy() {}
};

class ProposerProxy
{
public:
    virtual void replyPrepare(const ServerId & proposer, const PrepareReply & reply) = 0;

    virtual void replyPropose(const ServerId & proposer, const ProposeReply & reply) = 0;

    virtual ~ProposerProxy() {}
};

class Database
{
public:
    //load proposer
    virtual void load(const ServerId & id, ProposerData & data) = 0;

    //save proposer
    virtual void save(const ServerId & id, const ProposerData & data) = 0;

    //load acceptor
    virtual void load(const ServerId & id, AcceptorData & data) = 0;

    //save acceptor
    virtual void save(const ServerId & id, const AcceptorData & data) = 0;

    virtual ~Database() {}
};

