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
    : desktopName( QUTIM_DESKTOP_BASENAME )
    , indicateServer(new Server(desktopName, this))
    , quitButton(&indicateServer->createSourceString(QLatin1String("quitButton"), QT_TRANSLATE_NOOP("Plugin", "Close qutIM"), qutim_sdk_0_3::Icon("application-exit"), "", true, 100))
{
    indicateServer->registerApp();
	connect(indicateServer, SIGNAL(sourceActivated(MessagingMenu::Application::Source&)),
	        SLOT(onIndicatorDisplay(MessagingMenu::Application::Source&)));
	connect(qutim_sdk_0_3::ChatLayer::instance(), SIGNAL(sessionCreated(qutim_sdk_0_3::ChatSession*)), SLOT(onSessionCreated(qutim_sdk_0_3::ChatSession*)));

	//QImage icon = qutim_sdk_0_3::Icon("qutim").pixmap(64).toImage();

	qApp->setQuitOnLastWindowClosed(false);

	foreach (qutim_sdk_0_3::ChatSession *session, qutim_sdk_0_3::ChatLayer::instance()->sessions())
		onSessionCreated(session);
}

IndicatorService::~IndicatorService()
{
	disconnect(qutim_sdk_0_3::ChatLayer::instance(), SIGNAL(sessionCreated(qutim_sdk_0_3::ChatSession*)),
	           this, SLOT(onSessionCreated(qutim_sdk_0_3::ChatSession*)));
}

void IndicatorService::onSessionCreated(qutim_sdk_0_3::ChatSession *session)
{
	qDebug() << "onSessionCreated";
	if (sessionIndicators.contains(session))
		return;
	qDebug() << "List doesn't contain session. Adding indicator";

    Source *indicator = &indicateServer->createSourceTime(
            QString::number(reinterpret_cast<quintptr>(session)),
            session->getUnit()->title(),
            QIcon(),
            QDateTime::currentDateTime(),
            true
            );

	sessionIndicators.insert(session, indicator);

	connect(session, SIGNAL(destroyed(QObject*)), SLOT(onSessionDestroyed(QObject*)));
	connect(session, SIGNAL(unreadChanged(const qutim_sdk_0_3::MessageList&)),
	        SLOT(onUnreadChanged(const qutim_sdk_0_3::MessageList&)));
	connect(session, SIGNAL(activated(bool)), SLOT(onSessionActivated(bool)));

    indicator->setAttention(!session->unread().isEmpty());
    indicator->setCount(session->unread().count());
    if (!session->unread().isEmpty())
        indicator->setIcon(qutim_sdk_0_3::Icon("mail-unread-new"));
}

void IndicatorService::onSessionDestroyed(QObject *session)
{
	qDebug() << "onSessionDestroyed";
	qutim_sdk_0_3::ChatSession *_session = static_cast<qutim_sdk_0_3::ChatSession*>(session);
    Source *indicator = sessionIndicators.take(_session);

    indicateServer->removeSource(*indicator);

}

void IndicatorService::onUnreadChanged(const qutim_sdk_0_3::MessageList &messages)
{
	qDebug() << "onUnreadChanged";
	if (messages.isEmpty())
		return;
	qDebug() << "Message list isn't empty. Looking for session.";
	qutim_sdk_0_3::ChatSession* session = qobject_cast<qutim_sdk_0_3::ChatSession*>(sender());
	if (!session || session->isActive())
		return;
	qDebug() << "session exists and not active.";

	Source *indicator = sessionIndicators.value(session);
	if (!indicator)
		return;
	QDateTime time = messages.last().time();

    indicator->setTime(time);
    indicator->setAttention(true);
    indicator->setIcon(qutim_sdk_0_3::Icon("mail-unread-new"));
    indicator->setCount(session->unread().count());

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
	qDebug() << "onSessionActivated";
	qutim_sdk_0_3::ChatSession *session = qobject_cast<qutim_sdk_0_3::ChatSession*>(sender());
	if (!session)
		return;
	Source *indicator = sessionIndicators.value(session);
	if (!indicator)
		return;

    indicator->setTime(QDateTime::currentDateTime());
    indicator->setAttention(false);
    indicator->setCount(session->unread().count());
    indicator->setIcon(QIcon());

}

void IndicatorService::onIndicatorDisplay(SourceRef indicator)
{
	qDebug() << "onIndicatorDisplay";

    if (indicator.getLabel() == QT_TRANSLATE_NOOP("Plugin", "Close qutIM"))
    {
        qApp->quit();
        return;
    }
	qutim_sdk_0_3::ChatSession* session = sessionIndicators.key(&indicator);

	if (!sessionIndicators.contains(session))
		return;
	session->activate();
}

void IndicatorService::showMainWindow()
{
	qDebug() << "showMainWindow";
	if (QObject *obj = qutim_sdk_0_3::ServiceManager::getByName("ContactList"))
	{
		QMetaObject::invokeMethod(obj, "show");
		QWidget *objWidget = qobject_cast<QWidget*>(obj);
		if (objWidget)
			objWidget->raise();
	}
}

