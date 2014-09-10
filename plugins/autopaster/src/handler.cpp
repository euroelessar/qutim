/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2013 Tretyakov Roman <roman@trett.ru>
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

#include "handler.h"
//#include "autopasterdialog.h"
#include "ubuntupaster.h"
#include "hastebinpaster.h"
#include "kdepaster.h"

#include <QNetworkReply>
#include <QTimer>
#include <QCoreApplication>

#include <qutim/config.h>
#include <qutim/chatsession.h>
#include <qutim/json.h>

using namespace qutim_sdk_0_3;

AutoPasterHandler::AutoPasterHandler() :
	m_dialog(QStringLiteral("autopaster")),
	m_settings(QStringLiteral("autopaster"),
			   Settings::Plugin,
			   QIcon(),
			   QT_TRANSLATE_NOOP("Plugin", "AutoPaster"))
{
	addPaster(new UbuntuPaster);
	addPaster(new HastebinPaster);
	addPaster(new KdePaster);

	readSettings();

	registerHandler(this,
					QLatin1String("AutoPaster"),
					qutim_sdk_0_3::MessageHandler::NormalPriortity,
					qutim_sdk_0_3::MessageHandler::SenderPriority + 0x2000);

	Settings::registerItem(&m_settings);

	m_settings.connect(SIGNAL(saved()), this, SLOT(readSettings()));
}

AutoPasterHandler::~AutoPasterHandler()
{
	Settings::removeItem(&m_settings);

	qDeleteAll(m_pasters);
}

void AutoPasterHandler::addPaster(PasterInterface *paster)
{
	m_pasters << paster;
}

QStringList AutoPasterHandler::pasters()
{
	QStringList result;

	for (PasterInterface *paster : m_pasters)
		result << paster->name();

	return result;
}

QVariantList AutoPasterHandler::syntaxes()
{
	QVariantList result;

	const std::initializer_list<QPair<QString, QString>> syntaxes = {
		{ QStringLiteral("Plain Text"), QStringLiteral("text") },
		{ QStringLiteral("C++"), QStringLiteral("cpp") },
		{ QStringLiteral("Bash"), QStringLiteral("bash") },
		{ QStringLiteral("Perl"), QStringLiteral("perl") },
		{ QStringLiteral("PHP"), QStringLiteral("php") },
		{ QStringLiteral("C#"), QStringLiteral("csharp") },
		{ QStringLiteral("HTML"), QStringLiteral("html") },
		{ QStringLiteral("JavaScript"), QStringLiteral("js") },
		{ QStringLiteral("Java"), QStringLiteral("java") },
		{ QStringLiteral("Makefile"), QStringLiteral("make") },
		{ QStringLiteral("XML"), QStringLiteral("xml") },
		{ QStringLiteral("CSS"), QStringLiteral("css") },
	};

	for (const QPair<QString, QString> &pair : syntaxes) {
		result << QVariantMap({
			{ QStringLiteral("text"), pair.first },
			{ QStringLiteral("value"), pair.second },
		});
	}

	return result;
}

MessageHandlerAsyncResult AutoPasterHandler::doHandle(Message &message)
{
	if (!message.isIncoming()
			&& !message.property("service", false)
			&& !message.property("history", false)
			&& message.text().count('\n') + 1 >= m_lineCount) {

		QueueItem item = {
			MessageHandlerAsyncResult::Handler(),
			&message
		};

		if (m_autoSubmit) {
			upload(item, m_pasters.value(m_defaultLocation), QStringLiteral("text"));
		} else {
			m_queue << item;

			qDebug() << "Added item to queue, size:" << m_queue.size();

			if (m_queue.size() == 1) {
				emit messageReceived();
				m_dialog.show();
			}
		}

		return item.handler.result();
	} else {
		return makeAsyncResult(Accept, QString());
	}
}

void AutoPasterHandler::readSettings()
{
	Config cfg;
	cfg.beginGroup("autoPaster");
	m_autoSubmit = cfg.value(QLatin1String("autoSubmit"), false);
	m_defaultLocation = qBound(0, cfg.value(QLatin1String("defaultLocation"), 0), m_pasters.size() - 1);
	m_lineCount = cfg.value(QLatin1String("lineCount"), 5);
}

void AutoPasterHandler::upload(QueueItem item, PasterInterface *paster, const QString &syntax)
{
	QNetworkReply *reply = paster->send(&m_manager, item.message->text(), syntax);

	connect(reply, &QNetworkReply::finished, this, [item, paster, reply] () {
		reply->deleteLater();

		QString errorString;

		if (reply->error() == QNetworkReply::NoError) {
			QString url = paster->handle(reply, &errorString).toString();

			if (errorString.isEmpty()) {
				item.message->setText(url);
				item.handler.handle(Accept, QString());
			}
		} else {
			errorString = reply->errorString();
		}

		const QString reason = QCoreApplication::translate(
								   "AutoPaster",
								   "Failed to send message to paste service, service reported error: %1")
							   .arg(errorString);
		item.handler.handle(Error, reason);
	});
}

void AutoPasterHandler::upload(const QString &pasterName, const QString &syntax)
{
	Q_ASSERT(!m_queue.isEmpty());

	QueueItem item = m_queue.takeFirst();

	PasterInterface *paster = nullptr;
	for (PasterInterface *interface : m_pasters) {
		if (interface->name() == pasterName)
			paster = interface;
	}

	Q_ASSERT(paster);

	upload(item, paster, syntax);
}

void AutoPasterHandler::cancel()
{
	Q_ASSERT(!m_queue.isEmpty());

	QueueItem item = m_queue.takeFirst();
	item.handler.handle(Accept, QString());
}
