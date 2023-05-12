#include "versionctl/versioncontrol.h"
#include "versionctl/version.h"

#include "config/configbuilder.h"

#include "dbexception.h"
#include "dao.h"

QTDAO_BEGIN_NAMESPACE

void VersionControl::checkLocalVersion() {

    auto versionTbReader = new SqliteEntityReaderProvider<Version>; //same suitable for mysql/sqlserver
    globalConfig->dbClient->createTable(versionTbReader);
    delete versionTbReader;

    int localVersion = getLocalVersion();
    if (localVersion > globalConfig->mVersion) {
        throw DaoException("The config version is smaller than the local version!");
    }

    if (localVersion == globalConfig->mVersion) {
        return;
    }

    invokeCreateTables();
    if (localVersion != -1) {
        invokeTableUpgrade(localVersion, globalConfig->mVersion);
    }
    updateLocalVersion();
}

int VersionControl::getLocalVersion() {
    auto d = dao::_select<Version>().build().one();
    return d.getVer();
}

void VersionControl::updateLocalVersion() {

    dao::_delete<Version>().build().deleteBy();

    auto v = Version(globalConfig->mVersion);
    dao::_insert<Version>().build().insert(v);
}

void VersionControl::invokeCreateTables() {
    const auto metaObj = QMetaType::metaObjectForType(QMetaType::type(getDelegateStr()));
    Q_ASSERT_X(metaObj != nullptr, "database initialize fail", "cannot invoke table delegate, use plugin vscode-qtdao in VSCODE to create entities");
    QObject* obj = metaObj->newInstance();
    QMetaObject::invokeMethod(obj, "createEntityTables");
    delete obj;
}

void VersionControl::invokeTableUpgrade(int oldVer, int newVer) {
    const auto metaObj = QMetaType::metaObjectForType(QMetaType::type(getDelegateStr()));
    Q_ASSERT_X(metaObj != nullptr, "database initialize fail", "cannot invoke table delegate, use plugin vscode-qtdao in VSCODE to create entities");
    QObject* obj = metaObj->newInstance();
    QMetaObject::invokeMethod(obj, "entityTablesUpgrade", Q_ARG(int, oldVer), Q_ARG(int, newVer));
    delete obj;
}

QByteArray VersionControl::getDelegateStr() {
    QByteArray entityDelegate;
    switch (globalConfig->configType) {
        case ConfigType::Sqlite:
            entityDelegate = "DaoSqlite::SqliteEntityDelegate*";
            break;
        case ConfigType::Mysql:
            entityDelegate = "DaoMysql::MysqlEntityDelegate*";
            break;
        case ConfigType::SqlServer:
            entityDelegate = "DaoSqlServer::SqlServerEntityDelegate*";
            break;
    }
    return entityDelegate;
}

QTDAO_END_NAMESPACE