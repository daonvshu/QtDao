# ----------------------------------------------------
# This file is generated by the Qt Visual Studio Tools.
# ------------------------------------------------------

# This is a reminder that you are using a generated .pro file.
# Remove it when you are finished editing this file.


HEADERS += ../include/connectionpool.h \
    ../include/dao.h \
    ../include/dbexception.h \
    ../include/global.h \
    ../include/configselector.h \
    ../include/builder/option/columnbuilder.h \
    ../include/builder/option/constraintbuilder.h \
    ../include/builder/option/debugbuilder.h \
    ../include/builder/option/filterbuilder.h \
    ../include/builder/option/frombuilder.h \
    ../include/builder/option/fromselectbuilder.h \
    ../include/builder/option/joinconnectbuilder.h \
    ../include/builder/option/onconditionbuilder.h \
    ../include/builder/option/setbuilder.h \
    ../include/builder/option/unionbuilder.h \
    ../include/builder/option/sessionbuilder.h \
    ../include/builder/countbuilder.h \
    ../include/builder/deletebuilder.h \
    ../include/builder/insertbuilder.h \
    ../include/builder/insertintoselectbuilder.h \
    ../include/builder/joinbuilder.h \
    ../include/builder/recursivequerybuilder.h \
    ../include/builder/selectbuilder.h \
    ../include/builder/updatebuilder.h \
    ../include/builder/upsertbuilder.h \
    ../include/condition/connector/connectable.h \
    ../include/condition/connector/constraintconnector.h \
    ../include/condition/connector/entityconnector.h \
    ../include/condition/connector/fieldconnector.h \
    ../include/condition/connector/groupconnector.h \
    ../include/condition/conditionconstraint.h \
    ../include/condition/conditionoperator.h \
    ../include/condition/entitycondition.h \
    ../include/condition/entityfield.h \
    ../include/condition/functioncondition.h \
    ../include/condition/foreignkey.h \
    ../include/config/configbuilder.h \
    ../include/config/configmysql.h \
    ../include/config/configsqlite.h \
    ../include/config/configsqlserver.h \
    ../include/config/configmanager.h \
    ../include/config/configtype.h \
    ../include/dbclients/abstractclient.h \
    ../include/dbclients/mysqlclient.h \
    ../include/dbclients/sqliteclient.h \
    ../include/dbclients/sqlserverclient.h \
    ../include/macro/macro.h \
    ../include/query/reader/builderreaderinterface.h \
    ../include/query/reader/entityreaderinterface.h \
    ../include/query/basequery.h \
    ../include/query/delete.h \
    ../include/query/deleteimpl.h \
    ../include/query/explaininfo.h \
    ../include/query/insert.h \
    ../include/query/insertimpl.h \
    ../include/query/insertintoselect.h \
    ../include/query/insertintoselectimpl.h \
    ../include/query/join.h \
    ../include/query/joinimpl.h \
    ../include/query/select.h \
    ../include/query/selectimpl.h \
    ../include/query/update.h \
    ../include/query/updateimpl.h \
    ../include/query/upsert.h \
    ../include/query/upsertimpl.h \
    ../include/utils/dbupgrader.h \
    ../include/utils/logging.h \
    ../include/utils/listutils.h \
    ../include/utils/serializing.h \
    ../include/versionctl/version.h \
    ../include/versionctl/versioncontrol.h

SOURCES += ../src/connectionpool.cpp \
    ../src/qtdao.cpp \
    ../src/configselector.cpp \
    ../src/builder/frombuilder.cpp \
    ../src/builder/unionbuilder.cpp \
    ../src/condition/connector/connectable.cpp \
    ../src/condition/connector/entityconnector.cpp \
    ../src/condition/connector/groupconnector.cpp \
    ../src/condition/conditionconstraint.cpp \
    ../src/condition/entitycondition.cpp \
    ../src/condition/functioncondition.cpp \
    ../src/config/configbuilder.cpp \
    ../src/config/configmysql.cpp \
    ../src/config/configsqlite.cpp \
    ../src/config/configsqlserver.cpp \
    ../src/config/configmanager.cpp \
    ../src/dbclients/abstractclient.cpp \
    ../src/dbclients/mysqlclient.cpp \
    ../src/dbclients/sqliteclient.cpp \
    ../src/dbclients/sqlserverclient.cpp \
    ../src/query/basequery.cpp \
    ../src/query/deleteimpl.cpp \
    ../src/query/insertimpl.cpp \
    ../src/query/insertintoselectimpl.cpp \
    ../src/query/joinimpl.cpp \
    ../src/query/selectimpl.cpp \
    ../src/query/updateimpl.cpp \
    ../src/query/upsertimpl.cpp
    ../src/utils/logging.cpp \
    ../src/utils/dbupgrader.cpp \
    ../src/versionctl/versioncontrol.cpp

