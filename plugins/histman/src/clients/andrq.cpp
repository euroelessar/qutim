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

#include "andrq.h"
#include <QTextDocument>
#include <qutim/icon.h>

using namespace qutim_sdk_0_3;

namespace HistoryManager {

andrq::andrq()
{
}

andrq::~andrq()
{
}

QString andrq::getString(QDataStream &in, int key)
{
	qint32 len;
	in >> len;
    char *data = (char *)malloc(len);
	in.readRawData(data, len);
	int dh = 0, dl = 0;

	int temp = key;
	dl |= temp & 0x000000FF;
	temp >>= 20;
	dh |= temp & 0x000000FF;

/*	if (!ans)
		return false;*/

	int ah = 184; //10111000b;
	for (int i = 0; i < len; ++i)
	{
		int al = uchar(data[i]);
		al ^= ah;

		al=(al%32)*8+al/32;	//циклический сдвиг al на 3 бита влево

		al ^= dh;
		al -= dl;

		data[i] = char(al);
		ah=(ah%8)*32+ah/8;	//циклический сдвиг ah вправо на 3 бита
	}
	static QTextCodec *ansii_codec = QTextCodec::codecForName("cp1251");
	static QTextCodec *utf8_codec = QTextCodec::codecForName("utf-8");
	QTextCodec *codec = isValidUtf8(QByteArray::fromRawData(data, len)) ? utf8_codec : ansii_codec;
	QString result = codec->toUnicode(data, len);
    free(data);
	return result;
}

bool andrq::isValidUtf8(const QByteArray &array)
{
	int i = 0;
	unsigned int c;
	unsigned int n = 0, r = 0;
	unsigned char x;
	while (i < array.size()) {
		x = array[i];
		c = x;
		i ++;
		for (n = 0; (c & 0x80) == 0x80; c <<= 1) n ++;
		if (r > 0) {
			if (n == 1) {
				r --;
			} else {
				return false;
			}
		} else if (n == 1) {
			return false;
		} else if (n != 0) {
			r = n - 1;
		}
	}
	if (r > 0) {
		return false;
	}
	return true;
}

QDateTime andrq::getDateTime(QDataStream &in)
{
	static QDateTime zerodate;
	if(zerodate.isNull())
	{
		QDate date(1899, 12, 30);
		QTime time(0, 0, 0, 0);
		zerodate.setDate(date);
		zerodate.setTime(time);
	}
	double time;
	in >> time;
	int day = (int)time;
	int sec = (int)((time-day)*86400);
	return zerodate.addDays(day).addSecs(sec);

}

void andrq::loadMessages(const QString &path)
{
	QDir dir = path;
	QString account = dir.dirName();
	if(!dir.cd("history"))
		return;
	setProtocol("icq");
	setAccount(account);
	QFileInfoList files = dir.entryInfoList(QDir::Files);
	setMaxValue(files.size());
	for(int i = 0; i < files.size(); i++)
	{
		setValue(i + 1);
		QString uin = files[i].fileName();
		QFile file(files[i].absoluteFilePath());
		if(!file.open(QFile::ReadOnly))
			continue;
		setContact(uin);
		QDataStream in(&file);
		in.setByteOrder(QDataStream::LittleEndian);
		Message message;
		while(!in.atEnd())
		{
			qint32 type;
			in >> type;
			switch(type)
			{
			case -1: {
				quint8 kind;
				qint32 who;
				in >> kind >> who;
				QString from = QString::number(who);
				message.setIncoming(from == uin);
				message.setTime(getDateTime(in));
				qint32 tmp;
				in >> tmp;
				in.skipRawData(tmp);
				message.setText(getString(in, who));
				if(kind==1)
					appendMessage(message);
				break; }
			case -2: {
				qint32 tmp;
				in >> tmp;
				in.skipRawData(tmp);
				break; }
			case -3:
				in.skipRawData(5);
				break;
			default:
				break;
			}
		}
	}
}

bool andrq::validate(const QString &path)
{
	QDir dir = path;
	if(!dir.cd("history"))
		return false;
	QStringList files = dir.entryList(QDir::Files);
	return !files.isEmpty();
}

QString andrq::name()
{
	return "&RQ";
}

QIcon andrq::icon()
{
	return Icon("rnq");
}

}

