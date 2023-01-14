#include <iostream>
#include <vector>
#include <QJsonDocument>

#include "TestQSettingsJson.h"
#include "QSettingsJson.h"
  
TestQSettingsJson::TestQSettingsJson() 
{
    array_.push_back(test_case1);
    array_.push_back(test_case2);
    array_.push_back(test_case3);

}

void TestQSettingsJson::runTest()
{
    int i=0;
    for (auto f : array_) {
        auto ret = f();
        std::cout << "========== test " << ++i << " result = " << (ret ? " GOOD " : " BAD ") << "=======================" << std::endl;
    }
}

bool TestQSettingsJson::compareJson(QJsonObject *obj1, QJsonObject *obj2)
{  
    QJsonDocument doc1(*obj1);
    QJsonDocument doc2(*obj2);
    auto r = doc1.toJson(QJsonDocument::Compact).compare(doc2.toJson(QJsonDocument::Compact));
    bool ret = (r == 0);
    return ret;
}


bool TestQSettingsJson::test_case1()
{
   QSettingsJson set1;
   set1.clear();

   set1.setValue("a", "b");
   logger(INFO, "set1 value %s\n",set1.value("a").toString().toStdString().c_str());
   auto jobj = set1.exportJson();
   QSettingsJson importedSettings(jobj);


   assert(importedSettings.allKeys().count() == set1.allKeys().count());
   assert(importedSettings.value("a") == set1.value("a"));
   return true;
}

bool TestQSettingsJson::test_case2()
{
    QSettingsJson settings;
    settings.clear();
    settings.setValue("g1/a", "b");
   
    auto jobj = settings.exportJson();
    QSettingsJson importedSettings(jobj);

    assert(importedSettings.allKeys().count() == settings.allKeys().count());

    auto jobj2 = importedSettings.exportJson();
    QJsonDocument jdoc2(*jobj2);
    bool equal = compareJson(jobj, jobj2);
    logger(INFO, "%s equal: %s", jdoc2.toJson().toStdString().c_str(), (equal ? "true" : "false"));
    return equal;
}

bool TestQSettingsJson::test_case3()
{
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
   QJsonDocument jdoc2(*jobj2);
   bool equal = compareJson(jobj, jobj2);
   logger(INFO, "%s equal: %s", jdoc2.toJson().toStdString().c_str(), (equal ? "true" : "false"));
   return equal;
}

void TestQSettingsJson::logger(LogLevel level, const char * format, ...)
{
    if (level <= INFO) {
        return;
    }

    va_list ap;
    va_start(ap, format);
    auto ret = ::vprintf(format, ap);
    va_end(ap);

}
