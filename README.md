# QSettingJson

This is a Qt-related class that adds the ability to import and export json-formatted settings.

The QSettings class inherits from Qt's QSetting class and adds the following abilities:

1. A constructor that constructs a QSettingJson from a json text file:
    ```lang c++
    QSettingJson setting(QJsonObject * jobject);
    ```
2. A contructor that construct QSettingsJson from a QSettings object;
    ```lang cpp
    QSettingsJson(QSettings * settings);
    ```
3. A function that exports a standard json object 
    ```lang c++
    QJsonObject * exportJson();
    ```
4. A function that extracts a standard QSettings
    ```lang c++
    QSettings *extractSettings();
    ```
    (this functions can be be used rarlely, when QSettingJson cannot be used as-is)