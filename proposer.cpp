#include "proposer.h"

#include <random>

ProposerI::ProposerI(DatabasePtr db, AcceptorProxyPtr acceptor, ServerId id, const std::set<ServerId> & acceptorIds)
    : db_(db)
    , acceptor_(acceptor)
    , id_(id)
    , acceptorIds_(acceptorIds)
    , quorum_(acceptorIds.size()/2+1)
{
    load();
    ProposerState initState = data_.value_ ? ACCEPTED : UNACCEPTED;
    changeState(initState);

    prepareTimeout_ = std::chrono::microseconds(100);
    proposeTimeout_ = std::chrono::microseconds(100);
    defaultValue_ = "";
}

void ProposerI::onPrepareReply(const ServerId & acceptor, const PrepareReply & reply)
{
    const Version & version = reply.version_;
    const bool & isAccept = reply.isAccept_;
    const std::optional<Proposal> & proposal = reply.proposal_;

    // version mismatch
    if(version != data_.version_)
        return;

    // state mismatch
    if(state_ != PREPARING)
        return;

    // illega acceptor id or duplicate ballot
    if(votes_.erase(acceptor) != 1)
        return;

    // a acceptor accept this prepare
    if(isAccept)
    {
        ++acceptNum_;

        if(proposal && (proposal->version_ < data_.version_))
        {
            if((!proposal_) || (proposal->version_ > proposal_->version_))
            {
                proposal_ = proposal;
            }
        }

        // quorum accept this prepare, now start propose
        if(acceptNum_ >= quorum_)
        {
            // start propose
            ProposeRequest request{ data_.version_, (proposal_ ? proposal_->value_ : defaultValue_) };
            for(auto const & id : acceptorIds_)
            {
                acceptor_->propose(id, request);
            }

            changeState(PROPOSING);
        }
    }
    // a acceptor reject this prepare
    else
    {
        ++ rejectNum_;
        // quorum reject this prepare, prepare again
        if(rejectNum_ >= quorum_)
        {
            changeState(UNACCEPTED);
        }
    }
}

void ProposerI::onProposeReply(const ServerId & acceptor, const ProposeReply & reply)
{
    const Version & version = reply.version_;
    const bool & isAccept = reply.isAccept_;

    // version mismatch
    if(version != data_.version_)
        return;

    // state mismatch
    if(state_ != PREPARING)
        return;

    // illega acceptor id or duplicate ballot
    if(votes_.erase(acceptor) != 1)
        return;

    // a acceptor accept this propose
    if(isAccept)
    {
        ++ acceptNum_;
        // quorum accept this propose, the value was approved by quorum
        if(acceptNum_ >= quorum_)
        {
            changeState(ACCEPTED);
            data_.value_ = proposal_ ? proposal_->value_ : defaultValue_;
            save();
        }
    }
    // a acceptor accept reject this propose
    else
    {
        ++ rejectNum_;
        // quorum reject this propose
        if(rejectNum_ >= quorum_)
        {
            changeState(UNACCEPTED);
        }
    }
}

void ProposerI::resetBallot()
{
    votes_ = acceptorIds_;
    acceptNum_ = 0;
    rejectNum_ = 0;
    proposal_.reset();
}

void ProposerI::startPrepare()
{
    if(state_ != UNACCEPTED)
        return;

    ++data_.version_;
    save();

    PrepareRequest request{ data_.version_ };
    for(auto id : acceptorIds_)
    {
        acceptor_->prepare(id, request);
    }

    changeState(PREPARING);
}

void ProposerI::onTick()
{
    std::chrono::steady_clock::duration duration = std::chrono::steady_clock::now() - timestamp_;

    switch (state_)
    {
    case UNACCEPTED:
    {
        std::chrono::milliseconds timeout(std::rand()%100+100);
        if(duration > timeout)
        {
            startPrepare();
        }
    }
        break;
    case PREPARING:
    {
        if(duration > prepareTimeout_)
        {
            changeState(UNACCEPTED);
        }
    }
        break;
    case PROPOSING:
    {
        if(duration > proposeTimeout_)
        {
            changeState(UNACCEPTED);
        }
    }
    default:
        break;
    }
}

ProposerI::ProposerState ProposerI::state() const
{
    return state_;
}

const Version & ProposerI::version() const
{
    return data_.version_;
}

void ProposerI::load()
{
    db_->load(id_, data_);
}

void ProposerI::save()
{
    db_->save(id_, data_);
}

void ProposerI::changeState(ProposerState state)
{
    state_ = state;
    timestamp_ = std::chrono::steady_clock::now();
    if(state_ = UNACCEPTED)
    {
        resetBallot();
    }
}
