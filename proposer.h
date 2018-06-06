#pragma once

#include <system_error>
#include <optional>
#include <set>
#include <chrono>

#include "commdata.h"
#include "interface.h"

class ProposerI : public Proposer, private noncopyable
{
public:
    enum ProposerState
    {
        UNACCEPTED = 0,
        PREPARING,
        PROPOSING,
        ACCEPTED,
    };

    ProposerI(DatabasePtr db, AcceptorProxyPtr acceptor, ServerId id, const std::set<ServerId> & acceptorIds);

    void startPrepare();

    void onPrepareReply(const ServerId & acceptor, const PrepareReply & reply) override;

    void onProposeReply(const ServerId & acceptor, const ProposeReply & reply) override;

    void onTick() override;

    ProposerState state() const;

    const Version & version() const;

    const Value & value() const;

private:
    void resetBallot();

    void load();

    void save();

    void changeState(ProposerState state);

    DatabasePtr db_;

    AcceptorProxyPtr acceptor_;

    // this proposer id
    ServerId id_;

    ProposerState state_;

    // all acceptoer id
    std::set<ServerId> acceptorIds_;

    // version & value
    ProposerData data_;

    std::size_t quorum_;

    // ballot info
    std::set<ServerId> votes_;

    std::size_t acceptNum_;

    std::size_t rejectNum_;

    //the max proposal acceptor reply for prepare
    std::optional<Proposal> proposal_;

    // default value, if proposal_ is null propose this value
    Value defaultValue_;

    std::chrono::steady_clock::time_point timestamp_;

    std::chrono::steady_clock::duration prepareTimeout_;

    std::chrono::steady_clock::duration proposeTimeout_;
};
