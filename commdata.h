#pragma once

#include <cstdint>
#include <string>
#include <optional>

#include "noncopyable.h"

//typedef std::uint64_t Version;
typedef std::string Value;
typedef std::uint32_t ServerId;

class Version
{
public:
    Version(uint16_t server, uint64_t sequence)
    {
        value_ = server;
        value_ |= (sequence << 16);
    }

    Version(uint64_t v = 0)
        : value_(v)
    {
    }

    Version(const Version & other)
        : value_(other.value_)
    {
    }

    const Version & operator=(const Version & other)
    {
        value_ = other.value_;
        return *this;
    }

    operator uint64_t() const
    {
        return value_;
    }

    Version & operator++()
    {
        value_ += 1 << 16;
        return *this;
    }

    uint64_t value() const
    {
        return value_;
    }

    uint64_t sequence() const
    {
        return value_ >> 16;
    }

    uint16_t server()
    {
        return value_ & 0xffff;
    }

private:
    uint64_t value_;
};


enum MessageType : uint8_t
{
    MSG_INVALID = 0,
    MSG_PREPARE_REQUEST,
    MSG_PROPOSE_REQUEST,
    MSG_PREPARE_REPLY,
    MSG_PROPOSE_REPLY,
};

struct Proposal
{
    Version version_;
    Value value_;
};

struct AcceptorData
{
    ServerId id_;

    Version version_;

    std::optional<Proposal> proposal_;

    std::string toString() const;

    bool fromString(const std::string & str);
};

struct ProposerData
{
    ServerId id_;

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
