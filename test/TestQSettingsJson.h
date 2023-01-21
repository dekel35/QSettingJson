#ifndef TESTQSETTINGJSON_H
#define TESTQSETTINGJSON_H

class QJsonObject;

class TestQSettingsJson {
public:
    TestQSettingsJson();
    void runTest();
    enum LogLevel {
        DBG,
        INFO,
        WARN,
        ERR,
    };

private:
    static bool testBasic1();
    static bool testBasic2();
    static bool testGroups();
    static bool testArray();
    static bool testNested();
    static bool testBool();
    static bool testDouble();
    static bool testNull();

    static bool testCommon(QJsonDocument jdoc);

    static bool compareJson(QJsonObject *obj1, QJsonObject *obj2);
    static void logger(LogLevel level, const char *format, ...);

    typedef bool funcdef();
    std::vector<funcdef *> array_;
    LogLevel curLevel;
    static std::string lastFuncName;
};

#endif // TESTQSETTINGJSON_H
