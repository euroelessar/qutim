/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2011 Vsevolod Velichko <torkvema@gmail.com>
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

#include "indicatorservice.h"
#include <qutim/debug.h>
#include <qutim/icon.h>
#include <qutim/servicemanager.h>
#include <QDateTime>
#include <QApplication>

IndicatorService::IndicatorService()
#ifdef QUANTAL
    : desktopName( QUTIM_DESKTOP_BASENAME )
    , indicateServer(new Server(desktopName, this))
    , quitButton(&indicateServer->createSourceString(QLatin1String("quitButton"), QT_TRANSLATE_NOOP("Plugin", "Close qutIM"), Ureen::Icon("application-exit"), ""))
#else /* QUANTAL */
	: desktopName( QUTIM_DESKTOP_FILE )
	, indicateServer(Server::defaultInstance())
	, quitButton(new Source)
#endif /* QUANTAL */
{
#ifdef QUANTAL
    indicateServer->registerApp();
	connect(indicateServer, SIGNAL(sourceActivated(MessagingMenu::Application::Source&)),
	        SLOT(onIndicatorDisplay(MessagingMenu::Application::Source&)));
#else /* QUANTAL */
	indicateServer->setType("message.im");
	indicateServer->setDesktopFile(desktopName);
	indicateServer->show();
	connect(indicateServer, SIGNAL(serverDisplay()), SLOT(showMainWindow()));
#endif /* QUANTAL */
	connect(Ureen::ChatLayer::instance(), SIGNAL(sessionCreated(Ureen::ChatSession*)), SLOT(onSessionCreated(Ureen::ChatSession*)));

	//QImage icon = Ureen::Icon("qutim").pixmap(64).toImage();

	qApp->setQuitOnLastWindowClosed(false);

	/* Quit Button */
#ifndef QUANTAL
	quitButton->setNameProperty(QT_TRANSLATE_NOOP("Plugin", "Close qutIM"));
	QImage icon = Ureen::Icon("application-exit").pixmap(64).toImage();
	connect(quitButton, SIGNAL(display(QIndicate::Indicator*)), qApp, SLOT(quit()));
	quitButton->setIconProperty(icon);
	quitButton->show();
#endif /* QUANTAL */
	foreach (Ureen::ChatSession *session, Ureen::ChatLayer::instance()->sessions())
		onSessionCreated(session);
}

IndicatorService::~IndicatorService()
{
	disconnect(Ureen::ChatLayer::instance(), SIGNAL(sessionCreated(Ureen::ChatSession*)),
	           this, SLOT(onSessionCreated(Ureen::ChatSession*)));
#ifdef QUANTAL
    //indicateServer->unregisterApp();
#else /* QUANTAL */
	quitButton->hide();
	delete quitButton;
	disconnect(indicateServer, SIGNAL(serverDisplay()), this, SLOT(showMainWindow()));
	qDeleteAll(sessionIndicators);
	indicateServer->hide();
	delete indicateServer;
#endif /* QUANTAL */
}

void IndicatorService::onSessionCreated(Ureen::ChatSession *session)
{
	Ureen::debug() << "onSessionCreated";
	if (sessionIndicators.contains(session))
		return;
	Ureen::debug() << "List doesn't contain session. Adding indicator";

#ifdef QUANTAL
    Source *indicator = &indicateServer->createSourceTime(
            QString::number(reinterpret_cast<quintptr>(session)),
            session->getUnit()->title(),
            QIcon(),
            QDateTime::currentDateTime(),
            true
            );
#else /* QUANTAL */
	Source *indicator = new Source;
#endif /* QUANTAL */

	sessionIndicators.insert(session, indicator);

	connect(session, SIGNAL(destroyed(QObject*)), SLOT(onSessionDestroyed(QObject*)));
	connect(session, SIGNAL(unreadChanged(const Ureen::MessageList&)),
	        SLOT(onUnreadChanged(const Ureen::MessageList&)));
	connect(session, SIGNAL(activated(bool)), SLOT(onSessionActivated(bool)));
#ifdef QUANTAL
    indicator->setAttention(!session->unread().isEmpty());
    indicator->setCount(session->unread().count());
    if (!session->unread().isEmpty())
        indicator->setIcon(Ureen::Icon("mail-unread-new"));
#else /* QUANTAL */
	connect(indicator, SIGNAL(display(QIndicate::Indicator*)),
	        SLOT(onIndicatorDisplay(MessagingMenu::Application::Source)), Qt::QueuedConnection);

	QString name = session->getUnit()->title();
	Ureen::debug() << "Setting indicator name: " << name;
	indicator->setNameProperty(name);
	indicator->setTimeProperty(QDateTime::currentDateTime());
	indicator->setDrawAttentionProperty(!session->unread().isEmpty());
	indicator->setCountProperty(session->unread().count());
	QImage icon;
	if (!session->unread().isEmpty())
		icon = Ureen::Icon("mail-unread-new").pixmap(64).toImage();
	indicator->setIconProperty(icon);
	indicator->setIndicatorProperty("subtype", "im");
	indicator->setIndicatorProperty("sender", name);
	indicator->show();
#endif /* QUANTAL */
}

void IndicatorService::onSessionDestroyed(QObject *session)
{
	Ureen::debug() << "onSessionDestroyed";
	Ureen::ChatSession *_session = static_cast<Ureen::ChatSession*>(session);
    Source *indicator = sessionIndicators.take(_session);
#ifdef QUANTAL
    indicateServer->removeSource(*indicator);
#else /* QUANTAL */
	delete indicator;
#endif /* QUANTAL */
}

void IndicatorService::onUnreadChanged(const Ureen::MessageList &messages)
{
	Ureen::debug() << "onUnreadChanged";
	if (messages.isEmpty())
		return;
	Ureen::debug() << "Message list isn't empty. Looking for session.";
	Ureen::ChatSession* session = qobject_cast<Ureen::ChatSession*>(sender());
	if (!session || session->isActive())
		return;
	Ureen::debug() << "session exists and not active.";

	Source *indicator = sessionIndicators.value(session);
	if (!indicator)
		return;
	QDateTime time = messages.last().time();
#ifdef QUANTAL
    indicator->setTime(time);
    indicator->setAttention(true);
    indicator->setIcon(Ureen::Icon("mail-unread-new"));
    indicator->setCount(session->unread().count());
#else /* QUANTAL */
	indicator->setTimeProperty(time);
	indicator->setDrawAttentionProperty(true);
	QImage icon = Ureen::Icon("mail-unread-new").pixmap(64).toImage();
	indicator->setIconProperty(icon);
	indicator->setCountProperty(session->unread().count());
	// TODO: ->setIconProperty(QImage)
	indicator->show();
#endif /* QUANTAL */
}

void IndicatorService::onAccountCreated(Ureen::Account *)
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
	Ureen::debug() << "onSessionActivated";
	if (!active)
		return;
	Ureen::ChatSession *session = qobject_cast<Ureen::ChatSession*>(sender());
	if (!session)
		return;
	Source *indicator = sessionIndicators.value(session);
	if (!indicator)
		return;
#ifdef QUANTAL
    indicator->setTime(QDateTime::currentDateTime());
    indicator->setAttention(false);
    indicator->setCount(session->unread().count());
    indicator->setIcon(QIcon());
#else /* QUANTAL */
	indicator->setTimeProperty(QDateTime::currentDateTime());
	indicator->setDrawAttentionProperty(false);
	indicator->setCountProperty(session->unread().count());
	indicator->setIconProperty(QImage());
	//indicator->hide();
#endif /* QUANTAL */
}

void IndicatorService::onIndicatorDisplay(SourceRef indicator)
{
	Ureen::debug() << "onIndicatorDisplay";
#ifdef QUANTAL
    if (indicator.getLabel() == QT_TRANSLATE_NOOP("Plugin", "Close qutIM"))
        qApp->quit();
	Ureen::ChatSession* session = sessionIndicators.key(&indicator);
#else /* QUANTAL */
	Ureen::ChatSession* session = sessionIndicators.key(indicator);
#endif /* QUANTAL */
	if (!sessionIndicators.contains(session))
		return;
	session->activate();
}

void IndicatorService::showMainWindow()
{
	Ureen::debug() << "showMainWindow";
	if (QObject *obj = Ureen::ServiceManager::getByName("ContactList"))
	{
		QMetaObject::invokeMethod(obj, "show");
		QWidget *objWidget = qobject_cast<QWidget*>(obj);
		if (objWidget)
			objWidget->raise();
	}
}

