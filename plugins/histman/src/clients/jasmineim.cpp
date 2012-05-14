/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2012 DeKaN (DeKaNszn) <dekanszn@gmail.com>
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

#include "jasmineim.h"
#include <QDir>
#include <QDataStream>
#include <QFileInfo>
#include <qutim/icon.h>

using namespace qutim_sdk_0_3;

namespace HistoryManager {

QTextCodec *jasmineim::codec = QTextCodec::codecForName("cp-1251");

jasmineim::jasmineim()
{
}

jasmineim::~jasmineim ()
{
}

QString jasmineim::ReadMUTF8String (QDataStream &in)
{
	qint16 len;
	in >> len;
	char *data = new char[len];
	in.readRawData (data, len);
	QString retVal = QString::fromUtf8(data, len);
	delete[] data;
	return retVal;
}

QString jasmineim::ReadWin1251String (QDataStream &in)
{
	quint32 len;
	in >> len;
	char *data = new char[len];
	in.readRawData (data, len);
	QString retVal = codec->toUnicode(data);
	retVal.resize(len);
	delete[] data;
	return retVal;
}

QDataStream &operator >> (QDataStream &in, Message &message)
{
	quint8 isInputMsg;
	bool isXtrazMsg;
	quint64 milliseconds;
	in >> isInputMsg >> isXtrazMsg >> milliseconds;
	message.setIncoming(isInputMsg > 0);
	QDateTime date(QDateTime::fromTime_t(milliseconds / 1000));
	date.addMSecs(milliseconds % 1000);
	message.setTime(date);
	message.setText(jasmineim::ReadWin1251String(in));
	return in;
}

QDataStream &operator >> (QDataStream &in, ContactHistory &contact)
{
	int readed;
	Message msg;
	contact.uin = jasmineim::ReadMUTF8String(in);
	in >> readed;
	if (readed < 1)
		return in;
	in.skipRawData(readed);
	in >> readed;
	char *data = new char[readed];
	readed = in.readRawData(data, readed);
	QByteArray m(data, readed);
	delete[] data;
	QDataStream msgs(&m, QIODevice::ReadOnly);
	while (!msgs.atEnd())
	{
		msgs >> msg;
		contact.history.append(msg);
	}
	m.clear ();
	return in;
}

void jasmineim::loadMessages (const QString &path)
{
	QFileInfo info(path);
	if(!info.exists() || !info.isFile())
		return;
	QFile file(path);
	if (!file.open (QIODevice::ReadOnly))
		return;
	QDataStream input(&file);
	if(ReadMUTF8String(input) != QString(SIGNATURE))
		return;
	setProtocol("icq");
	QString account = ReadMUTF8String(input);
	setAccount(account);
	QList<ContactHistory> allContacts;
	while(!input.atEnd())
	{
		ContactHistory contact;
		input >> contact;
		allContacts.append(contact);
	}
	setMaxValue(allContacts.size());
	for (int i = 0; i < allContacts.size(); i++)
	{
		setValue(i + 1);
		setContact(allContacts[i].uin);
		QList<Message> msgs = allContacts[i].history;
		foreach (Message msg, allContacts[i].history)
		{
			appendMessage(msg);
		}
	}
	file.close();
}

bool jasmineim::validate (const QString &path)
{
	QFileInfo info(path);
	if(!info.exists() || !info.isFile())
		return false;
	QFile file(path);
	if (!file.open (QIODevice::ReadOnly))
		return false;
	QDataStream input(&file);
	bool retVal = ReadMUTF8String(input) == QString(SIGNATURE);
	file.close();
	return retVal;
}

QString jasmineim::name ()
{
	return "Jasmine";
}

QIcon jasmineim::icon ()
{
	return Icon("jasmine");
}

}
