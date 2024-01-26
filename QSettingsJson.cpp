#include <type_traits>
#include <iostream>
#include <QJsonArray>
#include <QSettingsJson.h>
#include <QJsonDocument>
#include <QMap>
#include <utility>

void dumpSettings(QSettings & settings, int depth = 0, int originDepth=0);
#define COUT std::cout << __func__ << ":" <<  __LINE__ << " "

std::map<QString, std::pair<JsonFuncPtr, VariantFuncPtr> > QSettingsJson::_extendedMap;

QSettingsJson::QSettingsJson(QJsonObject *jObject)
{
    clear();
    importSettingsFromJson(*jObject, *this, 0);
}

QSettingsJson::QSettingsJson(QSettings *settings)
{
    for (auto key : settings->allKeys()) {
        this->setValue(key, settings->value(key));
    }
}

QSettingsJson::~QSettingsJson()
{
}

QSettings *QSettingsJson::extractSettings()
{
    QSettings *ret = new QSettings();
    for (auto key : allKeys()) {
        ret->setValue(key, value(key));
    }
    return ret;
}

QJsonObject *QSettingsJson::exportJson()
{
    QJsonObject exported = exportOneKey("", 0);
    QJsonObject *ret = new QJsonObject(exported);
    return ret;
}

void QSettingsJson::addQMetaType(QString metaType, JsonFuncPtr func, VariantFuncPtr variantFunc)
{
    _extendedMap[metaType] = qMakePair(func, variantFunc);
    if (!(metaType.startsWith("QMetatype"))) {
        _extendedMap[QString("QMetatype-") + metaType] = qMakePair(func, variantFunc);
    }
    assert (QSettingsJson::_extendedMap.find(metaType) != QSettingsJson::_extendedMap.end());
}

void QSettingsJson::delQMetaType(QString metaType)
{
    _extendedMap.erase(metaType);
    if (!(metaType.startsWith("QMetatype"))) {
        _extendedMap.erase(QString("QMetatype-") + metaType);
    }
}

void QSettingsJson::delAllQMetaType()
{
    _extendedMap.clear();
}

void QSettingsJson::importSettingsFromJson(QJsonObject &jObject,
                                           QSettingsJson &retSettings, int depth)
{
    for (auto jkey : jObject.keys()) {
        if (jObject[jkey].isObject()) {
            QJsonObject subObj = jObject[jkey].toObject();
            if (jkey.startsWith("QMetatype")) { //TODO: never happens??
                QVariant var = convertMetadata(jkey, subObj);
                retSettings.setValue(jkey, var);
            } else {
                retSettings.beginGroup(jkey);
                importSettingsFromJson(subObj, retSettings, depth+1);
                endGroup();
            }
            continue;
        }
        auto subIter = jObject.find(jkey);
        QString value;
        auto val = subIter.value();

        if (val.isString()) {
            if (isMeta(val.toString())) {
                QJsonParseError e;
                QJsonObject metaObject = QJsonDocument::fromJson(val.toString().toLatin1(), &e).object();
                QVariant var = convertMetadata(jkey, metaObject);
                retSettings.setValue(jkey, var);
            } else {
                retSettings.setValue(jkey, val.toString());
            }
        } else if (val.isBool()) {
            retSettings.setValue(jkey, val.toBool());
        } else if (val.isDouble()) {
            retSettings.setValue(jkey, val.toDouble());
        } else if (val.isNull()) {
            retSettings.setValue(jkey, QVariant::fromValue(nullptr));
        } else if (val.isArray()) {
            auto array = val.toArray();
            QList<QVariant> list;
            for (auto s : array) {
                list.push_back(s.toString().toLatin1());
            }
            retSettings.setValue(jkey, list);
        } else if (val.isObject()) {
            abort(); // can't happen
        } else { 
            retSettings.setValue(jkey, val.toString());
        }
    }
    return;
}

QJsonObject QSettingsJson::exportOneKey(QString key, int depth)
{
    QJsonObject retObject;

    /* extract all groups recursively */
    auto groups = childGroups();
    for (auto s : groups) {
        beginGroup(s);
        auto prefix = key.length() == 0 ? "" : key + "/";
        auto varObject = exportOneKey(s, depth + 1);
        retObject[s] = varObject;
        endGroup();
    }

    /* now handle the key-value setting objects */
    auto keys = childKeys();
    for (auto s : childKeys()) {
        switch(value(s).typeId()) {
        case  QMetaType::QVariantList: {
            QJsonArray array;
            for (auto item : value(s).toList()) {
                array.push_back(item.toString().toStdString().c_str());
            }
            retObject[s] = array;
            break;
        }
        case QMetaType::Bool:
            retObject[s] = value(s).toBool();
            break;
        case QMetaType::Int:
            retObject[s] = value(s).toInt();
            break;
        case QMetaType::Double:
            retObject[s] = value(s).toDouble();
            break;
        case QMetaType::Nullptr:
            retObject[s] = QJsonValue::Null;
            break;
        /* if we reached this point, it means that the value can not be converted to a QVariant in a 1:1 manner,
           so, we call handleQVariant which will look for a callback convertor (if one was defined) */
        default:
            QString subObject = handleQVariant(s, key, retObject);
            retObject[s] = subObject;//QJsonDocument::fromJson(inner.toString().toLatin1().data()).object();
            break;
        }
    }
    return retObject;
}

bool QSettingsJson::isMeta(QString str)
{
    auto s = str.remove(' ');
    if ((s[0] == '{') && (s[1] == '\"') && s.indexOf("QMetatype-") == 2) return true;
    else return false;
}

/* convert a Qt key-value pair into a json object possibly using a callback function */
QString QSettingsJson::handleQVariant(const QString &metaKey, QString &key1, QJsonObject &retObject1)
{
    QString retObject{};
    const QVariant &variant = value(metaKey);
    QString typeName = variant.metaType().name();
     if (_extendedMap[typeName].first != nullptr) {
            auto jobj1 = _extendedMap[typeName].first(variant);
            retObject = QString("{ \"QMetatype-") + typeName +  "\" :" + jobj1 + "}" ;
            return retObject;
    }
    else { /* no callback defined, do the best we can */
        retObject = variant.toString();
    }
    return retObject;
}

/* convert a json key-value into a Qt type 
   e.g., {"QMetatype-QColor" : "#00ff00" } into QColor("#00ff00") */
QVariant QSettingsJson::convertMetadata(QString key, QJsonObject metaObject) {

    /* get the first (and only) key of the json object */
    auto metaKey = metaObject.begin().key();

    /* get the first (and only) value of th json Object */
    QJsonObject subObj = metaObject.begin().value().toObject();


    /* construct a QString value for the callback function */
    auto val = metaObject.begin().value();
    QString value{};
    switch (val.type()) {
        case QJsonValue::String :
            value =  val.toString();
            break;
        case QJsonValue::Double:
            value = QString(std::to_string(val.toDouble()).c_str());
            break;
        case QJsonValue::Object:
            value = QString(QJsonDocument(val.toObject()).toJson());
            break;
        default:
            value = val.toString();
    }

    /* locate function reference and call it, if found  */
    std::map<QString, std::pair<JsonFuncPtr, VariantFuncPtr> >::iterator qp = QSettingsJson::_extendedMap.find(metaKey);
    if (qp != QSettingsJson::_extendedMap.end() && &qp->second.second != nullptr) {
            return qp->second.second(subObj, value);
    }
    
    /* no function was defined for this object key return plain value */
    return value;
}
