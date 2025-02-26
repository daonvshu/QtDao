#include "testconfigloader.h"

#include <qfile.h>

TestConfigLoader &TestConfigLoader::instance() {
    static TestConfigLoader configLoader;
    return configLoader;
}

TestConfigLoader::TestConfigLoader() {
    QFile file(TEST_CONFIG_PATH);
    if (!file.open(QIODevice::ReadOnly)) {
        return;
    }
    auto data = file.readAll();
    file.close();

    auto doc = QJsonDocument::fromJson(data);
    if (doc.isNull()) {
        return;
    }

    configData.fromJson(doc.object());
}
