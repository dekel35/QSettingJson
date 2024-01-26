#ifndef TESTQSETTINGJSON_H
#define TESTQSETTINGJSON_H

#include "QSettingsJson.h"

class QJsonObject;

void dumpSettings(QSettings & settings, int depth = 0, int indent=0);

class TestQSettingsJson {
public:
    TestQSettingsJson();
    void runTest();
    enum LogLevel {
        DBG,
        INFO,
        MSG,
        WARN,
        ERR,
    };

private:
    static bool testBasic1();
    static bool testBasic2();
    static bool testGroups();
    static bool testArray();
    static bool testNestedSmall();
    static bool testNested();
    static bool test2Nested();
    static bool testBool();
    static bool testDouble();
    static bool testNull();
    static bool testQColor();
    static bool testQLocale();

    static bool testCommon(QJsonDocument &jdoc, QString key = "", JsonFuncPtr func = nullptr, VariantFuncPtr = nullptr);

    static bool compareSettings(QSettings& set1, QSettings& set2, int dept=0);

    static bool compareJson(QJsonObject *obj1, QJsonObject *obj2);
    static void logger(LogLevel level, const char *format, ...);

    typedef bool funcdef();
    std::vector<funcdef *> array_;
    LogLevel curLevel;
    static std::string lastFuncName;
};

#endif // TESTQSETTINGJSON_H
