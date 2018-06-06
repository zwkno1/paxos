#pragma once

#include <cstdint>
#include <string>
#include <optional>

typedef std::uint64_t Version;
typedef std::string Value;
typedef std::uint64_t ServerId;

enum MessageType : uint8_t
{
    MSG_INVALID = 0,
    MSG_PREPARE_REQUEST,
    MSG_PREPARE_REPLY,
    MSG_PROPOSE_REQUEST,
    MSG_PROPOSE_REPLY,
};

struct Proposal
{
    Version version_;
    Value value_;
};

struct AcceptorData
{
    Version version_;

    std::optional<Proposal> proposal_;

    std::string toString() const;

    bool fromString(const std::string & str);
};

struct ProposerData
{
    Version version_;

    std::optional<Value> value_;

    std::string toString() const;

    bool fromString(const std::string & str);
};

struct PrepareRequest
{
    Version version_;

    std::string toString() const;

    bool fromString(const std::string & str);
};

struct ProposeRequest
{
    Version version_;

    Value value_;

    std::string toString() const;

    bool fromString(const std::string & str);
};

struct PrepareReply
{
    Version version_;

    bool isAccept_;

    std::optional<Proposal> proposal_;

    std::string toString() const;

    bool fromString(const std::string & str);
};

struct ProposeReply
{
    Version version_;

    bool isAccept_;

    std::string toString() const;

    bool fromString(const std::string & str);
};
