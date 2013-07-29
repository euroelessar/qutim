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

#include <qutim/jsonfile.h>
#include <QDebug>
#include "plistconfigbackend.h"
#include <QDomDocument>
#include <QFile>
#include <QTime>
#include <QTextCodec>
#include <QRect>
#include <QDataStream>
#include <qutim/debug.h>

namespace Core
{
	QDomElement variantToDomElement (const QVariant &v, QDomDocument &root)
	{
		QString result;
		QString type = QLatin1String("string");
		QDomElement element;
		switch (v.type()) {
			case QVariant::StringList:
			case QVariant::List:
			case QVariant::Map: {
				return element;
			}
			case QVariant::Invalid:
				result = QLatin1String("@Invalid()");
				break;

			case QVariant::ByteArray: {
				QByteArray a = v.toByteArray();
				result = a.toBase64();
				type = QLatin1String("data");
				break;
			}

			case QVariant::String: {
				result = v.toString();
				break;
			}
			case QVariant::LongLong:
			case QVariant::ULongLong:
			case QVariant::Int:
			case QVariant::UInt: {
				result = v.toString();
				type = QLatin1String("integer");
				break;
			}
			case QVariant::Bool: {
				element = root.createElement(v.toString());
				return element;
			}
			case QVariant::Double: {
				result = v.toString();
				type = QLatin1String("real");
				break;
			}
			case QVariant::KeySequence: {
				result = v.toString();
				if (result.startsWith(QLatin1Char('@')))
					result.prepend(QLatin1Char('@'));
				break;
			}
			case QVariant::Rect: {
				QRect r = qvariant_cast<QRect>(v);
				result += QLatin1String("@Rect(");
				result += QString::number(r.x());
				result += QLatin1Char(' ');
				result += QString::number(r.y());
				result += QLatin1Char(' ');
				result += QString::number(r.width());
				result += QLatin1Char(' ');
				result += QString::number(r.height());
				result += QLatin1Char(')');
				break;
			}
			case QVariant::Size: {
				QSize s = qvariant_cast<QSize>(v);
				result += QLatin1String("@Size(");
				result += QString::number(s.width());
				result += QLatin1Char(' ');
				result += QString::number(s.height());
				result += QLatin1Char(')');
				break;
			}
			case QVariant::Point: {
				QPoint p = qvariant_cast<QPoint>(v);
				result += QLatin1String("@Point(");
				result += QString::number(p.x());
				result += QLatin1Char(' ');
				result += QString::number(p.y());
				result += QLatin1Char(')');
				break;
			}

			default: {
				QByteArray a;
				{
					QDataStream s(&a, QIODevice::WriteOnly);
					s.setVersion(QDataStream::Qt_4_0);
					s << v;
				}

				result = QLatin1String("@Variant(");
				result += QString::fromLatin1(a.constData(), a.size());
				result += QLatin1Char(')');
				break;
			}
		}
		element = root.createElement(type);
		element.appendChild(root.createTextNode(result));
		return element;
	}

	QVariant PListConfigBackend::load(const QString &file)
	{
		QDomDocument plist;
		QFile input(file);
		if (input.open(QIODevice::ReadOnly) && plist.setContent(&input)) { //some black magic
			input.close();
			QDomElement root = plist.documentElement();
			return generateConfigEntry(root.firstChild());
		} else {
			input.close();
			return QVariant(QVariant::Invalid);
		}
	}
	
	void PListConfigBackend::save(const QString &file, const QVariant &entry)
	{
		QDomImplementation domImpl;
		QDomDocumentType type = domImpl.createDocumentType(QLatin1String("plist"),
															QLatin1String("-//Apple Computer//DTD PLIST 1.0//EN"),
															QLatin1String("http://www.apple.com/DTDs/PropertyList-1.0.dtd")
															);
		QDomDocument root(type);
		QDomElement plist = root.createElement("plist");
		plist.setAttribute("version","1.0");
		plist.appendChild(generateQDomElement(entry, root));
		root.appendChild(plist);
		QFile output (file);
		if (!output.open(QIODevice::WriteOnly)) {
			warning() << tr("Cannot write to file %1").arg(file);
			return;
		}
		QTextStream out(&output);
		static QTextCodec *utf8 = QTextCodec::codecForName("utf-8");
		out.setCodec(utf8);
		out << QLatin1String("<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"); //hack
		root.save(out, 2, QDomNode::EncodingFromDocument);
		output.close();
	}
	
	QVariant PListConfigBackend::generateConfigEntry(const QDomNode &val)
	{
		if (val.isNull())
			return QVariant();
		
		QDomElement element = val.toElement();
		if (element.nodeName()=="true") {
			return true;
		} else if (element.nodeName()=="false") {
			return false;
		} else if (element.nodeName()=="real") {
			return element.text().toDouble();
		} else if (element.nodeName()=="string") {
			return element.text();
		} else if (element.nodeName()=="integer") {
			return element.text().toInt();
		} else if (element.nodeName()=="data") {
			return QByteArray::fromBase64(element.text().toLatin1());
		} else if (element.nodeName()=="date") {
			//TODO
			return element.text();
		} else if (element.nodeName()=="dict") {
			QString key;
			QVariantMap map;
			for (QDomNode node = val.firstChild(); !node.isNull(); node = node.nextSibling()) {
				if (node.toElement().nodeName()=="key")
					key = node.toElement().text();
				else
					map.insert(key, generateConfigEntry(node));
			}
			return map;
		} else if (element.nodeName()=="array") {
			QVariantList list;
			for (QDomNode node = val.firstChild(); !node.isNull(); node = node.nextSibling()) {
				list.append(generateConfigEntry(node));
			}
			return list;
		}
		return QVariant(QVariant::Invalid);
	}
	
	QDomElement PListConfigBackend::generateQDomElement (const QVariant &entry, QDomDocument &root)
	{
		QDomElement element;
		if (entry.type() == QVariant::Map) {
			const QVariantMap map = entry.toMap();
			QVariantMap::const_iterator i;
			element = root.createElement("dict");
			for (i = map.constBegin(); i != map.constEnd(); ++i) {
				QDomElement key = root.createElement("key");
				key.appendChild(root.createTextNode(i.key()));
				element.appendChild(key);
				element.appendChild(generateQDomElement(i.value(), root));
			}
		} else if (entry.type() == QVariant::List) {
			element = root.createElement("array");
			const QVariantList list = entry.toList();
			for (int i = 0; i < list.size(); i++)
				element.appendChild(generateQDomElement(list.at(i), root));
		} else {
			element = variantToDomElement(entry, root);
		}
		return element;
	}
}

