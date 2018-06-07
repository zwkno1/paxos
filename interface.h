#pragma once

#include <optional>
#include <memory>

#include "commdata.h"

class Role
{
public:
    virtual const ServerId & id() const = 0;

    virtual ~Role() {}
};

class Acceptor : public Role
{
public:
    virtual void onPrepare(const ServerId & proposer, const PrepareRequest & request) = 0;

    virtual void onPropose(const ServerId & proposer, const ProposeRequest & request) = 0;

    // current version
    virtual const Version & version() const = 0;

    // last accepted proposal
    virtual const std::optional<Proposal> & proposal() const = 0;
};

typedef std::shared_ptr<Acceptor> AcceptorPtr;

class Proposer : public Role
{
public:
    virtual void onPrepareReply(const ServerId & acceptor, const PrepareReply & reply) = 0;

    virtual void onProposeReply(const ServerId & acceptor, const ProposeReply & reply) = 0;

    virtual void onTick() = 0;

    // current version
    virtual const Version & version() const = 0;

    // accepted value
    virtual const std::optional<Value> & value() const = 0;
};

typedef std::shared_ptr<Proposer> ProposerPtr;


class AcceptorProxy
{
public:
    virtual void prepare(const ServerId & from, const ServerId & to, const PrepareRequest & request) = 0;

    virtual void propose(const ServerId & from, const ServerId & to, const ProposeRequest & request) = 0;

    virtual ~AcceptorProxy() {}
};

typedef std::shared_ptr<AcceptorProxy> AcceptorProxyPtr;


class ProposerProxy
{
public:
    virtual void replyPrepare(const ServerId & from, const ServerId & to, const PrepareReply & reply) = 0;

    virtual void replyPropose(const ServerId & from, const ServerId & to, const ProposeReply & reply) = 0;

    virtual ~ProposerProxy() {}
};

typedef std::shared_ptr<ProposerProxy> ProposerProxyPtr;


class Database
{
public:
    //load proposer
    virtual void load(ProposerData & data) = 0;

    //save proposer
    virtual void save(const ProposerData & data) = 0;

    //load acceptor
    virtual void load(AcceptorData & data) = 0;

    //save acceptor
    virtual void save(const AcceptorData & data) = 0;

    virtual ~Database() {}
};

typedef std::shared_ptr<Database> DatabasePtr;
