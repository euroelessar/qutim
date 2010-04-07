/****************************************************************************
 *  qip.cpp
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

#include "qip.h"
#include <QDir>
#include <QFile>
#include <QTextStream>
#include <QTextDocument>
#include <qutim/icon.h>

using namespace qutim_sdk_0_3;

namespace HistoryManager {

qip::qip()
{
}

qip::~qip()
{
}

void qip::loadMessages(const QString &path)
{
	QDir dir = path;
	QString account = dir.dirName();
	if(!dir.cd("History"))
		return;
	setProtocol("ICQ");
	setAccount(account);
	QStringList files = dir.entryList(QStringList() << "*.txt", QDir::Files);
	setMaxValue(files.size());
	for(int i = 0; i < files.size(); i++)
	{
		setValue(i + 1);
		if(files[i] == "_srvlog.txt"  || files[i] == "_botlog.txt" || files[i].startsWith("."))
			continue;
		QFile file(dir.filePath(files[i]));
		if (file.open(QIODevice::ReadOnly | QIODevice::Text))
		{
			Message message;
			QTextStream in(&file);
			in.setAutoDetectUnicode(false);
			in.setCodec("cp1251");
			QString uin = files[i];
			uin.chop(4);
			setContact(uin);
			QString text;
			bool first = true;
			bool point = false;
			while(!in.atEnd())
			{
				QString line = in.readLine();
				if(line == "-------------------------------------->-"
				   || line == "--------------------------------------<-")
				{
					if(!first)
					{
						text.chop(10);
						message.setText(text);
						appendMessage(message);
					}
					else
						first=false;
					message.setIncoming(line[38] == '<');
					point = true;
					text.clear();
				}
				else if(point)
				{
					if(account.isEmpty())
						account = line.section(' ',0,-3);
					message.setTime(QDateTime().fromString(line.section(' ',-2),"(hh:mm:ss d/MM/yyyy)"));
					point=false;
				}
				else
				{
					text += line;
					text += '\n';
				}
			}
			text.chop(10);
			message.setText(text);
			appendMessage(message);
		}
	}
}

bool qip::validate(const QString &path)
{
	QDir dir = path;
	if(!dir.cd("History"))
		return false;
	QStringList files = dir.entryList(QStringList() << "*.txt", QDir::Files);
	return !files.isEmpty();
}

QString qip::name()
{
	return "QIP 2005";
}

QIcon qip::icon()
{
	return Icon("qip-2005");
}

}
