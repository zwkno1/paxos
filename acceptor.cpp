#include "acceptor.h"
#include "logger.h"

AcceptorI::AcceptorI(DatabasePtr db, ProposerProxyPtr proposerProxy)
    : db_(db)
    , proposerProxy_(proposerProxy)
{
    load();
}

void AcceptorI::onPrepare(const ServerId & proposer, const PrepareRequest & request)
{
    Logger::debug() << "onPrepare, from proposer : " << proposer  << ", request: " << request.toString();

    const Version & version = request.version_;

    bool isAccept = false;
    if(version > data_.version_)
    {
        Logger::debug() << "onPrepare, accept";
        isAccept = true;
        data_.version_ = version;
        save();
    }

    PrepareReply reply{data_.version_, isAccept, data_.proposal_ };
    Logger::debug() << "onPrepare, reply: " << reply.toString();
    proposerProxy_->replyPrepare(data_.id_ ,proposer, reply);
}

void AcceptorI::onPropose(const ServerId & proposer, const ProposeRequest & request)
{
    Logger::debug() << "onPropose, from proposer : " << proposer  << ", request: " << request.toString();
    const Version & version = request.version_;
    const Value & value = request.value_;

    bool isAccept = false;
    if(version > data_.version_)
    {
        Logger::debug() << "onPropose, accept ";
        isAccept = true;
        data_.proposal_->version_ = version;
        data_.proposal_->value_ = value;
        data_.version_ = version;
        save();
    }

    ProposeReply reply{ version, isAccept };
    Logger::debug() << "onPropose, reply: " << reply.toString();
    proposerProxy_->replyPropose(data_.id_, proposer, reply);
}

const ServerId & AcceptorI::id() const
{
    return data_.id_;
}

const Version & AcceptorI::version() const
{
    return data_.version_;
}

const std::optional<Proposal> & AcceptorI::proposal() const
{
    return data_.proposal_;
}

void AcceptorI::load()
{
    db_->load(data_);
}

void AcceptorI::save()
{
    db_->save(data_);
}
