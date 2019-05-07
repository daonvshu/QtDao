#include "DaoBase.h"

dao::ErrPrintType dao::pType = dao::ETYPE_CONSOLE;
int dao::bindCount = 0;

DbLoader::SqlCfg DbLoader::sqlCfg;
DbLoader::SqlClient* DbLoader::sqlClient = nullptr;