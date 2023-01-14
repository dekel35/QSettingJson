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
    static bool test_case1();
    static bool test_case2();
    static bool test_case3();
    static bool test_case4();

    static bool compareJson(QJsonObject *obj1, QJsonObject *obj2);
    static void logger(LogLevel level, const char *format, ...);

    typedef bool funcdef();
    std::vector<funcdef *> array_;
    LogLevel curLevel;
};

#endif // TESTQSETTINGJSON_H
