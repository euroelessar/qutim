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

#include "qippda.h"
#include "qipinfium.h"
#include <QDir>
#include <QFileInfo>
#include <QTextDocument>
#include <qutim/icon.h>
#include <qutim/debug.h>

using namespace qutim_sdk_0_3;

namespace HistoryManager {

qippda::qippda()
{
}

qippda::~qippda()
{
}

void qippda::loadMessages(const QString &path)
{
	QDir dir = path;
	if(!dir.cd("History"))
		return;
	QHash<QString,QString> protocols;
	protocols[QLatin1String("icq")]    = QLatin1String("icq");
	protocols[QLatin1String("jabber")] = QLatin1String("jabber");
	protocols[QLatin1String("mra")]    = QLatin1String("mrim");
	QStringList filters = QStringList() << QLatin1String("*.qhf") << QLatin1String("*.ahf");
	QFileInfoList files = dir.entryInfoList(filters, QDir::Files);
	setMaxValue(files.size());
	for(int i = 0; i < files.size(); i++)
	{
		setValue(i + 1);
		QString protocol = files[i].fileName().section("_",0,0);
		while(!protocol.isEmpty() && protocol.at(protocol.length() - 1).isDigit())
			protocol.chop(1);
		// Old qip pda has only ICQ support and files are named like 1_myuin_hisuin
		if(protocol.isEmpty())
			protocol = QLatin1String("icq");
		else
		{
			protocol = protocols[protocol.toLower()];
			if(protocol.isEmpty())
			{
				qWarning() << "Unknown protocol:" << files[i].fileName();
				continue;
			}
		}
		setProtocol(protocol);
		QString account = files[i].fileName().section("_",1,1);
		setAccount(account);
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
			QString uin = qipinfium::getString(data, qipinfium::getUInt16(data));
			QString name = qipinfium::getString(data, qipinfium::getUInt16(data));
			Q_UNUSED(name);
			setContact(uin);
//			continue;
			while(data < end)
			{
				quint16 type = qipinfium::getUInt16(data);
				quint32 index = qipinfium::getUInt32(data);
				data += 2;
				const uchar *next = data + index;
				if(type == 1)
				{
					Message message;
					data += 10;
					QDateTime time = QDateTime::fromTime_t(qipinfium::getUInt32(data));
					time.setTimeSpec(Qt::LocalTime);
					if(version == 1)
						time = time.toUTC().addDays(2);
					else
						time = time.toUTC();
					message.setTime(time);
					data += 4;
					message.setIncoming(qipinfium::getUInt8(data) == 0);
					data += 4;
					int mes_len = version == 3 ? qipinfium::getUInt32(data) : qipinfium::getUInt16(data);
					message.setText(qipinfium::getString(data, mes_len, version > 1));
					appendMessage(message);
				}
				else
					data = next;
			}
		}
	}
}

bool qippda::validate(const QString &path)
{
	QDir dir = path;
	if(!dir.cd("History"))
		return false;
	QStringList files = dir.entryList(QStringList() << "*.qhf" << "*.ahf", QDir::Files);
	return !files.isEmpty();
}

QString qippda::name()
{
	return "QIP PDA";
}

QIcon qippda::icon()
{
	return Icon("qip-pda");
}

}

