#include "acceptor.h"

AcceptorI::AcceptorI(DatabasePtr db, ProposerProxyPtr proposerProxy, ServerId & id)
    : db_(db)
    , proposerProxy_(proposerProxy)
    , id_(id)
{
}

void AcceptorI::onPrepare(const ServerId & proposer, const PrepareRequest & request)
{
    const Version & version = request.version_;

    bool isAccept = false;
    if(version > data_.version_)
    {
        isAccept = true;
        data_.version_ = version;
        save();
    }

    PrepareReply reply{data_.version_, isAccept, data_.proposal_ };
    proposerProxy_->replyPrepare(proposer, reply);
}

void AcceptorI::onPropose(const ServerId & proposer, const ProposeRequest & request)
{
    const Version & version = request.version_;
    const Value & value = request.value_;

    bool isAccept = false;
    if(version > data_.version_)
    {
        isAccept = true;
        data_.proposal_->version_ = version;
        data_.proposal_->value_ = value;
        data_.version_ = version;
        save();
    }

    ProposeReply reply{ version, isAccept };
    proposerProxy_->replyPropose(proposer, reply);
}

void AcceptorI::load()
{
    db_->load(id_, data_);
}

void AcceptorI::save()
{
    db_->save(id_, data_);
}
