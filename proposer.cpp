#include "proposer.h"

#include <random>

#include "logger.h"

ProposerI::ProposerI(DatabasePtr db, AcceptorProxyPtr acceptor, const std::set<ServerId> & acceptorIds)
    : db_(db)
    , acceptor_(acceptor)
    , acceptorIds_(acceptorIds)
    , quorum_(acceptorIds.size()/2+1)
{
    load();
    ProposerState initState = data_.value_ ? ACCEPTED : UNACCEPTED;
    changeState(initState);

    prepareTimeout_ = std::chrono::microseconds(100);
    proposeTimeout_ = std::chrono::microseconds(100);
    proposalValue_ = "";
}

void ProposerI::onPrepareReply(const ServerId & acceptor, const PrepareReply & reply)
{
    Logger::debug() << "onPrepareReply, from acceptor: " << acceptor << reply.toString();

    const Version & version = reply.version_;
    const bool & isAccept = reply.isAccept_;
    const std::optional<Proposal> & proposal = reply.proposal_;

    // state mismatch
    if(state_ != PREPARING)
    {
        Logger::debug() << "onPrepareReply, state: "<< getStateString(state_);
        return;
    }

    // version mismatch
    if(version != data_.version_)
    {
        Logger::debug() << "onPrepareReply, proposer version: "<< data_.version_ << ", reply version : " << version;
        return;
    }

    // illega acceptor id or duplicate ballot
    if(votes_.erase(acceptor) != 1)
    {
        Logger::debug() << "onPrepareReply, acceptor id invalid: " << acceptor;
        return;
    }

    Logger::debug() << "onPrepareReply, before, accept num: " << acceptNum_ << ", reject num: " << rejectNum_;

    // a acceptor accept this prepare
    if(isAccept)
    {
        ++acceptNum_;

        if(proposal && (proposal->version_ < data_.version_))
        {
            if((!proposal_) || (proposal->version_ > proposal_->version_))
            {
                proposal_ = proposal;
                Logger::debug() << "onPrepareReply, update proposal, version:" << proposal_->version_ << ", value: " << proposal_->value_;
            }
        }

        // quorum accept this prepare, now start propose
        if(acceptNum_ >= quorum_)
        {
            // start propose
            ProposeRequest request{ data_.version_, (proposal_ ? proposal_->value_ : proposalValue_) };
            Logger::debug() << "onPrepareReply, quorum accept, start propose:" << request.toString();
            for(auto const & id : acceptorIds_)
            {
                acceptor_->propose(data_.id_, id, request);
            }
            changeState(PROPOSING);
            return;
        }
    }
    // a acceptor reject this prepare
    else
    {
        ++ rejectNum_;
        // quorum reject this prepare, prepare again
        if(rejectNum_ >= quorum_)
        {
            Logger::debug() << "onPrepareReply, quorum reject";
            changeState(UNACCEPTED);
            return;
        }
    }

    Logger::debug() << "onPrepareReply, after, accept num: " << acceptNum_ << ", reject num: " << rejectNum_;
}

void ProposerI::onProposeReply(const ServerId & acceptor, const ProposeReply & reply)
{
    const Version & version = reply.version_;
    const bool & isAccept = reply.isAccept_;

    // state mismatch
    if(state_ != PROPOSING)
    {
        Logger::debug() << "onProposeReply, state: "<< getStateString(state_);
        return;
    }

    // version mismatch
    if(version != data_.version_)
    {
        Logger::debug() << "onProposeReply, proposer version: "<< data_.version_ << ", reply version : " << version;
        return;
    }

    // illega acceptor id or duplicate ballot
    if(votes_.erase(acceptor) != 1)
    {
        Logger::debug() << "onProposeReply, acceptor id invalid: " << acceptor;
        return;
    }

    Logger::debug() << "onProposeReply, before, accept num: " << acceptNum_ << ", reject num: " << rejectNum_;
    // a acceptor accept this propose
    if(isAccept)
    {
        ++ acceptNum_;
        // quorum accept this propose, the value was approved by quorum
        if(acceptNum_ >= quorum_)
        {
            changeState(ACCEPTED);
            data_.value_ = (proposal_ ? proposal_->value_ : proposalValue_);
            Logger::debug() << "onProposeReply, quorum accept proposal: " << data_.toString();
            save();
            return;
        }
    }
    // a acceptor accept reject this propose
    else
    {
        ++ rejectNum_;
        // quorum reject this propose
        if(rejectNum_ >= quorum_)
        {
            Logger::debug() << "onProposeReply, quorum reject";
            changeState(UNACCEPTED);
            return;
        }
    }
    Logger::debug() << "onProposeReply, after, accept num: " << acceptNum_ << ", reject num: " << rejectNum_;
}

const std::string & ProposerI::getStateString(ProposerState state)
{
    static std::string stateStrings[] =
    {
        "UNACCEPTED",
        "PREPARING",
        "PROPOSING",
        "ACCEPTED",
    };

    return stateStrings[state];
}

void ProposerI::resetBallot()
{
    votes_ = acceptorIds_;
    acceptNum_ = 0;
    rejectNum_ = 0;
}

void ProposerI::setProposalValue(const Value & value)
{
    proposalValue_ = value;
}

void ProposerI::startPrepare()
{
    if(state_ != UNACCEPTED)
        return;

    ++data_.version_;
    save();

    PrepareRequest request{ data_.version_ };
    Logger::debug() << "startPrepare, request: " << request.toString();
    for(auto id : acceptorIds_)
    {
        acceptor_->prepare(data_.id_, id, request);
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
            Logger::debug() << "onTick, wait timeout, start prepare";
            startPrepare();
        }
    }
        break;
    case PREPARING:
    {
        if(duration > prepareTimeout_)
        {
            Logger::debug() << "onTick, prepare timeout, wait next turn";
            changeState(UNACCEPTED);
        }
    }
        break;
    case PROPOSING:
    {
        if(duration > proposeTimeout_)
        {
            Logger::debug() << "onTick, prepare timeout, wait next turn";
            changeState(UNACCEPTED);
        }
    }
    default:
        break;
    }
}

const ServerId & ProposerI::id() const
{
    return data_.id_;
}

const Version & ProposerI::version() const
{
    return data_.version_;
}

const std::optional<Value> & ProposerI::value() const
{
    return data_.value_;
}

void ProposerI::load()
{
    db_->load(data_);
}

void ProposerI::save()
{
    db_->save(data_);
}

void ProposerI::changeState(ProposerState state)
{
    state_ = state;
    timestamp_ = std::chrono::steady_clock::now();
    if(state_ == PREPARING)
    {
        proposal_.reset();
        resetBallot();
    }
    else if(state_ == PROPOSING)
    {
        resetBallot();
    }
}
