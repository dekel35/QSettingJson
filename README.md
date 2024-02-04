# QSettingJson

Qt has a Json support using [QJson](https://doc.qt.io/qt-6/json.html), but the iteraction with Qt's [QSettings](https://doc.qt.io/qt-6/qsettings.html) is lacking. 

QSettingsJson is a Qt-related class that adds the ability to import and export json-formatted settings.

## Details

The QSettingsJson class inherits from Qt's QSetting class and adds the following abilities:

1. A **constructor** that constructs a QSettingJson from a json object:
    ```lang c++
    QSettingJson setting(QJsonObject& jobject);
    ```
2. A **constructor** that construct QSettingsJson from a QSettings object:
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

QSetting and json are not fully interchangeable. QSettingsJson does what it can in order to bridge some of the gaps. At the time of this writing, QSettings supports 82 different values which range not only from numbers, strings and arrays. but also types like colors, drawing pens, polygons, etc.

On the other hand the json specification contains only 7 possible values:
1. Number 
2. String 
3. Boolean 
4. Array 
5. Object 
6. Whitespace
7. Null

Therefore converting QSetting into json requires the Qt type to be preserved, for a later restoration. 
QSettingsJson can generate a json construct that preserves such types. 

for example QColor("#00ff00") can be converted to a json element like this:
```lang=json
{ "Qmetetype-QColor" : "00ff00" }
```
In order to achieve such conversion a callback function must be created. Here is an sample callback function for the QColor type. The function receives a setting [QVariant](https://doc.qt.io/qt-6/qvariant.html) which it should convert into a json-compatible value:
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

Note that even so, there are still styiling variations that should be considered. Using the example above, the same color can be converted to Json in at least 2 different ways:
1. 
``` lang=json
{ 
    QMetatype-QColor : "00ff00" 
}
```
2. 
``` lang=json
{ 
    QMetatype-QColor : {
        red: 0,
        green: 255,
        blue: 0
    } 
}
```

and they both represent the same QColor value. So, it's up to the writers of the callback functions to decide how they wish to handle this. 

The testQsettingJson.cpp file contains 2 samples that demonstrate possible QColor and QLocale implementations.

the QColor test does the following:
1. Creates an empty set:
```lang=c++
QSettingsJson::clearMetadata();
    QSettingsJson set;
    set.clear();
```
2. Defines the callbacks QSettingJson class for **QColor** for both ways conversion
```lang=c++
set.addQMetaType("QColor", MyQColorConvertor, MyQColorVariant);
```
3. sets a test QColor value in the settings object:
```lang=c++
set.setValue("color", QColor(0, 255, 0));
```
After this any conversion back and forth will call **MyQColorConvertor** and **MyQColorVariant** for doing a correct conversion.

testQLocale function in that file contains similar example for QLocal manipulation.

Feb, 2024
