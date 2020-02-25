#include "DaoUtil.h"

dao::ErrPrintType dao::pType = dao::ETYPE_CONSOLE;
int dao::bindCount = 0;

DbLoader::SqlCfg DbLoader::sqlCfg;
DbLoader::SqlClient* DbLoader::sqlClient = nullptr;

dao::DaoExecutor::DaoExecutor(const ExecutorData& executorData) {
    this->executorData = executorData;
}

void dao::DaoExecutor::createSqlHead() {
    auto bindSqlExp = executorData.bindCondition.getExpressionStr();
    switch (executorData.operateType) {
        case dao::OPERATE_QUERY:
            sqlExpression = "select ";
            sqlExpression += bindSqlExp.isEmpty() ? "*" : bindSqlExp;
            sqlExpression += " from ";
            break;
        case dao::OPERATE_INSERT:
            sqlExpression = "insert into ";
            break;
        case dao::OPERATE_INSERT_OR_REPLACE:
            sqlExpression = "replace into ";
            break;
        case dao::OPERATE_UPDATE:
            sqlExpression = "update ";
            break;
        case dao::OPERATE_DELETE:
            sqlExpression = "delete from ";
            break;
        case dao::OPERATE_COUNT:
            sqlExpression = "select count(*) from ";
            break;
        default:
            break;
    }
    sqlExpression += getTableName();
    if (!executorData.recursiveQueryData.isEmpty) {
        sqlExpression = executorData.recursiveQueryData.recursiveStatement + sqlExpression;
    }
}

void dao::DaoExecutor::concatSqlStatement() {
    auto setSqlExp = executorData.setCondition.getExpressionStr();
    auto whereSqlExp = executorData.whereCondition.getExpressionStr();
    auto subWhereSqlExp = executorData.subWhereCondition.getExpressionStr(true);
    
    if (!setSqlExp.isEmpty()) {
        sqlExpression += " set ";
        sqlExpression += setSqlExp;
    }
    if (!whereSqlExp.isEmpty()) {
        sqlExpression += " where ";
        sqlExpression += whereSqlExp;
    }
    if (!subWhereSqlExp.isEmpty()) {
        sqlExpression += ' ';
        sqlExpression += subWhereSqlExp;
    }
}

void dao::DaoExecutor::mergeValueList() {
    if (!executorData.recursiveQueryData.isEmpty) {
        valueList.append(executorData.recursiveQueryData.recursiveContainValues);
    }
    valueList.append(executorData.bindCondition.getValueList());
    valueList.append(executorData.bindTableNameContainValues);
    valueList.append(executorData.setCondition.getValueList());
    valueList.append(executorData.whereCondition.getValueList());
    valueList.append(executorData.subWhereCondition.getValueList());
}

void dao::DaoExecutor::bindValue(QSqlQuery & query) {
    mergeValueList();
    for (const auto& d : valueList) {
        query.addBindValue(d, d.type() == QMetaType::QByteArray ? QSql::Binary : QSql::In);
    }
}

dao::DaoJoinExecutor::DaoJoinExecutor(QList<SqlJoinBuilder::JoinInfo> joinInfo, const QString sql, const QVariantList& valueList) {
    this->joinInfo = joinInfo;
    this->sqlExpression = sql;
    this->valueList = valueList;
}

dao::DaoJoinExecutor::DaoJoinExecutorItemWarpper dao::DaoJoinExecutor::list() {
    auto db = ConnectionPool::openConnection();
    QSqlQuery query(db);
    query.prepare(sqlExpression);
    for (const auto& a : valueList) {
        query.addBindValue(a, a.type() == QMetaType::QByteArray ? QSql::Binary : QSql::In);
    }
#ifdef PRINT_SQL_STATEMENT
    qDebug() << sqlExpression;
#endif // PRINT_SQL_STATEMENT
    DaoJoinExecutorItemWarpper warpperData(joinInfo);
    if (!query.exec()) {
        dao::printLog(query.lastError().text(), sqlExpression);
    } else {
        while (query.next()) {
            QVariantList data;
            auto record = query.record();
            for (int i = 0; i < record.count(); i++) {
                data.append(record.value(i));
            }
            warpperData.append(DaoJoinExecutorItem(data, warpperData.joinInfo));
        }
    }
    ConnectionPool::closeConnection(db);
    return warpperData;
}

dao::DaoJoinExecutor dao::SqlJoinBuilder::build() {
    QString sql = "select ";
    QVariantList valueList;
    //select (bindExpression) from (master) a (joinExpress) (subExpression)
    const JoinInfo* joinMasterTableInfo;
    QString bindExpression, joinExpression, subExpression;
    QVariantList valuesInBindExpression, valuesInJoinConditions, valuesInMasterSubConditions;
    int i = 0;
    for (const auto& info : joinInfos) {
        auto bindExpStr = info.bindCondition.getExpressionStr();
        bindExpression += bindExpStr;
        if (!bindExpStr.isEmpty()) {
            bindExpression += ",";
        }
        valuesInBindExpression.append(info.bindCondition.getValueList());

        if (info.joinType == JOIN_NULL) {
            joinMasterTableInfo = &info;
            subExpression += info.whereCondition.getExpressionStr();
            QString subWhExpression = info.subWhCondition.getExpressionStr(true);
            if (!subWhExpression.isEmpty()) {
                subExpression += ' ' + subWhExpression;
            }
            valuesInMasterSubConditions.append(info.whereCondition.getValueList());
            valuesInMasterSubConditions.append(info.subWhCondition.getValueList());
        } else {
            QString joinStr;
            switch (info.joinType) {
                default:
                case JOIN_LEFT:
                    joinStr = " left join ";
                    break;
                case JOIN_RIGHT:
                    joinStr = " right join ";
                    break;
                case JOIN_INNER:
                    joinStr = " inner join ";
                    break;
                case JOIN_FULL_OUTER:
                    joinStr = " full outer join ";
                    break;
            }
            QString no = ('a' + (i++ + 1));
            joinExpression += joinStr + info.tbName + ' ' + no;
            QString whExpression = info.whereCondition.getExpressionStr();
            if (!whExpression.isEmpty()) {
                joinExpression += " on " + info.whereCondition.getExpressionStr();
                QString subWhExpression = info.subWhCondition.getExpressionStr(true);
                if (!subWhExpression.isEmpty()) {
                    joinExpression += ' ' + subWhExpression;
                }
            }
            valuesInJoinConditions.append(info.whereCondition.getValueList());
            valuesInJoinConditions.append(info.subWhCondition.getValueList());
        }
    }
    sql += bindExpression.left(bindExpression.length() - 1);
    sql += " from " + joinMasterTableInfo->tbName + " a";
    sql += joinExpression;
    if (!subExpression.isEmpty()) {
        sql += " where ";
        sql += subExpression;
    }

    valueList.append(valuesInBindExpression);
    valueList.append(valuesInJoinConditions);
    valueList.append(valuesInMasterSubConditions);

    return DaoJoinExecutor(joinInfos, sql, valueList);
}

void dao::printLog(const QString & lastErr, const QString & sql) {
    Q_ASSERT_X(lastErr.isEmpty(), "dao::printLog", ("lastErr:" + lastErr + " sql:" + sql).toUtf8());
    if (pType == ETYPE_NONE)
        return;
    QString errStr = "--------sql error--------\r\nlast error:%1\r\n";
    errStr = errStr.arg(lastErr);
    switch (pType) {
        case dao::ETYPE_FILE:
            {
                QFile outFile("debug.log");
                outFile.open(QIODevice::WriteOnly | QIODevice::Append);
                QTextStream ts(&outFile);
                QString curTime = "[%1]";
                ts << errStr << curTime.arg(QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss:zzz")) << endl;
            }
            break;
        case dao::ETYPE_CONSOLE:
            {
                auto splitStr = errStr.split("\r\n");
                for (const auto& s : splitStr) {
                    qDebug() << s;
                }
            }
            break;
        case dao::ETYPE_ERRBOX:
            {
                auto e = new QErrorMessage;
                e->setWindowTitle(QStringLiteral("sql´íÎó"));
                e->showMessage(errStr);
            }
            break;
        default:
            break;
    }
}

QVariant DbLoader::readDbSetting(const QString & key, const QString & default) {
    QString iniFilePath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/db.ini";
    QSettings settings(iniFilePath, QSettings::IniFormat);
    return settings.value("Db/" + key, default);
}

void DbLoader::writeDbSetting(const QString & key, const QVariant & value) {
    QString iniFilePath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/db.ini";
    QSettings settings(iniFilePath, QSettings::IniFormat);
    settings.setValue("Db/" + key, value);
}

void DbLoader::loadConfigure(const QString & fileName) {
    QDomDocument doc;
    QFile file(fileName);
    auto s = file.open(QIODevice::ReadOnly);
    Q_ASSERT_X(s, "DbLoader::loadCondigure", "open config file fail!");
    s = doc.setContent(&file);
    Q_ASSERT_X(s, "DbLoader::loadCondigure", "dom set content fail!");
    file.close();
    auto root = doc.documentElement();

    auto client = root.attribute("type");
    if (client == "mysql") {
        sqlCfg.dbType = "QMYSQL";
        sqlClient = new MysqlClient;
    } else if (client == "sqlite") {
        sqlCfg.dbType = "QSQLITE";
        sqlClient = new SqliteClient;
    } else if (client == "sqlserver") {
        sqlCfg.dbType = "QODBC";
        sqlClient = new SqlServerClient;
    }

    sqlCfg.version = root.attribute("version").toInt();
    sqlCfg.dbName = root.attribute("dbname");

    auto options = root.childNodes();
    for (int i = 0; i < options.count(); i++) {
        auto c = options.at(i).toElement();
        if (c.tagName() == "dbname") {
            sqlCfg.dbName = c.text();
        } else if (c.tagName() == "dbhost") {
            sqlCfg.dbHost = c.text();
        } else if (c.tagName() == "dbuname") {
            sqlCfg.dbUName = c.text();
        } else if (c.tagName() == "dbpcc") {
            sqlCfg.dbPcc = c.text();
        } else if (c.tagName() == "dbport") {
            sqlCfg.dbPort = c.text().toInt();
        } else if (c.tagName() == "dboption") {
            sqlCfg.dbOption = c.text();
        }
    }
    if (sqlCfg.dbHost.isEmpty()) {
        sqlCfg.dbHost = readDbSetting(DB_SETTING_HOST, "localhost").toString();
    }
}

bool DbCreatorHelper::testConnect() {
    if (DbLoader::isSqlite()) {
        auto appLocal = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
        QDir dir;
        if (!dir.exists(appLocal)) {
            dir.mkdir(appLocal);
        }
        return true;
    }
    if (DbLoader::isMysql()) {
        auto db = ConnectionPool::prepareConnect("testMysql", "mysql");
        if (!db.open()) {
            qDebug() << "connect mysql fail! err = " << db.lastError();
            return false;
        }
        auto db2 = ConnectionPool::prepareConnect("testDb", DbLoader::getConfigure().dbName);
        if (!db2.open()) {
            //create target database
            QSqlQuery query(db);
            QString sql = "create database if not exists %1 default character set utf8 COLLATE utf8_general_ci";
            sql = sql.arg(DbLoader::getConfigure().dbName);
            if (!query.exec(sql)) {
                qDebug() << "create target database fail! err = " << db.lastError();
                db.close();
                return false;
            }
        }
        db.close();
        db2.close();
        return true;
    } else if (DbLoader::isSqlServer()) {
        auto db = ConnectionPool::prepareConnect("testSqlServer", DbLoader::getConfigure().dbName);
        if (!db.open()) {
            qDebug() << "connect sqlserver fail! err = " << db.lastError();
            return false;
        }
        db.close();
        return true;
    }
    return false;
}

bool DbCreatorHelper::checkDbVersion() {
    return DbLoader::readDbSetting(DB_SETTING_VERSION, "-1").toInt() == DbLoader::getConfigure().version;
}

void DbCreatorHelper::resetDbVersion() {
    DbLoader::writeDbSetting(DB_SETTING_VERSION, DbLoader::getConfigure().version);
}

bool DbCreatorHelper::checkTableExist(const QString & tbName) {
    bool exist = false;
    execSql(DbLoader::getClient()->findTableStatement(tbName), [&](auto& query) {
        if (query.next()) {
            exist = true;
        }
    }, [](auto& lastErr) {
        qDebug() << "check table exist fail, err = " << lastErr;
    });
    return exist;
}

bool DbCreatorHelper::renameTable(const QString & oldTb, const QString & newTb) {
    bool success = true;
    execSql(DbLoader::getClient()->renameTableStatement(oldTb, newTb), [&](auto& lastErr) {
        qDebug() << "rename table fail, err = " << lastErr;
        success = false;
    });
    return success;
}

bool DbCreatorHelper::dropTable(const QString & tb) {
    QString sql = "drop table " + tb;
    bool success = true;
    execSql(sql, [&](auto& lastErr) {
        qDebug() << "drop table fail, err = " << lastErr;
        success = false;
    });
    return success;
}

bool DbCreatorHelper::truncateTable(const QString & tb) {
    QString sql = "truncate table " + tb;
    bool success = true;
    execSql(sql, [&](auto& lastErr) {
        qDebug() << "truncate table fail, err = " << lastErr;
        success = false;
    });
    return success;
}