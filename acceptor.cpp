#include "acceptor.h"

AcceptorI::AcceptorI(Database & db, ProposerProxy & proposer, ServerId & id)
    : db_(db)
    , proposer_(proposer)
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
    proposer_.replyPrepare(proposer, reply);
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
    proposer_.replyPropose(proposer, reply);
}

void AcceptorI::load()
{
    db_.load(id_, data_);
}

void AcceptorI::save()
{
    db_.save(id_, data_);
}
