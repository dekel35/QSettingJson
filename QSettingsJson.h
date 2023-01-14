#ifndef QSETTINGJS_H
#define QSETTINGJS_H

#include <algorithm>
#include <memory>

#include <QSettings>
#include <QJsonObject>

class QSettingsJson;
typedef std::shared_ptr<QSettingsJson> SettingsPtr;

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

    // Additional json-related contructors
    QSettingsJson(QJsonObject *jobject);
    QSettingsJson(QSettings *settings);

    virtual ~QSettingsJson();

    // export current settings to a json object
    QJsonObject *exportJson();
    // extract this class into Qt QSetting object
    QSettings *extractSettings();

private:
    void importSettingsFromJson(QJsonObject &jObject, SettingsPtr &retSettings);
    QJsonObject exportOneKey(QString key, int depth);
};
#endif // QSETTINGSJ_H
