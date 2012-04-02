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

#include "pidgin.h"
#include <QtXml>
#include <QString>
#include <QFileInfo>
#include <QTextDocument>
#include <qutim/icon.h>

using namespace qutim_sdk_0_3;

namespace HistoryManager {

pidgin::pidgin()
{
}

pidgin::~pidgin()
{
}

void pidgin::loadMessages(const QString &path)
{
	QDir root = path;
	if(!root.cd("logs"))
		return;
	int num = 0;
	QStringList protocol_dirs = root.entryList(QDir::Dirs|QDir::NoDotAndDotDot);
	foreach(QString protocol, protocol_dirs)
	{
		QDir protocol_dir(root.filePath(protocol));
		QStringList account_dirs = protocol_dir.entryList(QDir::Dirs|QDir::NoDotAndDotDot);
		foreach(QString account, account_dirs)
		{
			QDir dir(protocol_dir.filePath(account));
			QStringList contacts = dir.entryList(QDir::Dirs | QDir::NoDotAndDotDot);
			foreach(const QString &contact, contacts)
			{
				QDir contact_dir(dir.filePath(contact));
				QFileInfoList files = contact_dir.entryInfoList(QStringList() << "*.html", QDir::Files|QDir::NoDotAndDotDot);
				num += files.size();
			}
		}
	}
	setMaxValue(num);
	num = 0;
	QTextDocument converter;
	const QStringList stamps = QStringList()
							   << "(hh:mm:ss)</font"
							   << "(yyyy-MM-dd hh:mm:ss)</font"
							   << "(dd.MM.yyyy hh:mm:ss)</font"
							   << "(yyyy.MM.dd hh:mm:ss)</font"
							   << "(dd-MM-yyyy hh:mm:ss)</font"
							   << "(hh:mm:ss AP)</font"
							   << "(hh:mm:ss ap)</font";
	// "(2009-02-13 20:27:43)</font"
	foreach(QString protocol_dir_name, protocol_dirs)
	{
		QDir protocol_dir(root.filePath(protocol_dir_name));
		QStringList account_dirs = protocol_dir.entryList(QDir::Dirs|QDir::NoDotAndDotDot);
		QString protocol = protocol_dir_name.toUpper();
		setProtocol(protocol.toLower());
		foreach(const QString &account, account_dirs)
		{
			setAccount(account);
			QDir dir(protocol_dir.filePath(account));
			QStringList contacts = dir.entryList(QDir::Dirs | QDir::NoDotAndDotDot);
			foreach(const QString &contact, contacts)
			{
				setContact(contact);
				QDir contact_dir(dir.filePath(contact));
				QFileInfoList files = contact_dir.entryInfoList(QStringList() << "*.html", QDir::Files|QDir::NoDotAndDotDot);
				foreach(QFileInfo fileInfo, files)
				{
					setValue(++num);
					QFile file(fileInfo.absoluteFilePath());
					//2008-07-23.163259+0600YEKST.html
					QString dayString = fileInfo.fileName();
					dayString = dayString.remove(4,1).remove(6,1);
					dayString.truncate(8);
					QDate day = QDate().fromString(dayString,"yyyyMMdd");
					uint last=0;
					if (file.open(QIODevice::ReadOnly | QIODevice::Text))
					{
						bool mes=true;
						QTextStream inc(&file);
						inc.setAutoDetectUnicode(false);
						inc.setCodec(charset());
						//inc.setAutoDetectUnicode(false);
						//inc.setCodec("cp1251");
						QStringList lines = inc.readAll().split('\n');
						for (int i = 1; i < lines.size()-2; ++i)
						{
							Message message;
	//<font color="#16569E"><font size="2">(16:35:00)</font> <b>EuroElessar:</b></font> gergr<br/>
							mes=true;
							if(lines[i].startsWith("<font color=\"#16569E\">"))
								message.setIncoming(false);
							else if(lines[i].startsWith("<font color=\"#A82F2F\">"))
								message.setIncoming(true);
							else
								mes=false;
							if (mes) {
								QDateTime date;
								QString date_string = lines[i].section(">",2,2);
								for(int j=0;j<stamps.size();j++) {
									if(j==0) {
										QTime time = QTime::fromString(date_string, stamps[j]);
										if(!time.isValid())
											continue;
										uint cur = time.hour()*3600+time.minute()*60+time.second();
										if(cur<last)
											day=day.addDays(1);
										last=cur;
										date = QDateTime(day,time);
										break;
									} else {
										date = QDateTime::fromString(date_string, stamps[j]);
										if(!date.isValid())
											continue;
										day = date.date();
										last = date.time().hour()*3600+date.time().minute()*60+date.time().second();
										break;
									}
								}
								message.setTime(date);
								QString text = lines[i].remove(0,lines[i].lastIndexOf("font>")+6);
								text.chop(5);
								converter.setHtml(text);
								message.setText(converter.toPlainText());
								converter.clearUndoRedoStacks();
								message.setProperty("html", text);
								appendMessage(message);
							}
						}
					}
				}
			}
		}
	}
}

bool pidgin::validate(const QString &path)
{
	QDir root = path;
	if(!root.cd("logs"))
		return false;
	QStringList protocol_dirs = root.entryList(QDir::Dirs|QDir::NoDotAndDotDot);
	foreach(QString protocol, protocol_dirs)
	{
		QDir protocol_dir(root.filePath(protocol));
		QStringList account_dirs = protocol_dir.entryList(QDir::Dirs|QDir::NoDotAndDotDot);
		foreach(QString account, account_dirs)
		{
			QDir dir(protocol_dir.filePath(account));
			QStringList contacts = dir.entryList(QDir::Dirs | QDir::NoDotAndDotDot);
			foreach(const QString &contact, contacts)
			{
				QDir contact_dir(dir.filePath(contact));
				QFileInfoList files = contact_dir.entryInfoList(QStringList() << "*.html", QDir::Files|QDir::NoDotAndDotDot);
				if(!files.isEmpty())
					return true;
			}
		}
	}
	return false;
}

QString pidgin::name()
{
	return "Pidgin";
}

QIcon pidgin::icon()
{
	return Icon("pidgin");
}

}

