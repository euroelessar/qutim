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
#include "nowplaying.h"
#include "icqsupport.h"
#include "jabbersupport.h"
#include "players/mpris/mprisplayerfactory.h"
#include <qutim/objectgenerator.h>
#include <qutim/settingslayer.h>
#include <qutim/debug.h>
#include <qutim/event.h>
#include <qutim/servicemanager.h>
#include <QVariantMap>

namespace qutim_sdk_0_3
{

namespace nowplaying
{

NowPlaying *NowPlaying::self;

NowPlaying::NowPlaying() :
	m_player(0), m_isWorking(false)
{
	Q_ASSERT(!self);
	self = this;
}

void NowPlaying::init()
{
	setInfo(QT_TRANSLATE_NOOP("Plugin", "Now Playing"),
			QT_TRANSLATE_NOOP("Plugin", "Now playing plugin"),
			PLUGIN_VERSION(0, 1, 0, 0));
	setCapabilities(Loadable);
	addAuthor(QT_TRANSLATE_NOOP("Author","Kazlauskas Ian"),
			  QT_TRANSLATE_NOOP("Task","Developer"),
			  QLatin1String("nayzak90@googlemail.com"));
	addAuthor(QT_TRANSLATE_NOOP("Author","Alexey Prokhin"),
			  QT_TRANSLATE_NOOP("Task","Developer"),
			  QLatin1String("alexey.prokhin@yandex.ru"));
	addAuthor(QLatin1String("euroelessar"));
	addExtension(QT_TRANSLATE_NOOP("Plugin", "Now Playing Mpris"),
				 QT_TRANSLATE_NOOP("Plugin", "Mpris support for now playing plugin"),
				 new GeneralGenerator<MprisPlayerFactory, PlayerFactory>(),
				 ExtensionIcon(""));
}

bool NowPlaying::load()
{
	Protocol *protocol = Protocol::all().value("icq");
	if (protocol)
		m_factories.insert(protocol, new IcqTuneStatus);
	protocol =  Protocol::all().value("jabber");
	if (protocol)
		m_factories.insert(protocol, new JabberTuneStatus);
	if (m_factories.isEmpty())
		return false;

	QHashIterator<Protocol*, AccountTuneStatus*> itr(m_factories);
	while (itr.hasNext()) {
		itr.next();
		foreach (Account *account, itr.key()->accounts())
			accountCreated(account);
		connect(itr.key(), SIGNAL(accountCreated(qutim_sdk_0_3::Account*)),
				SLOT(accountCreated(qutim_sdk_0_3::Account*)));
	}

	foreach(const ObjectGenerator *gen, ObjectGenerator::module<PlayerFactory>()) {
		QObject *obj = gen->generate();
		obj->installEventFilter(this);
		m_playerFactories.append(obj);
	}

	SettingsItem* settings = new GeneralSettingsItem<SettingsUI>(
				Settings::Plugin,
				QIcon(":images/images/logo.png"),
				QT_TRANSLATE_NOOP("NowPlaying", "Now Playing"));
	Settings::registerItem(settings);

	m_stopStartAction = new StopStartActionGenerator(this, false, false);
	MenuController *contactList = ServiceManager::getByName<MenuController*>("ContactList");
	if (contactList)
		contactList->addAction(m_stopStartAction);
	loadSettings();
	return true;
}

bool NowPlaying::unload()
{
	clearStatuses();
	return true;
}

bool NowPlaying::eventFilter(QObject *obj, QEvent *ev)
{
	static const quint16 playerId = PlayerEvent::eventId();
	if (ev->type() != Event::eventType())
		return Plugin::eventFilter(obj, ev);
	if (obj == m_player) {
		static const quint16 trackInfoId = TrackInfoEvent::eventId();
		static const quint16 stateId = StateEvent::eventId();
		if (static_cast<Event*>(ev)->id == trackInfoId) {
			setStatuses(static_cast<TrackInfoEvent*>(ev)->trackInfo());
			return true;
		} else if (static_cast<Event*>(ev)->id == stateId) {
			playingStatusChanged(static_cast<StateEvent*>(ev)->isPlaying());
			return true;
		}
		return false;
	} else if (static_cast<Event*>(ev)->id == playerId) {
		PlayerEvent *playerEvent = static_cast<PlayerEvent*>(ev);
		if (playerEvent->playerId() == m_playerId) {
			if (!m_player && playerEvent->playerInfo() == PlayerEvent::Available) {
				initPlayer(m_playerId, qobject_cast<PlayerFactory*>(obj));
			} else if (m_player && playerEvent->playerInfo() == PlayerEvent::Unavailable) {
				m_player.object()->deleteLater();
				m_player = 0;
			}
		}
		return true;
	}
	return Plugin::eventFilter(obj, ev);
}

void NowPlaying::loadSettings()
{
	Config cfg = config("global");
	foreach (AccountTuneStatus *account, m_accounts)
		account->loadSettings();
	foreach (AccountTuneStatus *factory, m_factories)
		factory->loadSettings();
	initPlayer(cfg.value("player", QString("amarok")));
	if (!m_player)
		m_isWorking = false;
	else
		m_isWorking = cfg.value("isWorking", false);
	m_forAllAccounts = cfg.value("enableForAllAccounts", true);
	m_stopStartAction->setState(m_isWorking, !!m_player);
}

void NowPlaying::initPlayer(const QString &playerId, PlayerFactory *factory)
{
	if (m_player) {
			if (playerId == m_playerId)
			return;
		m_player.object()->removeEventFilter(this);
		m_player->stopWatching();
		m_player.object()->deleteLater();
		m_player = 0;
	}
	m_playerId = playerId;
	if (!factory || !(m_player = factory->player(playerId))) {
		foreach (PlayerFactory *factory, m_playerFactories) {
			m_player = factory->player(playerId);
			if (m_player)
				break;
		}
	}

	if (m_player) {
		m_player.object()->installEventFilter(this);
		m_player->init();
		if (m_isWorking) {
			m_player->startWatching();
			m_player->requestState();
			//				if (m_player->isPlaying())
			//					setStatuses(m_player->trackInfo());
			//				else
			//					clearStatuses();
		}
	} else if (m_isWorking) {
		setState(false);
	}
}

void NowPlaying::setState(bool isWorking)
{
	if (m_isWorking == isWorking)
		return;
	m_isWorking = isWorking;
	m_stopStartAction->setState(isWorking, !!m_player);
	config("global").setValue("isWorking", isWorking);
	if (!m_player)
		return;
	if (isWorking) {
		m_player->startWatching();
		m_player->requestState();
	} else {
		m_player->stopWatching();
		clearStatuses();
	}

}

void NowPlaying::stopStartActionTrigged()
{
	setState(!m_isWorking);
}

void NowPlaying::playingStatusChanged(bool isPlaying)
{
	if(!isPlaying)
		clearStatuses();
	else
		m_player->requestTrackInfo();
}

void NowPlaying::setStatuses(const TrackInfo &info)
{
	debug() << info.location.toString();
	foreach (AccountTuneStatus *account, m_accounts)
		account->setStatus(info);
}

void NowPlaying::clearStatuses()
{
	foreach (AccountTuneStatus *account, m_accounts)
		account->removeStatus();
}

void NowPlaying::accountCreated(qutim_sdk_0_3::Account *account)
{
	AccountTuneStatus *factory = m_factories.value(account->protocol());
	if (!factory)
		return;
	AccountTuneStatus *accountTune = factory->construct(account, factory);
	m_accounts << accountTune;
	accountTune->loadSettings();
	connect(account, SIGNAL(destroyed(QObject*)), SLOT(accountDeleted(QObject*)));
}

void NowPlaying::accountDeleted(QObject *obj)
{
	Account* account = reinterpret_cast<Account*>(obj);
	QList<AccountTuneStatus*>::iterator itr = m_accounts.begin();
	QList<AccountTuneStatus*>::iterator endItr = m_accounts.end();
	while (itr != endItr) {
		if ((*itr)->account() == account) {
			(*itr)->deleteLater();
			m_accounts.erase(itr);
			break;
		}
		itr++;
	}
}

StopStartActionGenerator::StopStartActionGenerator(QObject* module, bool isWorking, bool isEnabled):
    ActionGenerator(QIcon(":images/images/logo.png"),
                    LocalizedString(),
                    module,
                    SLOT(stopStartActionTrigged())),
    m_isEnabled(isEnabled)
{
    setCheckable(true);
    setState(isWorking, isEnabled);
}

void StopStartActionGenerator::showImpl(QAction *action, QObject* /*obj*/)
{
	action->setText(m_text);
	action->setToolTip(m_text);
//	action->setEnabled(m_isEnabled);
}

void StopStartActionGenerator::setState(bool isWorking, bool isEnabled)
{
	Q_UNUSED(isEnabled);
	setChecked(isWorking);
	m_text = isWorking ?
				QT_TRANSLATE_NOOP("NowPlaying",  "Stop now playing") :
				QT_TRANSLATE_NOOP("NowPlaying", "Start now playing");
	foreach (QAction *action, actions()) {
		action->setChecked(isWorking);
//		action->setEnabled(isEnabled);
	}
}

} }

QUTIM_EXPORT_PLUGIN(qutim_sdk_0_3::nowplaying::NowPlaying)

