#include <iostream>
#include <vector>
#include <QJsonDocument>

#include "TestQSettingsJson.h"
#include "QSettingsJson.h"

std::string TestQSettingsJson::lastFuncName;

TestQSettingsJson::TestQSettingsJson()
{
    array_.push_back(testBasic1);
    array_.push_back(testBasic2);
    array_.push_back(testGroups);
    array_.push_back(testArray);
    array_.push_back(testNested);
    array_.push_back(testBool);
    array_.push_back(testDouble);
    array_.push_back(testNull);
}

void TestQSettingsJson::runTest()
{
    int i = 0;
    bool accum = true;
    for (auto f : array_) {
        auto ret = f();
        accum &= ret;
        std::cout << "========== " << ++i << ". test " << TestQSettingsJson::lastFuncName 
                  << " result = " << (ret ? " GOOD " : " BAD ")
                  << "=======================" << std::endl;
    }
    std::cout << (accum ? "" : "NOT ") << "all tests werc successful" << std::endl;
}

bool TestQSettingsJson::compareJson(QJsonObject *obj1, QJsonObject *obj2)
{
    QJsonDocument doc1(*obj1);
    QJsonDocument doc2(*obj2);
    logger(INFO, "compare 1\n%s\n", doc1.toJson().toStdString().c_str());
    logger(INFO, "compare 2\n%s\n", doc2.toJson().toStdString().c_str());
    auto r = doc1.toJson(QJsonDocument::Compact)
                 .compare(doc2.toJson(QJsonDocument::Compact));
    bool ret = (r == 0);
    return ret;
}

bool TestQSettingsJson::testBasic1()
{
    lastFuncName = __func__;
    QSettingsJson set1;
    set1.clear();

    set1.setValue("a", "b");
    logger(INFO, "set1 value %s\n",
           set1.value("a").toString().toStdString().c_str());
    auto jobj = set1.exportJson();
    QSettingsJson importedSettings(jobj);

    assert(importedSettings.allKeys().count() == set1.allKeys().count());
    assert(importedSettings.value("a") == set1.value("a"));
    return true;
}

bool TestQSettingsJson::testBasic2()
{
    lastFuncName = __func__;
    QSettingsJson settings;
    settings.clear();
    settings.setValue("g1/a", "b");

    auto jobj = settings.exportJson();
    QSettingsJson importedSettings(jobj);

    assert(importedSettings.allKeys().count() == settings.allKeys().count());

    auto jobj2 = importedSettings.exportJson();
    QJsonDocument jdoc2(*jobj2);
    bool equal = compareJson(jobj, jobj2);
    logger(INFO, "%s equal: %s", jdoc2.toJson().toStdString().c_str(),
           (equal ? "true" : "false"));
    return equal;
}

bool TestQSettingsJson::testGroups()
{
    lastFuncName = __func__;
    QSettingsJson settings;
    settings.clear();
    settings.beginGroup("fridge");
    settings.setValue("color", "white");
    settings.setValue("size", 32);
    settings.endGroup();
    settings.setValue("sofa", true);
    settings.setValue("tv", false);

    auto jobj = settings.exportJson();
    QSettingsJson importedSettings(jobj);

    assert(importedSettings.allKeys().count() == settings.allKeys().count());

    auto jobj2 = importedSettings.exportJson();
    bool equal = compareJson(jobj, jobj2);
    return equal;
}

bool TestQSettingsJson::testArray()
{
    lastFuncName = __func__;
    QJsonDocument jdoc = QJsonDocument::fromJson("{ \"key\" : [\"1\", \"2\"] }");
    return testCommon(jdoc);
}

bool TestQSettingsJson::testNested()
{
    lastFuncName = __func__;
    QJsonDocument jdoc = QJsonDocument::fromJson("{ \"key1\" : { \"key2\": {\"key3\" : {\"key4\" : {\"key5\" : {\"key6\" : [\"1\", \"2\"] }}}}}}");
    return testCommon(jdoc);
}

bool TestQSettingsJson::testBool()
{
    lastFuncName = __func__;
    QJsonDocument jdoc = QJsonDocument::fromJson("{ \"key1\" : true, \"key2\" : false}");
    return testCommon(jdoc);
}

bool TestQSettingsJson::testDouble()
{
    lastFuncName = __func__;
   QJsonDocument jdoc = QJsonDocument::fromJson("{ \"key1\" : 1.1, \"key2\" : 2.2, \"key3\" : 42}");
   return testCommon(jdoc);
}

bool TestQSettingsJson::testNull()
{
    lastFuncName = __func__;
   QJsonDocument jdoc = QJsonDocument::fromJson("{ \"key1\" : null, \"key2\" : \"something\"}");
   return testCommon(jdoc);
}

bool TestQSettingsJson::testCommon(QJsonDocument jdoc)
{
    QJsonObject jobj = jdoc.object();
    logger(INFO, jdoc.toJson().toStdString().c_str());

    QSettingsJson importedSettings(&jobj);
    auto jobj2 = importedSettings.exportJson();
    QJsonDocument jdoc2(*jobj2);
    logger(INFO, jdoc2.toJson().toStdString().c_str());
    bool equal = compareJson(&jobj, jobj2);
    return equal;
}

void TestQSettingsJson::logger(LogLevel level, const char *format, ...)
{
    if (level <= INFO) {
        return;
    }

    va_list ap;
    va_start(ap, format);
    auto ret = ::vprintf(format, ap);
    va_end(ap);
}

