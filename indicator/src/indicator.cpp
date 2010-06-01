/****************************************************************************
 * indicator.cpp
 *  Copyright © 2010, Vsevolod Velichko <torkvema@gmail.com>.
 *  Licence: GPLv3 or later
 *
 ****************************************************************************
 *                                                                          *
 *   This library is free software; you can redistribute it and/or modify   *
 *   it under the terms of the GNU General Public License as published by   *
 *   the Free Software Foundation; either version 3 of the License, or      *
 *   (at your option) any later version.                                    *
 *                                                                          *
 ****************************************************************************/

#include <qindicateindicator.h>
#include "indicator.h"
#include <qutim/debug.h>
#include <qutim/icon.h>
#include <QDateTime>

IndicatorPlugin::IndicatorPlugin () :
	desktopName( QUTIM_DESKTOP_FILE )
{
}

void IndicatorPlugin::init ()
{
	qutim_sdk_0_3::ExtensionIcon icon("info");
	addAuthor(QT_TRANSLATE_NOOP("Author", "Vsevolod Velichko"),
			QT_TRANSLATE_NOOP("Task", "Developer"),
			QLatin1String("torkvema@gmail.com")
		 );
	setInfo(QT_TRANSLATE_NOOP("Plugin", "Indicator"),
			QT_TRANSLATE_NOOP("Plugin", "Ubuntu Indicator applet integration"),
			PLUGIN_VERSION(0, 0, 1, 0),
			icon
	       );

}

bool IndicatorPlugin::load ()
{
	qDebug() << "[Indicator] Plugin load";
	qDebug() << desktopName;
	indicateServer = QIndicate::Server::defaultInstance();
	indicateServer->setType("message.im");
	indicateServer->setDesktopFile(desktopName);
	indicateServer->show();
	connect(indicateServer, SIGNAL(serverDisplay()), SLOT(showMainWindow()));
	connect(qutim_sdk_0_3::ChatLayer::instance(), SIGNAL(sessionCreated(qutim_sdk_0_3::ChatSession*)), SLOT(onSessionCreated(qutim_sdk_0_3::ChatSession*)));

	//QImage icon = qutim_sdk_0_3::Icon("qutim").pixmap(64).toImage();
	return true;
}

bool IndicatorPlugin::unload ()
{
	qDebug() << "[Indicator] Plugin unload";
	disconnect(indicateServer, SIGNAL(serverDisplay()), this, SLOT(showMainWindow()));
	disconnect(qutim_sdk_0_3::ChatLayer::instance(), SIGNAL(sessionCreated(qutim_sdk_0_3::ChatSession*)), this, SLOT(onSessionCreated(qutim_sdk_0_3::ChatSession*)));
	qDeleteAll(sessionIndicators);

	return false;
}

void IndicatorPlugin::onSessionCreated(qutim_sdk_0_3::ChatSession *session)
{
	qDebug() << "[Indicator] onSessionCreated";
	if (sessionIndicators.contains(session))
		return;
	qDebug() << "[Indicator] List doesn't contain session. Adding indicator";
	QIndicate::Indicator *indicator = new QIndicate::Indicator;
	sessionIndicators.insert(session, indicator);
	
	connect(session, SIGNAL(destroyed(QObject*)), SLOT(onSessionDestroyed(QObject*)));
	connect(session, SIGNAL(unreadChanged(const qutim_sdk_0_3::MessageList&)), SLOT(onUnreadChanged(const qutim_sdk_0_3::MessageList&)));
	connect(session, SIGNAL(activated(bool)), SLOT(onSessionActivated(bool)));
	connect(indicator, SIGNAL(display(QIndicate::Indicator*)), SLOT(onIndicatorDisplay(QIndicate::Indicator*)), Qt::QueuedConnection);
}

void IndicatorPlugin::onSessionDestroyed(QObject* session)
{
	qDebug() << "[Indicator] onSessionDestroyed";
	HashIndicator::Iterator it = sessionIndicators.find(qobject_cast<qutim_sdk_0_3::ChatSession*>(session));
	if (it == sessionIndicators.end())
		return;
	sessionIndicators.erase(it);
	delete it.value();
}

void IndicatorPlugin::onSessionActivated(bool active)
{
	qDebug() << "[Indicator] onSessionActivated";
	if (!active)
		return;
	qutim_sdk_0_3::ChatSession *session = qobject_cast<qutim_sdk_0_3::ChatSession*>(sender());
	if (!session)
		return;
	QIndicate::Indicator *indicator = sessionIndicators.value(session);
	if (!indicator)
		return;
	indicator->hide();
}

void IndicatorPlugin::onUnreadChanged(const qutim_sdk_0_3::MessageList &messages)
{
	qDebug() << "[Indicator] onUnreadChanged";
	if (messages.isEmpty())
		return;
	qDebug() << "[Indicator] Message list isn't empty. Looking for session.";
	qutim_sdk_0_3::ChatSession* session = qobject_cast<qutim_sdk_0_3::ChatSession*>(sender());
	if (!session || session->isActive())
		return;
	qDebug() << "[Indicator] session exists and not active.";

	QIndicate::Indicator *indicator = sessionIndicators.value(session);
	if (!indicator)
		return;
	qDebug() << "[Indicator] Indicator has been found. Displaying.";
	QDateTime time = messages.last().time();
	QString name = session->getUnit()->title();
	qDebug() << "Setting indicator time: " << time;
	qDebug() << "Setting indicator name: " << name;
	indicator->setTimeProperty(time);
	indicator->setNameProperty(name);
	indicator->setDrawAttentionProperty(true);
	indicator->setCountProperty(messages.size());
	indicator->setIndicatorProperty("subtype", "im");
	indicator->setIndicatorProperty("sender", name);
	// TODO: ->setIconProperty(QImage)
	indicator->show();
}

void IndicatorPlugin::onIndicatorDisplay(QIndicate::Indicator* indicator)
{
	qDebug() << "[Indicator] onIndicatorDisplay";
	qutim_sdk_0_3::ChatSession* session = sessionIndicators.key(indicator);
	if (!sessionIndicators.contains(session))
		return;
	session->activate();
}

void IndicatorPlugin::showMainWindow()
{
	qDebug() << "[Indicator] showMainWindow";
	if (QObject *obj = qutim_sdk_0_3::getService("ContactList"))
		obj->metaObject()->invokeMethod(obj, "show");
}

QUTIM_EXPORT_PLUGIN(IndicatorPlugin)
