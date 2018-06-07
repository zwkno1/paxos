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

    ProposerI(DatabasePtr db, AcceptorProxyPtr acceptor, const std::set<ServerId> & acceptorIds);

    void setProposalValue(const Value & value);

    void startPrepare();

    void onPrepareReply(const ServerId & acceptor, const PrepareReply & reply) override;

    void onProposeReply(const ServerId & acceptor, const ProposeReply & reply) override;

    void onTick() override;

    const ServerId & id() const override;

    const Version & version() const override;

    const std::optional<Value> & value() const override;

private:
    const std::string & getStateString(ProposerState state);

    void resetBallot();

    void load();

    void save();

    void changeState(ProposerState state);

    DatabasePtr db_;

    AcceptorProxyPtr acceptor_;

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

    // propose this value if all acceptor return null proposal
    Value proposalValue_;

    std::chrono::steady_clock::time_point timestamp_;

    std::chrono::steady_clock::duration prepareTimeout_;

    std::chrono::steady_clock::duration proposeTimeout_;
};
