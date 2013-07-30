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

#include "historymanagerwindow.h"
#include "chooseclientpage.h"
#include "clientconfigpage.h"
#include "importhistorypage.h"
#include "dumphistorypage.h"
#include "chooseordumppage.h"
#include <qutim/icon.h>
#include <qutim/iconloader.h>
#include <qutim/account.h>
#include <qutim/protocol.h>
#include <qutim/systeminfo.h>
#include <QLabel>
#include <QTextDocument>
#include <QComboBox>

using namespace qutim_sdk_0_3;

namespace HistoryManager {

int inline compare_datetime_helper(const QDateTime &dt1, const QDateTime &dt2)
{
	QDateTime dtu1 = dt1.toUTC();
	QDateTime dtu2 = dt2.toUTC();
	QDate d1 = dtu1.date();
	QDate d2 = dtu2.date();
	if(d1 == d2)
		return dtu2.time().secsTo(dtu1.time());
	else
		return d2.daysTo(d1);
}

bool compare_message_helper(const Message &msg1, const Message &msg2)
{
	int cmp_d = compare_datetime_helper(msg1.time(), msg2.time());
	if(!cmp_d)
	{
		int cmp_m = msg1.text().compare(msg2.text());
		if(!cmp_m)
			return msg1.isIncoming() && !msg2.isIncoming();
		else
			return cmp_m < 0;
	}
	else
		return cmp_d < 0;
}

HistoryManagerWindow::HistoryManagerWindow(QWidget *parent) :
	QWizard(parent)
{
	m_protocol = 0;
	m_account = 0;
	m_contact = 0;
	m_current_client = 0;
	m_message_num = 0;
	m_qutim = new qutim();
	setPixmap(WatermarkPixmap, QPixmap(":/pictures/wizard.png"));
#ifndef Q_WS_MAC
	setWizardStyle(ModernStyle);
#endif
	setWindowTitle(tr("History manager"));
	setWindowIcon(Icon("view-history"));
	setPage(ChooseClient, new ChooseClientPage(this));
	setPage(ConfigClient, new ClientConfigPage(this));
	setPage(ImportHistory, new ImportHistoryPage(this));
	setPage(ChooseOrDump, new ChooseOrDumpPage(this));
	setPage(ExportHistory, new DumpHistoryPage(this));
//	qutim_sdk_0_2::SystemsCity::PluginSystem()->centerizeWidget(this);
	setAttribute(Qt::WA_QuitOnClose, false);
	setAttribute(Qt::WA_DeleteOnClose, true);
	m_dump = tr("&Dump");
	m_is_dumping = false;
}

HistoryManagerWindow::~HistoryManagerWindow()
{
}

void HistoryManagerWindow::appendMessage(const Message &message)
{
	m_is_dumping = false;
	Q_ASSERT(m_contact);
	QDate date = message.time().date();
	qint64 month_id = date.year() * 100 + date.month();
	qutim_sdk_0_3::MessageList &month = m_contact->operator [](month_id);
	int position = qLowerBound(month.begin(), month.end(), message, compare_message_helper) - month.begin();
	if(month.size() != position
	   && month[position].time() == message.time()
	   && month[position].isIncoming() == message.isIncoming()
	   && month[position].text() == message.text())
		return;
	m_message_num++;
	month.insert(position, message);
}

void HistoryManagerWindow::setProtocol(const QString &protocol)
{
	m_is_dumping = false;
	m_protocol = &m_protocols.operator [](protocol);
}

void HistoryManagerWindow::setAccount(const QString &account)
{
	m_is_dumping = false;
	Q_ASSERT(m_protocol);
	m_account = &m_protocol->operator [](account);
}

void HistoryManagerWindow::setContact(const QString &contact)
{
	m_is_dumping = false;
	Q_ASSERT(m_account);
	m_contact = &m_account->operator [](contact);
}

void HistoryManagerWindow::setMaxValue(int max)
{
	if(m_is_dumping)
		emit saveMaxValueChanged(max);
	else
		emit maxValueChanged(max);
}

void HistoryManagerWindow::setValue(int value)
{
	if(m_is_dumping)
		emit saveValueChanged(value);
	else
		emit valueChanged(value);
}

ConfigWidget HistoryManagerWindow::createAccountWidget(const QString &protocol)
{
	QLabel *label = new QLabel;
	QString html = IconLoader::iconPath("im-" + protocol.toLower(), 16);
	if(html.isEmpty())
		html = protocol;
	else
	{
        QString tmp = protocol.toHtmlEscaped();
		tmp += " <img src=\"";
		tmp += html;
		tmp += "\"/>";
		html = tmp;
	}
	label->setText(html);
	QComboBox *combo = new QComboBox;
	combo->setEditable(true);
	if (qutim_sdk_0_3::Protocol *proto = qutim_sdk_0_3::Protocol::all().value(protocol)) {
		foreach (qutim_sdk_0_3::Account *acc, proto->accounts())
			combo->addItem(acc->id(), acc->id());
	}
	return ConfigWidget(label, combo);
}

QString quoteByFormat(const QString &text, char format)
{
	if(format == 'j')
		return qutim::quote(text);
	else if(format == 'b')
		return QLatin1String(text.toUtf8().toHex());
	return text;
}

void HistoryManagerWindow::saveMessages(char format)
{
	if(format != 'b' && format != 'j')
		return;
	int total_count = 0;
	int num = 0;
	foreach(const Protocol &protocol, m_protocols)
		foreach(const Account &account, protocol)
			foreach(const Contact &contact, account)
				total_count += contact.size();
	emit saveMaxValueChanged(total_count);
	QDir dir = SystemInfo::getDir(SystemInfo::HistoryDir);
	if(!dir.exists())
		dir.mkpath(dir.absolutePath());

	QHash<QString, Protocol>::const_iterator protocol = m_protocols.constBegin();
	for(; protocol != m_protocols.constEnd(); protocol++)
	{
		QHash<QString,Account>::const_iterator account = protocol.value().constBegin();
		for(; account != protocol.value().constEnd(); account++)
		{
			QString account_path = protocol.key();
			account_path += ".";
			account_path += quoteByFormat(account.key(), format);
			if(!dir.exists(account_path))
				dir.mkdir(account_path);
			QDir account_dir = dir.filePath(account_path);
			QHash<QString,Contact>::const_iterator contact = account.value().constBegin();
			for(; contact != account.value().constEnd(); contact++)
			{
				QMap<qint64,MessageList>::const_iterator month = contact.value().constBegin();
				for(; month != contact.value().constEnd(); month++)
				{
					emit saveValueChanged(++num);
					QString filename = quoteByFormat(contact.key(), format);
					filename += ".";
					filename += QString::number(month.key());
//					if(format == 'j')
						filename += ".json";
//					else
//						filename += ".log";
					QFile file(account_dir.filePath(filename));
//					if(format == 'j')
					{
						if(!file.open(QIODevice::WriteOnly | QIODevice::Text))
							continue;
						file.write("[\n");
						bool first = true;
						foreach(const Message &message, month.value())
						{
							if(first)
								first = false;
							else
								file.write(",\n");
							file.write(" {\n");
							foreach(const QByteArray &name, message.dynamicPropertyNames())
							{
								QByteArray data;
								if(!Json::generate(data, message.property(name), 2))
									continue;
								file.write("  ");
								file.write(Json::quote(QString::fromUtf8(name)).toUtf8());
								file.write(": ");
								file.write(data);
								file.write(",\n");
							}
							file.write("  \"datetime\": \"");
							QDateTime time = message.time();
							if(!time.isValid())
								time = QDateTime::currentDateTime();
							file.write(time.toString(Qt::ISODate).toLatin1());
							file.write("\",\n  \"in\": ");
							file.write(message.isIncoming() ? "true" : "false");
							file.write(",\n  \"text\": ");
							file.write(Json::quote(message.text()).toUtf8());
							file.write("\n }");
						}
						file.write("\n]");
					}
//					else if(format == 'b')
//					{
//						if(!file.open(QIODevice::WriteOnly))
//							continue;
//						QDataStream out(&file);
//						foreach(const Message &msg, month.value())
//							out << msg.time << msg.type << msg.in << msg.text;
//					}
				}
			}
		}
	}
}

void HistoryManagerWindow::changeEvent(QEvent *e)
{
	QWidget::changeEvent(e);
	switch (e->type()) {
	case QEvent::LanguageChange:
		setWindowTitle(tr("History manager"));
		break;
	default:
		break;
	}
}

}

