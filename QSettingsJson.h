#ifndef QSETTINGJS_H
#define QSETTINGJS_H

#include <algorithm>
#include <memory>

#include <QSettings>
#include <QJsonObject>

class QSettingsJson;
// typedef std::shared_ptr<QSettingsJson> SettingsPtr;
typedef QString (*JsonFuncPtr)(const QVariant& value);
typedef QVariant (*VariantFuncPtr) (const QJsonObject &jobj, QString value);

class QSettingsJson : public QSettings {
public:
    // QSettings contructors implementation
    QSettingsJson(QSettings::Scope scope, QObject *parent = nullptr)
        : QSettings(scope, parent)
    {
    }
    QSettingsJson(QObject *parent = nullptr) : QSettings(parent)
    {
    }
    QSettingsJson(const QString &fileName, QSettings::Format format,
                  QObject *parent = nullptr)
        : QSettings(fileName, format, parent)
    {
    }
    QSettingsJson(QSettings::Format format, QSettings::Scope scope,
                  const QString &organization,
                  const QString &application = QString(),
                  QObject *parent = nullptr)
        : QSettings(format, scope, organization, application, parent)
    {
    }
    QSettingsJson(QSettings::Scope scope, const QString &organization,
                  const QString &application = QString(),
                  QObject *parent = nullptr)
        : QSettings(scope, organization, application, parent)
    {
    }

    static void clearMetadata()
    {
        _extendedMap.clear();
    }

    // Additional json-related contructors
    QSettingsJson(QJsonObject& jobject);
    QSettingsJson(QSettings& settings);

    virtual ~QSettingsJson();

    // export current settings to a json object
    QJsonObject *exportJson();
    // extract this class into Qt QSetting object
    QSettings *extractSettings();

    /* define callback for a given QMetatype name 
       parameters:
       1. Metatype - metatype name (e.g., "QColor")
       2, jsonFunc - a function receiving a QVariaant repressing a value and returning a string as value
       3. variantFunc - a function receiving a json value and returninh QVariant object holding the Qt value

       This function is global and the list of metatype will be reserved accrross object creations. 
    */
    static void addQMetaType(QString metaType, JsonFuncPtr jsonFunc = nullptr, VariantFuncPtr variantFunc = nullptr);

    /* delete one metatype definition addded by addQMetaType */
    static void delQMetaType(QString metaType);

    /*clear all meta types */
    static void delAllQMetaType();

private:
    void importSettingsFromJson(QJsonObject &jObject, QSettingsJson &retSettings, int depth);
    QJsonObject exportOneKey(QString key, int depth);
    QString handleQVariant(const QString &metaKey, QString &key, QJsonObject &retObject);
    QVariant convertMetadata(QString key, QJsonObject subObj);
    bool isMeta(QString str);

    static std::map<QString, std::pair<JsonFuncPtr, VariantFuncPtr> > _extendedMap;
};
//TODO: check nulls, etc, especially in callbacks
//TODO: casing etc.
//references insted of objects as parameters
#endif // QSETTINGSJ_H
