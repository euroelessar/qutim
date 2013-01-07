/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2011 Ruslan Nigmatullin <euroelessar@yandex.ru>
**
*****************************************************************************
**
** $QUTIM_BEGIN_LICENSE$
** This program is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation, either version 3 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
** See the GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program.  If not, see http://www.gnu.org/licenses/.
** $QUTIM_END_LICENSE$
**
****************************************************************************/

#ifndef JSONFILE_H
#define JSONFILE_H

#include "libqutim_global.h"

namespace qutim_sdk_0_3
{
class JsonFilePrivate;

/**
* @brief JsonFile provides high-level api for fast writing and reading files with JSON content.
*
* @code
JsonFile file("test.json");
QVariantMap map;
map.insert("list", QVariantList() << QVariant(1) << QVariant("2"));
map.insert("string", "Hello world!");
file.write(var);
@endcode
* File will contain something like:
* @verbatim
{
 "string": "Hello world!",
 "list": [
  1,
  "2"
 ]
}@endverbatim
* To read file conents back to QVariant just do:
* @code
QVariant var;
JsonFile file("test.json");
file.read(var);
@endcode
* @note JsonFile is wrapper around methods, provided in Json namespace
*/
class LIBQUTIM_EXPORT JsonFile
{
    Q_DISABLE_COPY(JsonFile)
    Q_DECLARE_PRIVATE(JsonFile)
public:
#ifndef Q_QDOC
    enum OpenMode { ReadOnly, ReadWrite };
#endif
    /**
    * @brief Constructor
    *
    * @param name File name
    */
    JsonFile(const QString &name = QString());
    /**
    * Destructor
    */
    virtual ~JsonFile();
    /**
    * @brief Set file name to @a name
    */
    void setFileName(const QString &name);
    /**
    * @brief Load file's json data to @a variant
    *
    * @return @b True if file was successfully parsed, else @b false
    */
    bool load(QVariant &variant);
    /**
    * @brief Save content of @a variant as json to file
    *
    * @return @b True if file was successfully parsed, else @b false
    */
    bool save(const QVariant &variant);
#ifndef Q_QDOC
private:
    QScopedPointer<JsonFilePrivate> d_ptr;
#endif
};
}

#endif // JSONFILE_H

