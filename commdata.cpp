#include "commdata.h"
#include <json/json.h>

namespace
{

std::string json_to_string(const Json::Value & value)
{
    Json::StreamWriterBuilder builder;
    builder["indentation"] = "";
    return Json::writeString(builder, value);
}

Json::Value json_from_string(const std::string & str)
{
    Json::Value value;
    Json::Reader reader;
    if(!reader.parse(str, value))
        return Json::Value::null;
    return value;
}

}
std::string AcceptorData::toString() const
{
    Json::Value root;
    root["version"] = version_;
    Json::Value p;
    if(proposal_)
    {
        p["version"] = proposal_->version_;
        p["value"] = proposal_->value_;
    }
    root["proposal"] = p;
    return json_to_string(root);
}

bool AcceptorData::fromString(const std::string & str)
{
    version_ = 0;
    proposal_.reset();

    Json::Value root = json_from_string(str);
    if(!root.isObject())
        return false;

    if(!root.isMember("version"))
        return false;
    if(!root.isMember("proposal"))
        return false;

    Json::Value v = root.get("version", Json::Value::null);
    if(!v.isUInt64())
        return false;
    version_ = v.asUInt64();

    v = root.get("proposal", Json::Value::null);
    if(v.isObject())
    {
        Proposal p;
        Json::Value v2;
        v2 = v.get("version", Json::Value::null);
        if(!v2.isUInt64())
            return false;
        p.version_ = v2.asUInt64();

        v2 = v.get("Value", Json::Value::null);
        if(!v2.isString())
            return false;
        p.value_ = v2.asString();
        proposal_ = p;
    }
    else if(!v.isNull())
    {
        return false;
    }

    return true;
}

std::string ProposerData::toString() const
{
    Json::Value root;
    root["version"] = version_;
    if(value_)
        root["value"] = *value_;
    else
        root["value"] = Json::Value::null;
    return json_to_string(root);
}

bool ProposerData::fromString(const std::string & str)
{
    version_ = 0;
    value_.reset();

    Json::Value root = json_from_string(str);
    if(!root.isObject())
        return false;

    if(!root.isMember("version"))
        return false;
    if(!root.isMember("value"))
        return false;

    Json::Value v = root.get("version", Json::Value::null);
    if(!v.isUInt64())
        return false;
    version_ = v.asUInt64();

    v = root.get("value", Json::Value::null);
    if(v.isString())
    {
        value_ = v.asString();
    }
    else if(!v.isNull())
    {
        return false;
    }

    return true;
}

std::string PrepareRequest::toString() const
{
    Json::Value root;
    root["version"] = version_;
    return json_to_string(root);
}

bool PrepareRequest::fromString(const std::string & str)
{
    Json::Value root = json_from_string(str);
    if(!root.isObject())
        return false;

    if(!root.isMember("version"))
        return false;

    Json::Value v = root.get("version", Json::Value::null);
    if(!v.isUInt64())
        return false;
    version_ = v.asUInt64();

    return true;
}

std::string ProposeRequest::toString() const
{
    Json::Value root;
    root["version"] = version_;
    root["value"] = value_;
    return json_to_string(root);
}

bool ProposeRequest::fromString(const std::string & str)
{
    version_ = 0;
    value_.clear();

    Json::Value root = json_from_string(str);
    if(!root.isObject())
        return false;

    if(!root.isMember("version"))
        return false;
    if(!root.isMember("value"))
        return false;

    Json::Value v = root.get("version", Json::Value::null);
    if(!v.isUInt64())
        return false;
    version_ = v.asUInt64();

    v = root.get("value", Json::Value::null);
    if(!v.isString())
        return false;
    value_ = v.asString();

    return true;
}

std::string PrepareReply::toString() const
{
    Json::Value root;
    root["version"] = version_;
    root["isAccept"] = isAccept_;

    Json::Value v;
    if(proposal_)
    {
        v["version"] = proposal_->version_;
        v["value"] = proposal_->value_;
    }
    root["proposal"] = v;

    return json_to_string(root);
}

bool PrepareReply::fromString(const std::string & str)
{
    version_ = 0;
    isAccept_ = false;
    proposal_.reset();

    Json::Value root = json_from_string(str);
    if(!root.isObject())
        return false;

    if(!root.isMember("version"))
        return false;
    if(!root.isMember("isAccept"))
        return false;
    if(!root.isMember("proposal"))
        return false;

    Json::Value v = root.get("version", Json::Value::null);
    if(!v.isUInt64())
        return false;
    version_ = v.asUInt64();

    v = root.get("isAccept", Json::Value::null);
    if(!v.isBool())
        return false;
    isAccept_ = v.asBool();

    v = root.get("proposal", Json::Value::null);
    if(v.isObject())
    {
        if(!v.isMember("version"))
            return false;
        if(!v.isMember("value"))
            return false;

        Proposal p;
        Json::Value v2 = v.get("version", Json::Value::null);
        if(!v2.isUInt64())
            return false;
        p.version_ = v2.asUInt64();

        v2 = v.get("value", Json::Value::null);
        if(!v2.isString())
            return false;
        p.value_ = v2.asString();
        proposal_ = p;
    }
    else if(!v.isNull())
    {
        return false;
    }

    return true;
}

std::string ProposeReply::toString() const
{
    Json::Value root;
    root["version"] = version_;
    root["isAccept"] = isAccept_;

    return json_to_string(root);
}

bool ProposeReply::fromString(const std::string & str)
{
    version_ = 0;
    isAccept_ = false;

    Json::Value root = json_from_string(str);
    if(!root.isObject())
        return false;

    if(!root.isMember("version"))
        return false;
    if(!root.isMember("isAccept"))
        return false;

    Json::Value v = root.get("version", Json::Value::null);
    if(!v.isUInt64())
        return false;
    version_ = v.asUInt64();

    v = root.get("isAccept", Json::Value::null);
    if(!v.isBool())
        return false;
    version_ = v.asBool();

    return true;
}








