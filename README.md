# QSettingJson

QSettingsJson is a Qt-related class that adds the ability to import and export json-formatted settings.

## Details

The QSettingsJson class inherits from Qt's QSetting class and adds the following abilities:

1. A **constructor** that constructs a QSettingJson from a json text file:
    ```lang c++
    QSettingJson setting(QJsonObject * jobject);
    ```
2. A **contructor** that construct QSettingsJson from a QSettings object:
    ```lang cpp
    QSettingsJson(QSettings * settings);
    ```
3. A **function** that exports a standard json object:
    ```lang c++
    QJsonObject * exportJson();
    ```
4. A **function** that extracts a standard QSettings (this function can be be used in rare cases, when QSettingJson cannot be used as-is):
    ```lang c++
    QSettings *extractSettings();
    ```
    
As a general rule QSettingJson should be used whenever QSettings is to be used, but with the extra ability to import/export a json-formatted string.

# Note:

QSetting and json are not 100% interchangable. QSettingsJson does what it can in order to bridge gaps.At the time of this writing, QSettings supports 82 different values which range not only from numbers, strings and arrays. but also types like colors, drawing pens, poligons, etc.

On the other hand the json specification cntains only 7 possible values:
1. Number 
2. String 
3. Boolean 
4. Array 
5. Object 
6. Whitespace
7. Null

Therefor converting QSetting with some exotic values to json may produce undesirable results.