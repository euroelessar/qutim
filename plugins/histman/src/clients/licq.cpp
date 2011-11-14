/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright (C) 2011 Ruslan Nigmatullin <euroelessar@yandex.ru>
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

#include "licq.h"
#include <QDir>
#include <QFileInfo>
#include <QTextStream>
#include <QTextDocument>
#include <QSettings>
#include <qutim/icon.h>

using namespace qutim_sdk_0_3;

namespace HistoryManager {

licq::licq()
{
}

licq::~licq()
{
}

void licq::loadMessages(const QString &path)
{
	QDir dir = path;
	QSettings settings(dir.filePath("owner.Licq"), QSettings::IniFormat);
	QString account = settings.value("Uin").toString();
	if(!dir.cd("history"))
		return;
	setProtocol("icq");
	setAccount(account);
	QFileInfoList files = dir.entryInfoList(QDir::Files|QDir::NoDotAndDotDot);
	setMaxValue(files.size());
	for(int i=0;i<files.size();i++)
	{
		setValue(i + 1);
		QString uin = files[i].fileName();
		uin.truncate(uin.indexOf('.'));
		setContact(uin);
		QFile file(files[i].absoluteFilePath());
		if (file.open(QIODevice::ReadOnly | QIODevice::Text))
		{
			QTextStream inc(&file);
			inc.setAutoDetectUnicode(false);
			inc.setCodec(charset());
			QStringList lines = inc.readAll().remove('\r').split("\n");
			bool first=true;
			Message message;
			QString text;
			for (int i = 0; i < lines.size(); ++i)
			{
				if(lines[i].startsWith("["))
				{
					//[ R | 0001 | 0260 | 0000 | 1169984229 ]
					if(first) {
						first=!first;
					} else {
						text.chop(5);
						message.setText(text);
						appendMessage(message);
					}
					message.setIncoming(lines[i].section(" ",1,1)=="R");
					message.setTime(QDateTime::fromTime_t(lines[i].section(" ",9,9).toULongLong()));
					text.clear();
				}
				else {
					text += lines[i].section(":",1);
					text += '\n';
				}
			}
			text.chop(5);
			message.setText(text);
			appendMessage(message);
		}
	}
}

bool licq::validate(const QString &path)
{
	QDir dir = path;
	if(!dir.cd("history"))
		return false;
	QStringList files = dir.entryList(QDir::Files|QDir::NoDotAndDotDot);
	return !files.isEmpty();
}

QString licq::name()
{
	return "Licq";
}

QIcon licq::icon()
{
	return Icon("licq");
}

}

