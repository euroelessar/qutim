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

#include "qipinfium.h"
#include <QDir>
#include <QTextDocument>
#include <QCoreApplication>
#include <qutim/icon.h>
#include <qutim/debug.h>

using namespace qutim_sdk_0_3;

namespace HistoryManager {

qipinfium::qipinfium()
{
}

qipinfium::~qipinfium()
{
}

quint8 qipinfium::getUInt8(const uchar * &data)
{
	return *(data++);
}

quint16 qipinfium::getUInt16(const uchar * &data)
{
	quint16 a = (*(data++) << 8);
	a += *(data++);
	return a;
}

quint32 qipinfium::getUInt32(const uchar * &data)
{
	quint32 a = (*(data++) << 24);
	a += (*(data++) << 16);
	a += (*(data++) << 8);
	a += *(data++);
	return a;
}

QString qipinfium::getString(const uchar * &data, int len, bool crypt)
{
	QByteArray str((const char *)data, len);
	if(crypt)
	{
		for(int i = 0; i < len; i++)
		{
			str[i] = str[i] + i + 1;
			str[i] = 255 - str[i];
		}
	}
	data += len;
	return QString::fromUtf8(str.constData()/*, len*/); // <censored> qip can write zero bytes to end of file
}

void qipinfium::loadMessages(const QString &path)
{
	QDir dir = path;
	if(!dir.cd("History"))
		return;
	QHash<QString,QString> protocols;
	protocols[QLatin1String("icq")]    = QLatin1String("ICQ");
	protocols[QLatin1String("jabber")] = QLatin1String("Jabber");
	protocols[QLatin1String("mra")]    = QLatin1String("MRIM");
	QStringList filters;
	foreach(QString format,protocols.keys())
		filters << (format + QLatin1String("*.qhf")) << (format + QLatin1String("*.ahf"));
	QFileInfoList files = dir.entryInfoList(filters, QDir::Files);
	setMaxValue(files.size());
	for(int i = 0; i < files.size(); i++)
	{
		setValue(i + 1);
		QString protocol = files[i].fileName().section("_",0,0);
		while(!protocol.isEmpty() && protocol.at(protocol.length() - 1).isDigit())
			protocol.chop(1);
		protocol = protocols[protocol.toLower()];
		if(protocol.isEmpty())
		{
			warning() << "Unknown protocol: " << files[i].fileName();
			continue;
		}
		setProtocol(protocol);
		setAccount(m_accounts.value(protocol));
		QFile file(files[i].absoluteFilePath());
		if(file.open(QFile::ReadOnly))
		{
			QByteArray bytearray = file.readAll();
			const uchar *data = (const uchar *)bytearray.constData();
			const uchar *end = data + bytearray.size();
			if(memcmp(data, "QHF", 3))
				continue;
			uchar version = data[3];
			data += 44;
			QString uin = getString(data, getUInt16(data));
			QString name = getString(data, getUInt16(data));
			Q_UNUSED(name);
			QDateTime time;
			setContact(uin);
			while(data < end)
			{
				quint16 type = getUInt16(data);
				quint32 index = getUInt32(data);
				data += 2;
				if(type == 1)
				{
					Message message;
					data += 10;
					time = QDateTime::fromTime_t(getUInt32(data));
					time.setTimeSpec(Qt::LocalTime);
					if(version == 1)
						time = time.toUTC().addDays(2);
					else
						time = time.toUTC();
					message.setTime(time);
					data += 4;
					message.setIncoming(getUInt8(data) == 0);
					data += 4;
					int mes_len = version == 3 ? getUInt32(data) : getUInt16(data);
					message.setText(getString(data, mes_len, version > 1));
					appendMessage(message);
				}
				else
					data += index;
			}
		}
	}
}

bool qipinfium::validate(const QString &path)
{
	QDir dir = path;
	if(!dir.cd("History"))
		return false;
	QStringList files = dir.entryList(QStringList() << "*.qhf" << "*.ahf", QDir::Files);
	return !files.isEmpty();
}

QString qipinfium::name()
{
	return "QIP Infium";
}

QIcon qipinfium::icon()
{
	return Icon("qip-infium");
}

QList<ConfigWidget> qipinfium::config()
{
	return m_config_list = QList<ConfigWidget>()
			<< createAccountWidget("ICQ")
			<< createAccountWidget("Jabber")
			<< createAccountWidget("MRIM");
}

bool qipinfium::useConfig()
{
	m_accounts.insert("ICQ", m_config_list[0].second->property("currentText").toString());
	m_accounts.insert("Jabber", m_config_list[1].second->property("currentText").toString());
	m_accounts.insert("MRIM", m_config_list[2].second->property("currentText").toString());
	return true;
}

QString qipinfium::additionalInfo()
{
	return QCoreApplication::translate("ClientConfigPage", "Select accounts for each protocol in the list.");
}

}

