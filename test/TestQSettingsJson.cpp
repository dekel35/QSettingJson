#include <iostream>
#include <vector>
#include <QJsonDocument>
#include <QJsonArray>
#include <QtGui/QColor>

#include "TestQSettingsJson.h"
#include "QSettingsJson.h"

QString TestQSettingsJson::lastFuncName;
QString jobjectToStr(const QJsonObject& obj) {
    return QJsonDocument(obj).toJson();
}

TestQSettingsJson::TestQSettingsJson()
{
    array_.push_back(testBasic1);
    array_.push_back(testBasic2);
    array_.push_back(testGroups);
    array_.push_back(testArray);
    array_.push_back(testNestedSmall);
    array_.push_back(testNested);
    array_.push_back(test2Nested);
    array_.push_back(testBool);
    array_.push_back(testDouble);
    array_.push_back(testNull);
    array_.push_back(testQColor);
    array_.push_back(testQLocale);
}

void TestQSettingsJson::runTest()
{
    int i = 0;
    bool accum = true;
    QList<QString> errList;

    for (auto f : array_) {
        auto ret = f();
        accum &= ret;
        if (ret == false) {
            errList += lastFuncName;
        }
        qDebug() << "========== " << ++i << ". test " << lastFuncName
                  << " result = " << (ret ? " GOOD " : " BAD ")
                  << "=======================";
    }
    qDebug() << (accum ? "" : "NOT ") << "all tests were successful";
    for (auto e: errList) {
        qDebug() << "\t" << e;
    }
}

bool TestQSettingsJson::compareJson(QJsonObject *obj1, QJsonObject *obj2)
{
    QJsonDocument doc1(*obj1);
    QJsonDocument doc2(*obj2);
    logger(MSG, "compare 1\n%s\n", doc1.toJson().toStdString().c_str());
    logger(MSG, "compare 2\n%s\n", doc2.toJson().toStdString().c_str());
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
    QSettingsJson::clearMetadata();
    set1.setValue("a", "b");
    auto jobj = set1.exportJson();
    QSettingsJson importedSettings(*jobj);
    delete jobj;

    assert(importedSettings.allKeys().count() == set1.allKeys().count());
    assert(importedSettings.value("a") == set1.value("a"));
    return true;
}

bool TestQSettingsJson::testBasic2()
{
    lastFuncName = __func__;
    QSettingsJson settings;
    settings.clear();
    QSettingsJson::clearMetadata();
    settings.setValue("g1/a", "b");

    auto jobj = settings.exportJson();
    QSettingsJson importedSettings(*jobj);

    assert(importedSettings.allKeys().count() == settings.allKeys().count());

    auto jobj2 = importedSettings.exportJson();
    QJsonDocument jdoc2(*jobj2);
    bool equal = compareJson(jobj, jobj2);
    delete jobj; 
    delete jobj2;
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
    QSettingsJson importedSettings(*jobj);

    assert(importedSettings.allKeys().count() == settings.allKeys().count());

    auto jobj2 = importedSettings.exportJson();
    bool equal = compareJson(jobj, jobj2);
    delete jobj;
    delete jobj2;
    return equal;
}

bool TestQSettingsJson::testArray()
{
    lastFuncName = __func__;
    QJsonDocument jdoc = QJsonDocument::fromJson("{ \"key\" : [\"1\", \"2\", \"3\"] }");
    return testCommon(jdoc);
}

bool TestQSettingsJson::testNestedSmall()
{
    lastFuncName = __func__;
    QJsonDocument jdoc = QJsonDocument::fromJson("{ \"key1\" : { \"key2\": { \"key3\":  [\"1\", \"2\"] }}}");
    return testCommon(jdoc);
}

bool TestQSettingsJson::testNested()
{
    lastFuncName = __func__;
    QJsonDocument jdoc = QJsonDocument::fromJson("{ \"key1\" : { \"key2\": {\"key3\" : {\"key4\" : {\"key5\" : {\"key6\" : [\"1\", \"2\"] }}}}}}");
    return testCommon(jdoc);
}

bool TestQSettingsJson::test2Nested()
{
    lastFuncName = __func__;
    QJsonDocument jdoc = QJsonDocument::fromJson("{ \"key1\" : { \"key11\": \"11\" } , \"key2\" : { \"key22\": \"22\" } }");
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

//This callback function is used by the next test to correctly convert QColor to a json string (see testQColor())
QString MyQColorConvertor(const QVariant& value)
{
    if (value.canConvert<QColor>()) {
        QString ba (
            QString("\"") +
            value.toString() +
            QString("\"")
            );
        return QString(ba);
    }
    else return nullptr;
}

//This callback function will be used by the next test to correctly convert a color json representation to QColor variant
QVariant MyQColorVariant(const QJsonObject &color, QString value)
{
    QColor qcol1(value);
    return QVariant(qcol1);
}

bool TestQSettingsJson::testQColor()
{
    lastFuncName = __func__;
    QSettingsJson::clearMetadata();
    QSettingsJson set;
    set.clear();
    set.addQMetaType("QColor", MyQColorConvertor, MyQColorVariant);
    set.setValue("color", QColor(0, 255, 0));
    // dumpSettings(set);
    auto jobj = set.exportJson();
    QJsonDocument jdoc(*jobj);
    return testCommon(jdoc, "QColor", MyQColorConvertor, MyQColorVariant);
}

//This function will be used by the next test to correctly convert QLocale to a json string (see testQLocale())
QString MyQLocalConvertor(const QVariant& value)
{
    if (value.canConvert<QLocale>()) {
        QLocale embeddedType = value.value<QLocale>();
        QByteArray ba (
            QByteArray("\"") +
            QByteArray(embeddedType.bcp47Name().toLatin1()) +
            QByteArray("\"")
        );
        return QString(ba);
    }
    else return nullptr;
}

//This function will be used by the next test to convert a color json representation to return a QLocale variant
QVariant MyQLocalVariant(const QJsonObject& locale, QString value)
{
    QLocale qloc(value);
    return QVariant(qloc);
}


bool TestQSettingsJson::testQLocale()
{
    lastFuncName = __func__;
    QSettingsJson set;
    QSettingsJson::clearMetadata();
    set.clear();
    set.addQMetaType("QLocale", MyQLocalConvertor, MyQLocalVariant);
    set.setValue("locale", QLocale("el_CY"));
    //dumpSettings(set);
    auto jobj = set.exportJson();
    QJsonDocument jdoc(*jobj);

    return testCommon(jdoc, "QLocale", MyQLocalConvertor, MyQLocalVariant);
}

bool TestQSettingsJson::testCommon(QJsonDocument &jdoc, QString key, JsonFuncPtr jsonFunc, VariantFuncPtr variantFunc)
{
    QJsonObject jobj = jdoc.object();
    logger(DBG, jdoc.toJson().toStdString().c_str());

    QSettingsJson::clearMetadata();
    if (!key.isEmpty()) {
        QSettingsJson::addQMetaType(key, jsonFunc, variantFunc);
    }
    QSettingsJson exportedSettings(jobj);
    QJsonObject *jobj2 = exportedSettings.exportJson();
    logger(DBG, QJsonDocument(*jobj2).toJson().data());
    bool equal = compareJson(&jobj, jobj2);
    if (equal) {
        QSettingsJson settingsFromJson(*jobj2);
        // dumpSettings(exportedSettings);
        // dumpSettings(settingsFromJson);
        equal = compareSettings(exportedSettings, settingsFromJson);
    }
    delete jobj2;
    return equal;
}

void TestQSettingsJson::dumpSettings(QSettings& settings, int depth, int originDepth) //TODO: enter into class
{
    for (auto s : settings.childGroups()) {
        qDebug() << QString(depth*4, ' ') << originDepth <<  " --- group " << s << " ---";
        settings.beginGroup(s);
        dumpSettings(settings, depth+1, originDepth);
        settings.endGroup();
    }
    for (auto s : settings.childKeys()) {
         qDebug() << QString(depth*4 , ' ') << originDepth << " Key:" << s << "(" << typeid(s).name() << ") " << " Value:" << settings.value(s).toString()  <<
           " (" <<  settings.value(s).typeName() << ")";
         switch (settings.value(s).typeId()) {
            case QMetaType::QVariantList: {
                for (auto item : settings.value(s).toList()) {
                    qDebug() << QString(depth*4 , ' ') << originDepth << " item:" << item.toString() << " (" << typeid(item).name() << "," << item.typeId() << ") ";
                        qDebug() << QString(depth*4 , ' ') << originDepth << "   -> " << item.typeName();
                        break;
            }
            case QMetaType::QLocale: {
                qDebug() << QString(depth*4 , ' ') << originDepth << " item:" << settings.value(s).toLocale().name();
                break;
            }
            default:
                break;
            }
        }
    }
}

bool TestQSettingsJson::compareSettings(QSettings& set1, QSettings& set2, int depth)
{
    QStringList groupList1 = set1.childKeys();
    QStringList groupList2 = set2.childKeys();
    auto s2i = groupList2.begin();
    for (auto s1 : groupList2) {
        auto s2 = *s2i;
        if (s1 != s2) {
            return false;
        }
        set1.beginGroup(s1);
        set2.beginGroup(s2);
        if (!compareSettings(set1, set2, depth+1)) {
            logger(WARN, "compare failure\n");
            dumpSettings(set1, depth);
            dumpSettings(set2, depth);
            return false;
        }
        set2.endGroup();
        set1.endGroup();
        s2i++;
    }
    if (set1.childKeys().count() != set2.childKeys().count()) {
        return false;
    }

    auto childList1 = set1.childKeys();
    auto childList2 = set2.childKeys();
    auto s1ci = childList1.begin();
    auto s2ci = childList2.begin();
    while (s1ci != childList1.end()) {
        QString s1 = *s1ci;
        QString s2 = *s2ci;
        if (s1.compare(s2) != 0) {
            return false;
        }
        if (set1.value(s1).typeId() == QMetaType::QVariantList) {

            auto array1 = set1.value(s1).toStringList();
            auto array2 = set1.value(s2).toStringList();
            if (array1.size() != array2.size()) {
                return false;
            }
            for (auto i = 0; i < array1.size(); i++) {
                if (array1[i].compare(array2[i]) != 0) {
                    return false;
                }
            }
        } else {
            auto v1 = set1.value(s1).toString();
            auto v2 = set2.value(s1).toString();
            if (v1.compare(v2) != 0) {
                return false;
            }
        }
        s1ci++;
        s2ci++;
    }
    return true;
}

int TestQSettingsJson::logger(LogLevel level, const char *format, ...)
{
    if (level <= MSG) {
        return 0; 
    }

    va_list ap;
    va_start(ap, format);
    auto ret = ::vprintf(format, ap);
    va_end(ap);
    return ret;
}
