#ifndef TESTQSETTINGJSON_H
#define TESTQSETTINGJSON_H

#include "QSettingsJson.h"

class QJsonObject;

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

    static void dumpSettings(QSettings& settings, int depth, int indet = 0);
    static bool compareSettings(QSettings& set1, QSettings& set2, int dept=0);

    static bool compareJson(QJsonObject *obj1, QJsonObject *obj2);
    static int logger(LogLevel level, const char *format, ...);

    typedef bool funcdef();
    QVector<funcdef *> array_;
    LogLevel curLevel;
    static QString lastFuncName;
};

#endif // TESTQSETTINGJSON_H
