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