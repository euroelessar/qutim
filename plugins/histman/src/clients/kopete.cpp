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

#include "kopete.h"
#include <QtXml>
#include <QTextDocument>
#include <qutim/icon.h>
#include <qutim/protocol.h>
#include <qutim/account.h>
#include <qutim/buddy.h>

using namespace qutim_sdk_0_3;

namespace HistoryManager {

kopete::kopete()
{
}

kopete::~kopete()
{
}

bool kopete::validate(const QString &kopete_id, const QString &id)
{
	if(kopete_id.size() == id.size())
	{
//		FIXME: It's too slow
		static const QRegExp regexp("[./~?*]");
		QString tmp_id = id;
		if(tmp_id.replace(regexp, "-") == kopete_id)
			return true;
	}
	return false;
}

QString kopete::guessFromList(const QString &kopete_id, const QStringList &list)
{
	for(int i = 0; i < list.size(); i++)
	{
		if(validate(kopete_id, list[i]))
			return list[i];
	}
	return QString();
}

inline void kopete_id_helper(QHash<QString, QString> &hash, const QString &id)
{
	static const QRegExp regexp("[./~?*]");
	if(id.contains(regexp))
	{
		QString tmp_id = id;
		hash.insert(tmp_id.replace(regexp, "-"), id);
	}
}

QString kopete::guessID(const QString &kopete_id)
{
	static QHash<QString, QString> hash;
	if(hash.isEmpty())
	{
		foreach (qutim_sdk_0_3::Protocol *proto, qutim_sdk_0_3::Protocol::all()) {
			kopete_id_helper(hash, proto->id());
			foreach (qutim_sdk_0_3::Account *account, proto->accounts()) {
				kopete_id_helper(hash, account->id());
				foreach (Buddy *buddy, account->findChildren<Buddy*>())
					kopete_id_helper(hash, buddy->id());
			}
		}
	}
	return hash.value(kopete_id, kopete_id);
}

void kopete::loadMessages(const QString &path)
{
	QDir dir = path;
	if(!dir.cd("logs"))
		return;
	int num=0;
	QStringList protocol_dirs = dir.entryList(QDir::Dirs|QDir::NoDotAndDotDot);
	foreach(QString protocol, protocol_dirs)
	{
		QDir protocol_dir(dir.filePath(protocol));
		QStringList account_dirs = protocol_dir.entryList(QDir::Dirs|QDir::NoDotAndDotDot);
		foreach(QString account, account_dirs)
		{
			QDir dir(protocol_dir.filePath(account));
			QStringList files = dir.entryList(QDir::Files|QDir::NoDotAndDotDot);
			num+=files.size();
		}
	}
	setMaxValue(num);
	num = 0;

	foreach(QString protocol_name, protocol_dirs)
	{
		QDir protocol_dir(dir.filePath(protocol_name));
		QStringList account_dirs = protocol_dir.entryList(QDir::Dirs|QDir::NoDotAndDotDot);
		QString protocol = protocol_name.remove("Protocol");
		setProtocol(guessID(protocol).toLower());
		foreach(QString account, account_dirs)
		{
			QFileInfoList files = QDir(protocol_dir.filePath(account)).entryInfoList(QDir::Files|QDir::NoDotAndDotDot);
			setAccount(guessID(account));
			for(int i=0;i<files.size();i++)
			{
				setValue(++num);
				if(!files[i].fileName().startsWith("log.20"))
				{
					QFile file(files[i].absoluteFilePath());
					if (file.open(QIODevice::ReadOnly) )
					{
						QDomDocument doc;
						if(doc.setContent(&file))
						{
							QDomElement rootElement = doc.documentElement();
							QDomElement headElement = rootElement.firstChildElement("head");
							QDomElement dateElement = headElement.firstChildElement("date");
							int msgCount = rootElement.elementsByTagName("msg").count();
							QString otherDate = dateElement.attribute("month")+"/"+dateElement.attribute("year");
							QDomElement msg = rootElement.firstChildElement("msg");
							QString uin = files[i].fileName();
							uin.truncate(uin.indexOf('.'));
							setContact(guessID(uin));
							for ( int j = 0; j < msgCount ; j++ )
							{
								QString datetime = msg.attribute("time");
								datetime += " ";
								datetime += otherDate;
								Message message;
								message.setTime(QDateTime::fromString(datetime,"d h:m:s M/yyyy"));
								message.setText(msg.text());
								message.setIncoming(msg.attribute("in") == "1");
								appendMessage(message);
								msg = msg.nextSiblingElement("msg");
							}
						}
					}
				}
			}
		}
	}
}

// ~/.kde/share/apps/kopete/
bool kopete::validate(const QString &path)
{
	QDir dir = path;
	if(!dir.cd("logs"))
		return false;
	QStringList protocol_dirs = dir.entryList(QDir::Dirs|QDir::NoDotAndDotDot);
	foreach(const QString &protocol, protocol_dirs)
	{
		QDir protocol_dir = dir.filePath(protocol);
		QStringList account_dirs = protocol_dir.entryList(QDir::Dirs|QDir::NoDotAndDotDot);
		static QStringList filter = QStringList() << "*.xml";
		foreach(const QString &account, account_dirs)
		{
			if(!QDir(protocol_dir.filePath(account)).entryList(filter, QDir::Files).isEmpty())
				return true;
		}
	}
	return false;
}

QString kopete::name()
{
	return "Kopete";
}

QIcon kopete::icon()
{
	return Icon("kopete");
}

}

