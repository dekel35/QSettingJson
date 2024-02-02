# QSettingJson

Qt has a Json support using [QJson](https://doc.qt.io/qt-6/json.html), but the iteraction with Qt\s [QSettings](https://doc.qt.io/qt-6/qsettings.html) is not natual. 

QSettingsJson is a Qt-related class that adds the ability to import and export json-formatted settings.

## Details

The QSettingsJson class inherits from Qt's QSetting class and adds the following abilities:

1. A **constructor** that constructs a QSettingJson from a json object:
    ```lang c++
    QSettingJson setting(QJsonObject& jobject);
    ```
2. A **contructor** that construct QSettingsJson from a QSettings object:
    ```lang cpp
    QSettingsJson(QSettings& settings);
    ```
3. A **function** that exports a standard json object:
    ```lang c++
    QJsonObject * exportJson();
    ```
4. A **function** that extracts a standard QSettings (this function can be be used in rare cases, when QSettingJson cannot be used as-is):
    ```lang c++
    QSettings *extractSettings();
    ```
    
As a general rule QSettingJson can be used whenever QSettings is to be used, but with the extra ability to import/export a json-formatted string.

# A note about compatibility between json and QSettings

QSetting and json are not fully interchangable. QSettingsJson does what it can in order to bridge gaps.At the time of this writing, QSettings supports 82 different values which range not only from numbers, strings and arrays. but also types like colors, drawing pens, poligons, etc.

On the other hand the json specification cntains only 7 possible values:
1. Number 
2. String 
3. Boolean 
4. Array 
5. Object 
6. Whitespace
7. Null

Therefore converting QSetting into json requires the Qt type to be preserved, for later restore. 
QSettingsJson can generate a json construct that preserves such types. 

for axample QColor("#00ff00") **can** be converted to a json element like this:
```lang=json
{ "Qmetetype-QColor" : "00ff00" }
```
In order to achieve such convertion a callback function must be created. Here is an sample callback function for the QColor type. The function receives a setting [QVariant](https://doc.qt.io/qt-6/qvariant.html) which it shoud convert into a json-compatible value:
```lang=c++
String MyQColorConvertor(const QVariant& value)
{
    if (value.canConvert<QColor>()) {
        QColor embeddedType = value.value<QColor>();
        QString ba (
            QString("\"") +
            value.toString() +
            QString("\"")
            );
        return QString(ba);
    }
    else return nullptr;
}
```

... and the function that converts the above json object back to a Qt type, looks like this:
```lang=c++
QVariant MyQColorVariant(const QJsonObject &color, QString value)
{
    QColor qcol(value);
    return QVariant(qcol);
}
```
the parameters to this callback function are
1. a json object containing the meta type constract (e.g., { "Qmetetype-QColor" : "00ff00" })
1. the extracted string with the value (*"00ff00"*, in this case)

