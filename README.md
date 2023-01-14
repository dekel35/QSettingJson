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

QSetting and json are not 100% interchangable. QSettingsJson does what it can in order to bridge gaps, but it will never be able to do a perfect conversion for some types:

consider, for example the following json:

```json
{
    "key" : ["1", "2"]
}
```
The value ( ["1", "2"] ) is a json array. Something QSettings does not have a parallel for. In low-level terms it means that there is no parallel QVariant. Therefore for such json file, when converted to Settings, the array value ["1", "2'] will be transformed to the string "[1, 2]".

transformint it back to json will produce the following file:
```json
{
    "key" : "[1, 2]"
}
```
Which is not what we would have liked, but that is because of the incompatibilities between the formats.