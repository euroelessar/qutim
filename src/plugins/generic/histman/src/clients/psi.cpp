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

#include "psi.h"
#include <QDir>
#include <QFileInfo>
#include <QTextStream>
#include <QTextDocument>
#include <QCoreApplication>
#include <qutim/icon.h>

using namespace qutim_sdk_0_3;

namespace HistoryManager {

psi::psi()
{
}

QString psi::decode(const QString &jid)
{
	QString jid2;
	int n;

	for(n = 0; n < (int)jid.length(); ++n) {
		if(jid.at(n) == '%' && (jid.length() - n - 1) >= 2) {
			QString str = jid.mid(n+1,2);
			bool ok;
			char c = str.toInt(&ok, 16);
			if(!ok)
				continue;

			QChar a(c);
			jid2.append(a);
			n += 2;
		}
		else {
			jid2.append(jid.at(n));
		}
	}

	// search for the _at_ backwards, just in case
	for(n = (int)jid2.length(); n >= 3; --n) {
		if(jid2.mid(n, 4) == "_at_") {
			jid2.replace(n, 4, "@");
			break;
		}
	}

	return jid2;
}

QString psi::logdecode(const QString &str)
{
	QString ret;

	for(int n = 0; n < str.length(); ++n) {
		if (str.at(n) == '\\') {
			++n;
			if (n >= str.length())
				break;

			if (str.at(n) == 'n')
				ret.append('\n');
			if (str.at(n) == 'p')
				ret.append('|');
			if (str.at(n) == '\\')
				ret.append('\\');
		} else {
			ret.append(str.at(n));
		}
	}

	return ret;
}

void psi::loadMessages(const QString &path)
{
	QDir dir = path;
	if(!dir.cd("history"))
		return;
	QFileInfoList files = dir.entryInfoList(QStringList() << "*.history", QDir::Files);
	setProtocol("jabber");
	setAccount(m_account);
	setMaxValue(files.size());
	for(int i = 0; i < files.size(); i++)
	{
		setValue(i + 1);
		QString contact = files[i].fileName();
		contact.chop(4);
		contact = decode(contact);
		setContact(contact);
		QFile file(files[i].absoluteFilePath());
		if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
			continue;
		QTextStream in(&file);
		in.setCodec("utf-8");
		while(!in.atEnd())
		{
			static const QChar c('|');
			QString line = in.readLine();
			if(line.isEmpty())
				continue;
			// |2008-07-13T15:27:57|5|from|N3--|Цитата #397796|http://bash.org.ru/quote/397796|Цитата #397796|xxx: cool text
			Message message;
			message.setTime(QDateTime::fromString(line.section(c, 1, 1), Qt::ISODate));
			message.setIncoming(line.section(c, 3, 3) == "from");
			QString text = line.mid(line.lastIndexOf(c) + 1);
			int psi_type = line.section(c, 2, 2).toInt();
			if(psi_type == 2 || psi_type == 3 || psi_type == 6 || psi_type == 7 || psi_type == 8 || text.isEmpty())
				continue;
			message.setText(logdecode(text));
			appendMessage(message);
		}
	}
}

bool psi::validate(const QString &path)
{
	QDir dir = path;
	if(!dir.cd("history"))
		return false;
	QStringList files = dir.entryList(QStringList() << "*.history", QDir::Files);
	return !files.isEmpty();
}

QString psi::name()
{
	return "Psi";
}

QIcon psi::icon()
{
	return Icon("psi");
}

QList<ConfigWidget> psi::config()
{
	return QList<ConfigWidget>() << (m_config = createAccountWidget("Jabber"));
}

bool psi::useConfig()
{
	m_account = m_config.second->property("currentText").toString();
	return true;
}

QString psi::additionalInfo()
{
	return QCoreApplication::translate("ClientConfigPage", "Select your Jabber account.");
}

}

