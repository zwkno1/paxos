#pragma once

#include "asio.h"
#include "commdata.h"

class ServerMap
{
public:
    ServerMap(const std::map<ServerId, udp::endpoint> & servers)
        : endpoints_(servers)
    {
        for(auto const & i : servers)
        {
            ids_[i.second] = i.first;
        }
    }

    inline bool getEndpoint(const ServerId & id, udp::endpoint & ep)
    {
        auto iter = endpoints_.find(id);
        if(iter == endpoints_.end())
            return false;
        ep = iter->second;
        return true;
    }

    inline bool getId(const udp::endpoint & endpint, ServerId & id)
    {
        auto iter = ids_.find(endpint);
        if(iter == ids_.end())
            return false;
        id = iter->second;
        return true;
    }

private:
    std::map<ServerId, udp::endpoint> endpoints_;
    std::map<udp::endpoint, ServerId> ids_;
};
