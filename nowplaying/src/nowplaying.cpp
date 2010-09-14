#include "nowplaying.h"
#include "icqsupport.h"
#include "jabbersupport.h"
#include "players/amarok/amarok.h"
#include <qutim/objectgenerator.h>
#include <qutim/settingslayer.h>
#include <qutim/debug.h>
#include <qutim/event.h>
#include <QVariantMap>

namespace qutim_sdk_0_3
{

namespace nowplaying
{

	NowPlaying *NowPlaying::self;

	NowPlaying::NowPlaying() :
		m_player(0)
	{
		Q_ASSERT(!self);
		self = this;
	}

	void NowPlaying::init()
	{
		setInfo(QT_TRANSLATE_NOOP("Plugin", "Now Playing"),
				QT_TRANSLATE_NOOP("Plugin", "Now playing plugin"),
				PLUGIN_VERSION(0, 1, 0, 0));
		addAuthor(QT_TRANSLATE_NOOP("Author","Kazlauskas Ian"),
				  QT_TRANSLATE_NOOP("Task","Developer"),
				  QLatin1String("nayzak90@googlemail.com"));
		addAuthor(QT_TRANSLATE_NOOP("Author","Alexey Prokhin"),
				  QT_TRANSLATE_NOOP("Task","Developer"),
				  QLatin1String("alexey.prokhin@yandex.ru"));
		addExtension(QT_TRANSLATE_NOOP("Plugin", "Now Playing Amarok 2"),
					 QT_TRANSLATE_NOOP("Plugin", "Amarok 2 support for now playing plugin"),
					 new GeneralGenerator<Amarok, Player>(),
					 ExtensionIcon(""));
	}

	bool NowPlaying::load()
	{
		Protocol *protocol = allProtocols().value("icq");
		if (protocol)
			m_factories.insert(protocol, new IcqTuneStatus);
		protocol = allProtocols().value("jabber");
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

		foreach(const ObjectGenerator *gen, moduleGenerators<Player>()) {
			Player* player =  gen->generate<Player>();
			m_players.insert(player->playerName(), player);
		}
		loadSettings();

		SettingsItem* settings = new GeneralSettingsItem<SettingsUI>(
				Settings::Plugin,
				QIcon(":images/images/logo.png"),
				QT_TRANSLATE_NOOP("NowPlaying", "Now Playing"));
		Settings::registerItem(settings);

		m_stopStartAction = new StopStartActionGenerator(this, m_isWorking);
		MenuController *contactList = getService<MenuController*>("ContactList");
		if (contactList)
			contactList->addAction(m_stopStartAction);
		return true;
	}

	bool NowPlaying::unload()
	{
		clearStatuses();
		return true;
	}

	void NowPlaying::loadSettings()
	{
		Config cfg = config("global");
		m_isWorking = cfg.value("isWorking", false);
		foreach (AccountTuneStatus *account, m_accounts)
			account->loadSettings();
		foreach (AccountTuneStatus *factory, m_factories)
			factory->loadSettings();
		initPlayer(cfg.value("player", QString("Amarok")));
		m_forAllAccounts = cfg.value("enableForAllAccounts", true);
	}

	void NowPlaying::initPlayer(const QString &playerName)
	{
		if (m_player) {
			if (playerName != m_player->playerName())
				return;
			disconnect(m_player, 0, this, 0);
			m_player->stopWatching();
		}
		m_player = m_players.value(playerName);
		if (m_player != 0) {
			connect(m_player, SIGNAL(playingStatusChanged(bool)), SLOT(playingStatusChanged(bool)));
			connect(m_player, SIGNAL(trackChanged(TrackInfo)), SLOT(setStatuses(TrackInfo)));
			m_player->init();
			if (m_isWorking) {
				if (m_player->isPlaying())
					setStatuses(m_player->trackInfo());
				else
					clearStatuses();
			}
		} else if (m_isWorking) {
			setState(false);
		}
	}

	void NowPlaying::setState(bool isWorking)
	{
		Q_ASSERT(!(!m_player && m_isWorking));
		if (m_isWorking == isWorking || !m_player)
			return;
		m_isWorking = isWorking;
		m_stopStartAction->setState(isWorking);
		config("global").setValue("isWorking", isWorking);
		if (isWorking) {
			m_player->startWatching();
			if (m_player->isPlaying())
				setStatuses(m_player->trackInfo());
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
			setStatuses(m_player->trackInfo());
	}

	void NowPlaying::setStatuses(const TrackInfo &info)
	{
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
		connect(account, SIGNAL(destroyed()), SLOT(accountDeleted()));
	}

	void NowPlaying::accountDeleted()
	{
		Account* account = reinterpret_cast<Account*>(sender());
		QList<AccountTuneStatus*>::iterator itr = m_accounts.begin();
		QList<AccountTuneStatus*>::iterator endItr = m_accounts.end();
		while (itr != endItr) {
			if ((*itr)->account() == account) {
				(*itr)->deleteLater();
				m_accounts.erase(itr);
				break;
			}
		}
	}

	StopStartActionGenerator::StopStartActionGenerator(QObject* module, bool isWorking):
			ActionGenerator(QIcon(":images/images/logo.png"),
							LocalizedString(),
							module,
							SLOT(stopStartActionTrigged()))
	{
		setState(isWorking);
	}

	void StopStartActionGenerator::showImpl(QAction *action, QObject* /*obj*/)
	{
		action->setText(m_text);
	}

	void StopStartActionGenerator::setState(bool isWorking)
	{
		m_text = isWorking ?
				 QT_TRANSLATE_NOOP("NowPlaying",  "Stop now playing") :
				 QT_TRANSLATE_NOOP("NowPlaying", "Start now playing");
	}

} }

QUTIM_EXPORT_PLUGIN(qutim_sdk_0_3::nowplaying::NowPlaying)
