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

#include "jsonfile.h"
#include "json.h"
#include <QFile>
#include <QTextStream>
#include <QStringList>
#include <QDebug>
#include <QFileInfo>
#include <QDir>
#include "debug.h"

namespace qutim_sdk_0_3
{
class JsonFilePrivate
{
public:
    QFile file;
};

JsonFile::JsonFile(const QString &name) : d_ptr(new JsonFilePrivate)
{
    d_func()->file.setFileName(name);
}

JsonFile::~JsonFile()
{
}

void JsonFile::setFileName(const QString &name)
{
    d_func()->file.setFileName(name);
}

bool JsonFile::load(QVariant &variant)
{
    Q_D(JsonFile);
    variant.clear();
    if (!d->file.open(QIODevice::ReadOnly | QIODevice::Text))
        return false;
    int len = d->file.size();
    QByteArray array;
    const uchar *fmap = d->file.map(0, d->file.size());
    if(!fmap)
    {
        array = d->file.readAll();
        fmap = (uchar *)array.constData();
    }
    const uchar *s = Json::skipBlanks(fmap, &len);
    bool result = (Json::parseRecord(variant, s, &len) != 0);
    if (!result)
        qDebug() << "Parse error of json file " << d->file.fileName();
    d->file.close();
    return result;
}

bool json_file_generate (QTextStream &res, const QVariant &val, int indent) {
    switch (val.type()) {
    case QVariant::Invalid: res << "null"; break;
    case QVariant::Bool: res << (val.toBool() ? "true" : "false"); break;
    case QVariant::Char: res << Json::quote(QString(val.toChar())).toUtf8(); break;
    case QVariant::Double: res << QString::number(val.toDouble()).toLatin1(); break; //CHECKME: is '.' always '.'?
    case QVariant::Int: res << QString::number(val.toInt()).toLatin1(); break;
    case QVariant::String: res << Json::quote(val.toString()).toUtf8(); break;
    case QVariant::UInt: res << QString::number(val.toUInt()).toLatin1(); break;
    case QVariant::ULongLong: res << QString::number(val.toULongLong()).toLatin1(); break;
    case QVariant::Map: {
        //for (int c = indent; c > 0; c--) res << ' ';
        res << "{";
        indent++; bool comma = false;
        QVariantMap m(val.toMap());
        QVariantMap::const_iterator i;
        for (i = m.constBegin(); i != m.constEnd(); ++i) {
            if (comma) res << ",\n"; else { res << '\n'; comma = true; }
            for (int c = indent; c > 0; c--) res << ' ';
            res << Json::quote(i.key()).toUtf8();
            res << ": ";
            if (!json_file_generate(res, i.value(), indent)) return false;
        }
        indent--;
        if (comma) {
            res << '\n';
            for (int c = indent; c > 0; c--) res << ' ';
        }
        res << '}';
        indent--;
    } break;
    case QVariant::List: {
        //for (int c = indent; c > 0; c--) res << ' ';
        res << "[";
        indent++; bool comma = false;
        QVariantList m(val.toList());
        foreach (const QVariant &v, m) {
            if (comma) res << ",\n"; else { res << '\n'; comma = true; }
            for (int c = indent; c > 0; c--) res << ' ';
            if (!json_file_generate(res, v, indent)) return false;
        }
        indent--;
        if (comma) {
            res << '\n';
            for (int c = indent; c > 0; c--) res << ' ';
        }
        res << ']';
        indent--;
    } break;
    case QVariant::StringList: {
        //for (int c = indent; c > 0; c--) res << ' ';
        res << "[";
        indent++; bool comma = false;
        QStringList m(val.toStringList());
        foreach (const QString &v, m) {
            if (comma) res << ",\n"; else { res << '\n'; comma = true; }
            for (int c = indent; c > 0; c--) res << ' ';
            res << Json::quote(v).toUtf8();
        }
        indent--;
        if (comma) {
            res << '\n';
            for (int c = indent; c > 0; c--) res << ' ';
        }
        res << ']';
        indent--;
    } break;
    default: return false;
    }
    return true;
}

bool JsonFile::save(const QVariant &variant)
{
    Q_D(JsonFile);
    {
        QDir dir = QFileInfo(d->file).absoluteDir();
        if(!dir.exists())
            dir.mkpath(dir.absolutePath());
    }
    if (!d->file.open(QIODevice::WriteOnly | QIODevice::Text))
        return false;
    QTextStream stream(&d->file);
    stream.setCodec("utf-8");
    bool result = json_file_generate(stream, variant, 0);
    stream.flush();
    d->file.close();
    return result;
}
}

