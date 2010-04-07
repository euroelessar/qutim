/****************************************************************************
 *  licq.cpp
 *
 *  Copyright (c) 2010 by Nigmatullin Ruslan <euroelessar@gmail.com>
 *
 ***************************************************************************
 *                                                                         *
 *   This library is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************
*****************************************************************************/

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
