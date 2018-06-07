#include "leveldb.h"
#include "logger.h"

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

void LevelDB::load(ProposerData & data)
{
    // load proposor data
    std::string key = "proposer";
    std::string value;
    auto const status = db_->Get(leveldb::ReadOptions(), key, &value);
    checkStatus(status);
    Logger::debug() << "load proposer: " << value;
    checkData(data.fromString(value));
}

void LevelDB::save(const ProposerData & data)
{
    // save proposor data
    std::string key = "proposer";
    std::string value = data.toString();
    leveldb::Slice const keySlice(key.data(), key.size());
    leveldb::Slice const valueSlice(value.data(), value.size());
    Logger::debug() << "save proposer: " << value;
    auto const status = db_->Put(leveldb::WriteOptions(), keySlice, valueSlice);
    checkStatus(status);
}

void LevelDB::load(AcceptorData & data)
{
    // load acceptor data
    std::string key = "acceptor";
    std::string value;
    auto const status = db_->Get(leveldb::ReadOptions(), key, &value);
    checkStatus(status);
    Logger::debug() << "load acceptor: " << value;
    checkData(data.fromString(value));
}

void LevelDB::save(const AcceptorData & data)
{
    // save acceptor data
    std::string key = "acceptor";
    std::string value = data.toString();
    leveldb::Slice const keySlice(key.data(), key.size());
    leveldb::Slice const valueSlice(value.data(), value.size());
    Logger::debug() << "save acceptor: " << value;
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
