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

#include "sim.h"
#include <QDir>
#include <QFileInfo>
#include <QTextStream>
#include <QTextCodec>
#include <QTextDocument>
#include <qutim/icon.h>

using namespace qutim_sdk_0_3;

namespace HistoryManager {

// Message flags:
const unsigned  MESSAGE_RECEIVED	= 0x00000001;
const unsigned  MESSAGE_RICHTEXT	= 0x00000002;
const unsigned  MESSAGE_SECURE		= 0x00000004;
const unsigned  MESSAGE_URGENT		= 0x00000008;
const unsigned  MESSAGE_LIST		= 0x00000010;
const unsigned  MESSAGE_NOVIEW		= 0x00000020;

const unsigned  MESSAGE_SAVEMASK	= 0x0000FFFF;

const unsigned  MESSAGE_TRANSLIT	= 0x00010000;
const unsigned  MESSAGE_1ST_PART	= 0x00020000;
const unsigned  MESSAGE_NOHISTORY	= 0x00040000;
const unsigned  MESSAGE_LAST		= 0x00080000;
const unsigned  MESSAGE_MULTIPLY	= 0x00100000;
const unsigned  MESSAGE_FORWARD		= 0x00200000;
const unsigned  MESSAGE_INSERT		= 0x00400000;
const unsigned  MESSAGE_OPEN		= 0x00800000;
const unsigned  MESSAGE_NORAISE		= 0x01000000;
const unsigned  MESSAGE_TEMP		= 0x10000000;

sim::sim()
{
}

sim::~sim()
{
}

void sim::loadMessages(const QString &path)
{
	// ~/.kde4/share/apps/sim/CoolProfile/history/ICQ.my_uin.its_uin
	QDir dir = path;
	static QStringList filters = QStringList()
						  << "Jabber.*"
						  << "ICQ.*"
						  << "AIM.*"
						  << "Yahoo.*"
						  << "MSN.*";
	QStringList profiles = dir.entryList(QDir::Dirs|QDir::NoDotAndDotDot);
	int num = 0;
	foreach(const QString &profile_name, profiles)
	{
		QString profile_path =  dir.filePath(profile_name);
		profile_path += QDir::separator();
		profile_path += "history";
		QDir profile_dir = profile_path;
		QStringList files = profile_dir.entryList(filters, QDir::Files|QDir::NoDotAndDotDot);
		num += files.size();
	}
	QTextDocument doc;
	setMaxValue(num);
	num = 0;
	foreach(const QString &profile_name, profiles)
	{
		QString profile_path =  dir.filePath(profile_name);
		profile_path += QDir::separator();
		profile_path += "history";
		QDir profile_dir = profile_path;
		QStringList files = profile_dir.entryList(filters, QDir::Files|QDir::NoDotAndDotDot, QDir::Name);
		foreach(const QString &contact_path, files)
		{
			QString protocol = contact_path.section(".", 0, 0);
			QString acccount;
			QString contact;
			if(protocol == "ICQ" || protocol == "AIM")
			{
				acccount = contact_path.section(".", 1, 1);
				contact = contact_path.section(".", 2, 2);
			}
			else
			{
				QString tmp_string = contact_path.section(".", 1);
				if(tmp_string.endsWith(".removed"))
					tmp_string.chop(8);
				acccount = tmp_string.section("+", 0, 0);
				contact  = tmp_string.section("+", 1);
			}
			setProtocol(protocol);
			setAccount(acccount);
			setContact(contact);
			setValue(++num);
			QFile file(profile_dir.filePath(contact_path));
			if (file.open(QIODevice::ReadOnly | QIODevice::Text))
			{
				QTextStream inc(&file);
				inc.setAutoDetectUnicode(false);
				inc.setCodec("Latin-1");
				//inc.setCodec(defCharset);
				QHash<QString,QString> message_data;
				QString type;
				QStringList lines = inc.readAll().remove('\r').split("\n");
				QString line;
				QTextCodec* icqCodec = QTextCodec::codecForName(charset());
				QTextCodec* defCodec = QTextCodec::codecForName(charset());
				QString icqCharset="";
				//msg=icqCodec->makeDecoder()->toUnicode(QTextCodec::codecForName(defCharset)->makeEncoder()->fromUnicode(msg));
				for(int i=0;i<lines.size()+1;i++)
				{
					if(i==lines.size())
						line="[Finish]";
					else
						line=lines[i];
					if(line.startsWith('[')&&line.endsWith(']'))
					{
						if(message_data.contains("Charset"))
						{
							icqCharset = message_data.value("Charset");
							icqCharset = icqCharset.remove(icqCharset.size()-1,1).remove(0,1);
							icqCodec = QTextCodec::codecForName(icqCharset.toLatin1());
						}
						if((type=="Message" || type==protocol+"Message") && (message_data.contains("Text") || message_data.contains("ServerText")))
						{
							int flags = message_data.value("Flags","0").toInt();
							Message message;
							message.setTime(QDateTime::fromTime_t(message_data.value("Time").toULongLong()));
							message.setIncoming((flags & MESSAGE_RECEIVED) > 0);
							QString text;
							if(message_data.contains("Text"))
								text = QString::fromUtf8(message_data.value("Text").toLatin1());
							else
								text = ((type=="Message") ? defCodec : icqCodec)->toUnicode(message_data.value("ServerText").toLatin1());
							text.chop(1);
							text.remove(0, 1);
							text.remove("\\x0d").replace("\\n", "\n");
							if (flags & MESSAGE_RICHTEXT) {
								message.setProperty("html", text);
								doc.setHtml(text);
								message.setText(doc.toPlainText());
								doc.clearUndoRedoStacks();
							} else {
								message.setText(text);
							}
							appendMessage(message);
						}
						type=line.mid(1,line.length()-2);
						message_data.clear();
					}
					else
						message_data.insert(line.section('=',0,0),line.section('=',1));
				}
			}
		}
	}
}

bool sim::validate(const QString &path)
{
	QDir dir = path;
	QStringList filters = QStringList()
						  << "Jabber.*"
						  << "ICQ.*"
						  << "AIM.*"
						  << "Yahoo.*"
						  << "MSN.*";
	QStringList profiles = dir.entryList(QDir::Dirs|QDir::NoDotAndDotDot);
	foreach(const QString &profile_name, profiles)
	{
		QString profile_path =  dir.filePath(profile_name);
		profile_path += QDir::separator();
		profile_path += "history";
		QDir profile_dir = profile_path;
		QStringList files = profile_dir.entryList(filters, QDir::Files|QDir::NoDotAndDotDot);
		if(!files.isEmpty())
			return true;
	}
	return false;
}

QString sim::name()
{
	return "SIM";
}

QIcon sim::icon()
{
	return Icon("sim");
}

}

