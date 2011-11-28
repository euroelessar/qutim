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

#include <qindicateindicator.h>
#include "indicatorservice.h"
#include <qutim/debug.h>
#include <qutim/icon.h>
#include <qutim/servicemanager.h>
#include <QDateTime>
#include <QApplication>

IndicatorService::IndicatorService() :
	desktopName( QUTIM_DESKTOP_FILE ),
	indicateServer(QIndicate::Server::defaultInstance()),
	quitButton(new QIndicate::Indicator)
{
	indicateServer->setType("message.im");
	indicateServer->setDesktopFile(desktopName);
	indicateServer->show();
	connect(indicateServer, SIGNAL(serverDisplay()), SLOT(showMainWindow()));
	connect(qutim_sdk_0_3::ChatLayer::instance(), SIGNAL(sessionCreated(qutim_sdk_0_3::ChatSession*)), SLOT(onSessionCreated(qutim_sdk_0_3::ChatSession*)));

	//QImage icon = qutim_sdk_0_3::Icon("qutim").pixmap(64).toImage();
	
	qApp->setQuitOnLastWindowClosed(false);

	/* Quit Button */
	quitButton->setNameProperty(QT_TRANSLATE_NOOP("Plugin", "Close qutIM"));
	QImage icon = qutim_sdk_0_3::Icon("application-exit").pixmap(64).toImage();
	connect(quitButton, SIGNAL(display(QIndicate::Indicator*)), qApp, SLOT(quit()));
	quitButton->setIconProperty(icon);
	quitButton->show();
	foreach (qutim_sdk_0_3::ChatSession *session, qutim_sdk_0_3::ChatLayer::instance()->sessions())
		onSessionCreated(session);
}

IndicatorService::~IndicatorService()
{
	quitButton->hide();
	delete quitButton;
	disconnect(indicateServer, SIGNAL(serverDisplay()), this, SLOT(showMainWindow()));
	disconnect(qutim_sdk_0_3::ChatLayer::instance(), SIGNAL(sessionCreated(qutim_sdk_0_3::ChatSession*)),
	           this, SLOT(onSessionCreated(qutim_sdk_0_3::ChatSession*)));
	qDeleteAll(sessionIndicators);
	indicateServer->hide();
	delete indicateServer;
}

void IndicatorService::onSessionCreated(qutim_sdk_0_3::ChatSession *session)
{
	qutim_sdk_0_3::debug() << "[Indicator] onSessionCreated";
	if (sessionIndicators.contains(session))
		return;
	qutim_sdk_0_3::debug() << "[Indicator] List doesn't contain session. Adding indicator";
	QIndicate::Indicator *indicator = new QIndicate::Indicator;
	sessionIndicators.insert(session, indicator);
	
	connect(session, SIGNAL(destroyed(QObject*)), SLOT(onSessionDestroyed(QObject*)));
	connect(session, SIGNAL(unreadChanged(const qutim_sdk_0_3::MessageList&)),
	        SLOT(onUnreadChanged(const qutim_sdk_0_3::MessageList&)));
	connect(session, SIGNAL(activated(bool)), SLOT(onSessionActivated(bool)));
	connect(indicator, SIGNAL(display(QIndicate::Indicator*)),
	        SLOT(onIndicatorDisplay(QIndicate::Indicator*)), Qt::QueuedConnection);

	QString name = session->getUnit()->title();
	qutim_sdk_0_3::debug() << "Setting indicator name: " << name;
	indicator->setNameProperty(name);
	indicator->setTimeProperty(QDateTime::currentDateTime());
	indicator->setDrawAttentionProperty(!session->unread().isEmpty());
	indicator->setCountProperty(session->unread().count());
	QImage icon;
	if (!session->unread().isEmpty())
		icon = qutim_sdk_0_3::Icon("mail-unread-new").pixmap(64).toImage();
	indicator->setIconProperty(icon);
	indicator->setIndicatorProperty("subtype", "im");
	indicator->setIndicatorProperty("sender", name);
	indicator->show();
}

void IndicatorService::onSessionDestroyed(QObject *session)
{
	qutim_sdk_0_3::debug() << "[Indicator] onSessionDestroyed";
	qutim_sdk_0_3::ChatSession *_session = static_cast<qutim_sdk_0_3::ChatSession*>(session);
	delete sessionIndicators.take(_session);
}

void IndicatorService::onUnreadChanged(const qutim_sdk_0_3::MessageList &messages)
{
	qutim_sdk_0_3::debug() << "[Indicator] onUnreadChanged";
	if (messages.isEmpty())
		return;
	qutim_sdk_0_3::debug() << "[Indicator] Message list isn't empty. Looking for session.";
	qutim_sdk_0_3::ChatSession* session = qobject_cast<qutim_sdk_0_3::ChatSession*>(sender());
	if (!session || session->isActive())
		return;
	qutim_sdk_0_3::debug() << "[Indicator] session exists and not active.";

	QIndicate::Indicator *indicator = sessionIndicators.value(session);
	if (!indicator)
		return;
	QDateTime time = messages.last().time();
	indicator->setTimeProperty(time);
	indicator->setDrawAttentionProperty(true);
	QImage icon = qutim_sdk_0_3::Icon("mail-unread-new").pixmap(64).toImage();
	indicator->setIconProperty(icon);
	indicator->setCountProperty(session->unread().count());
	// TODO: ->setIconProperty(QImage)
	indicator->show();
}

void IndicatorService::onAccountCreated(qutim_sdk_0_3::Account *)
{
}

void IndicatorService::onAccountDestroyed(QObject *)
{
}

void IndicatorService::loadSettings()
{
}

void IndicatorService::onSessionActivated(bool active)
{
	qutim_sdk_0_3::debug() << "[Indicator] onSessionActivated";
	if (!active)
		return;
	qutim_sdk_0_3::ChatSession *session = qobject_cast<qutim_sdk_0_3::ChatSession*>(sender());
	if (!session)
		return;
	QIndicate::Indicator *indicator = sessionIndicators.value(session);
	if (!indicator)
		return;
	indicator->setTimeProperty(QDateTime::currentDateTime());
	indicator->setDrawAttentionProperty(false);
	indicator->setCountProperty(session->unread().count());
	indicator->setIconProperty(QImage());
	//indicator->hide();
}

void IndicatorService::onIndicatorDisplay(QIndicate::Indicator* indicator)
{
	qutim_sdk_0_3::debug() << "[Indicator] onIndicatorDisplay";
	qutim_sdk_0_3::ChatSession* session = sessionIndicators.key(indicator);
	if (!sessionIndicators.contains(session))
		return;
	session->activate();
}

void IndicatorService::showMainWindow()
{
	qutim_sdk_0_3::debug() << "[Indicator] showMainWindow";
	if (QObject *obj = qutim_sdk_0_3::ServiceManager::getByName("ContactList"))
	{
		QMetaObject::invokeMethod(obj, "show");
		QWidget *objWidget = qobject_cast<QWidget*>(obj);
		if (objWidget)
			objWidget->raise();
	}
}

