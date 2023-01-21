#include <type_traits>
#include <iostream>
#include <QJsonArray>
#include <QSettingsJson.h>

QSettingsJson::QSettingsJson(QJsonObject *jObject)
{
    SettingsPtr tmp = std::make_shared<QSettingsJson>();
    tmp->clear();
    importSettingsFromJson(*jObject, tmp);
    for (auto key : (*tmp).allKeys()) {
        this->setValue(key, tmp->value(key));
    }
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

void QSettingsJson::importSettingsFromJson(QJsonObject &jObject,
                                           SettingsPtr &retSettings)
{
    for (auto jkey : jObject.keys()) {
        if (jObject[jkey].isObject()) {
            retSettings->beginGroup(jkey);
            auto subObj = jObject[jkey].toObject();
            importSettingsFromJson(subObj, retSettings);
            retSettings->endGroup();
            continue;
        }
        auto subIter = jObject.find(jkey);
        QString value;
        auto val = subIter.value();
        if (val.isString()) {
            retSettings->setValue(jkey, subIter.value().toString());
        } else if (val.isBool()) {
            retSettings->setValue(jkey, subIter.value().toBool());
        } else if (val.isDouble()) {
            retSettings->setValue(jkey, subIter.value().toDouble());
        } else if (val.isNull()) {
            retSettings->setValue(jkey, QVariant::fromValue(nullptr));
        } else if (val.isArray()) {
            auto array = subIter.value().toArray();
            QList<QVariant> list;
            for (auto s : array) {
                list.push_back(s.toString().toLatin1());
            }
            retSettings->setValue(jkey, list);
        } else if (val.isObject()) {
            abort(); // can't happen
        } else { // catchall
            retSettings->setValue(jkey, subIter.value().toString());
        }
    }
}

QJsonObject QSettingsJson::exportOneKey(QString key, int depth)
{
    QJsonObject retObject;
    QString prevGroup = "";
    auto groups = childGroups();
    for (auto s : groups) {
        beginGroup(s);
        auto prefix = key.length() == 0 ? "" : key + "/";
        auto jGroup = exportOneKey(prefix + s, depth + 1);
        endGroup();
        retObject[s] = jGroup;
    }
    auto keys = childKeys();
    for (auto s : childKeys()) {
        if (value(s).typeId() == QMetaType::QVariantList) {
            QJsonArray array;
            for (auto item :value(s).toList()) {
                array.push_back(item.toString().toStdString().c_str());
            }
            retObject[s] = array;
        } else if (value(s).typeId() == QMetaType::Bool) {
            retObject[s] = value(s).toBool();
        } else if (value(s).typeId() == QMetaType::Int) {
            retObject[s] = value(s).toInt();
        } else if (value(s).typeId() == QMetaType::Double) {
            retObject[s] = value(s).toDouble();
        } else if (value(s).typeId() == QMetaType::Nullptr) {
            retObject[s] = QJsonValue::Null;
        } else {
            retObject[s] = value(s).toString();
        }
    }
    return retObject;
}
