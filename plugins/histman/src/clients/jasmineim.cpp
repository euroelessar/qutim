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

#define OLD_SIGNATURE 0x00044A50
#define NEW_SIGNATURE 0x4A484132
#define NEW_MSG_SIGN_LEN 3

QTextCodec *jasmineim::codec = QTextCodec::codecForName("cp-1251");
bool jasmineim::isOldFormat = true;
bool jasmineim::isIcqImport = true;

jasmineim::jasmineim()
{
}

jasmineim::~jasmineim()
{
}

QString jasmineim::readMUTF8String(QDataStream &in)
{
	qint16 len;
	in >> len;
	QByteArray data(len, Qt::Uninitialized);
	in >> data;
	QString retVal = QString::fromUtf8(data, len);
	return retVal;
}

QString jasmineim::readWin1251String(QDataStream &in)
{
	quint32 len;
	in >> len;
	char *data = new char[len + 1];
	in.readRawData(data, len);
	data[len] = '\0';
	QString retVal = codec->toUnicode(data, len);
	retVal.resize(len);
	delete[] data;
	return retVal;
}

QDataStream &operator >> (QDataStream &in, Message &message)
{
	quint8 isInputMsg;
	quint64 milliseconds;
	in >> isInputMsg;
	if (jasmineim::isIcqImport)	{
		bool isXtrazMsg;
		in >> isXtrazMsg;
		if (!jasmineim::isOldFormat) {
			// this is place for future empowerment
			int skipThis;
			in >> skipThis;
		}
	}
	in >> milliseconds;
	if (!jasmineim::isIcqImport) {
		// this is place for future empowerment
		int skipThis;
		in >> skipThis;
	}
	QString msgText;
	if (jasmineim::isOldFormat)
		msgText = jasmineim::readWin1251String(in);
	else in >> msgText;
	message.setIncoming(isInputMsg > 0);
	QDateTime date;
	date.setMSecsSinceEpoch(milliseconds);
	message.setTime(date);
	message.setText(msgText);
	return in;
}

void jasmineim::loadMessages(const QString &path)
{
	QFileInfo info(path);
	if (!info.exists() || !info.isFile())
		return;
	QFile file(path);
	if (!file.open(QIODevice::ReadOnly))
		return;
	QDataStream input(&file);
	quint32 magic;
	input >> magic;
	isOldFormat = (magic == OLD_SIGNATURE);
	if (isOldFormat)
		input.skipRawData(2);
	if (!isOldFormat && (magic != NEW_SIGNATURE))
		return;
	QString account;
	if (isOldFormat)
		account = readMUTF8String(input);
	else input >> account;
	QString protocol;
	if (isOldFormat) {
		protocol = QLatin1String("icq");
	} else {
		quint8 protocolType, subType;
		input >> protocolType >> subType;
		switch (protocolType) {
			case 0:
				protocol = QLatin1String("icq");
				break;
			case 1:
				protocol = QLatin1String("jabber");
				isIcqImport = false;
				break;
			case 2:
				protocol = QLatin1String("mrim");
				isIcqImport = false;
				break;
		}
	}
	setProtocol(protocol);
	setAccount(account);
	setMaxValue(file.size());
	while(!input.atEnd()) {
		int readed;
		Message msg;
		QString uin;
		if (jasmineim::isOldFormat)
			uin = jasmineim::readMUTF8String(input);
		else
			input >> uin;
		setContact(uin);
		input >> readed;
		input.skipRawData(readed);
		if (jasmineim::isOldFormat && (readed < 1))
			continue;
		input >> readed;
		if (!jasmineim::isOldFormat) {
			input.skipRawData(NEW_MSG_SIGN_LEN);
			readed -= NEW_MSG_SIGN_LEN;
			if (readed < 1)
				continue;
		}
		char *data = new char[readed];
		readed = input.readRawData(data, readed);
		QByteArray m(data, readed);
		delete[] data;
		QDataStream msgs(&m, QIODevice::ReadOnly);
		while (!msgs.atEnd()) {
			msgs >> msg;
			appendMessage(msg);
			setValue(file.pos());
		}
		m.clear();
	}
	file.close();
}

bool jasmineim::validate(const QString &path)
{
	QFileInfo info(path);
	if (!info.exists() || !info.isFile())
		return false;
	QFile file(path);
	if (!file.open(QIODevice::ReadOnly))
		return false;
	QDataStream input(&file);
	quint32 magic;
	input >> magic;
	bool retVal = (magic == OLD_SIGNATURE);
	if (retVal)
		input.skipRawData(2);
	else retVal = (magic == NEW_SIGNATURE);
	file.close();
	return retVal;
}

QString jasmineim::name()
{
	return "Jasmine";
}

QIcon jasmineim::icon()
{
	return Icon("jasmine");
}

}
