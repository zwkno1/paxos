#pragma once

#include "interface.h"

#include <memory>
#include <leveldb/db.h>

class LevelDB : public Database
{
public:
    LevelDB(const std::string & path);

    //load proposer
    void load(ProposerData & data) override;

    //save proposer
    void save(const ProposerData & data) override;

    //load acceptor
    void load(AcceptorData & data) override;

    //save acceptor
    void save(const AcceptorData & data) override;

private:
    void checkStatus(const leveldb::Status & status);
    void checkData(bool result);

    std::unique_ptr<leveldb::DB> db_;

    leveldb::WriteOptions writeOptions_;

    std::string path_;
};


