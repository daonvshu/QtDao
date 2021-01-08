#pragma once

enum EngineModel {
    Engine_Sqlite = 0,
    Engine_Mysql,
    Engine_SqlServer
};

class EngineModelSelector {
public:
    EngineModelSelector(EngineModel engineModel) {
        this->engineModel = engineModel;
    }

protected:
    EngineModel engineModel;
};

#define PASSSQLITE if (engineModel == Engine_Sqlite) return
#define PASSMYSQL if (engineModel == Engine_Mysql) return
#define PASSSQLSERVER if (engineModel == Engine_SqlServer) return