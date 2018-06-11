#include "factory.h"
#include "leveldb.h"


DatabasePtr makeDatabase(const std::string &dbPath)
{
    return std::make_shared<LevelDB>(dbPath);
}
