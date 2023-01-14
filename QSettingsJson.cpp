#include <type_traits>
#include <iostream>
#include <QSettingsJson.h>


QSettingsJson::QSettingsJson(QJsonObject* jObject) 
{
  SettingsPtr tmp = std::make_shared<QSettingsJson>();
  tmp->clear();
  importSettingsFromJson(*jObject, tmp);
  for (auto key : (*tmp).allKeys()) {
    this->setValue(key, tmp->value(key));
  }
}

QSettingsJson::QSettingsJson(QSettings * settings)
{
  for (auto key : settings->allKeys()) {
    this->setValue(key, settings->value(key));
  }
}

QSettingsJson::~QSettingsJson() 
{

}

QSettings * QSettingsJson::extractSettings()
{
  QSettings *ret = new QSettings();
  for (auto key : allKeys()) {
    ret->setValue(key, value(key));
  }
  return ret;
}

QJsonObject * QSettingsJson::exportJson()
{
    QJsonObject exported = exportOneKey("", 0);
    QJsonObject * ret = new QJsonObject(exported);
    return ret;
}

void QSettingsJson::importSettingsFromJson(QJsonObject& jObject, SettingsPtr& retSettings)
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
    if (subIter.value().isString()) {
      value = subIter.value().toString();
    } else if (subIter.value().isBool()) {
      value = subIter.value().toBool() ? "true" : "false";
    } else {
        abort();
    }
    retSettings->setValue(jkey, value);
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
        auto jGroup = exportOneKey(prefix + s, depth+1);
        endGroup();
        retObject[s] = jGroup;
    }
    auto keys = childKeys();
    for (auto s : childKeys()) {
        retObject[s] = value(s).toString();
    }
    return retObject;
}
