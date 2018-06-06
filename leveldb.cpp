#include "leveldb.h"

LevelDB::LevelDB(const std::string & path)
{
    leveldb::Options options;
    options.create_if_missing = true;
    options.max_open_files = 256;

    leveldb::DB * db = nullptr;
    auto const status = leveldb::DB::Open(options, path, &db);
    checkStatus(status);
    db_.reset(db);
}

void LevelDB::load(const ServerId & id, ProposerData & data)
{
    // load proposor data
    std::string key = std::string("P_") + std::to_string(id);
    std::string val;
    auto const status = db_->Get(leveldb::ReadOptions(), key, &val);
    checkStatus(status);
    checkData(data.fromString(val));
}

void LevelDB::save(const ServerId & id, const ProposerData & data)
{
    // save proposor data
    std::string key = std::string("P_") + std::to_string(id);
    std::string value = data.toString();
    leveldb::Slice const keySlice(key.data(), key.size());
    leveldb::Slice const valueSlice(value.data(), value.size());
    auto const status = db_->Put(leveldb::WriteOptions(), keySlice, valueSlice);
    checkStatus(status);
}

void LevelDB::load(const ServerId & id, AcceptorData & data)
{
    // load acceptor data
    std::string key = std::string("A_") + std::to_string(id);
    std::string val;
    auto const status = db_->Get(leveldb::ReadOptions(), key, &val);
    checkStatus(status);
    checkData(data.fromString(val));
}

void LevelDB::save(const ServerId & id, const AcceptorData & data)
{
    // save acceptor data
    std::string key = std::string("A_") + std::to_string(id);
    std::string value = data.toString();
    leveldb::Slice const keySlice(key.data(), key.size());
    leveldb::Slice const valueSlice(value.data(), value.size());
    auto const status = db_->Put(leveldb::WriteOptions(), keySlice, valueSlice);
    checkStatus(status);
}

void LevelDB::checkStatus(const leveldb::Status & status)
{
   if (status.ok())
       return;

   const char * statusStr;
   if (status.ok())
       statusStr = "OK";
   else if (status.IsIOError())
       statusStr = "IOError";
   else if (status.IsCorruption())
       statusStr = "Corruption";
   else if (status.IsNotFound())
       statusStr = "NotFound";
   else
       statusStr = "Unknown";

   throw statusStr;
}

void LevelDB::checkData(bool result)
{
    if(!result)
        throw "parse eroor";
}
