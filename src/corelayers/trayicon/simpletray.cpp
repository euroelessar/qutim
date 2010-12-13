#include "simpletray.h"
#include <qutim/icon.h>
#include <qutim/extensioninfo.h>
#include "qutim/metaobjectbuilder.h"
#include <qutim/servicemanager.h>
#include <QApplication>
#include <QWidgetAction>
#include <QToolButton>
#include <qutim/debug.h>

namespace Core
{
	class ProtocolSeparatorActionGenerator : public ActionGenerator
	{
	public:
		ProtocolSeparatorActionGenerator(Protocol *proto, const ExtensionInfo &info) :
				ActionGenerator(info.icon(), MetaObjectBuilder::info(info.generator()->metaObject(),"Protocol"), 0, 0)
		{
			setType(-1);
			m_proto = proto;
		}

		virtual QObject *generateHelper() const
		{
			if (m_proto->accounts().isEmpty())
				return NULL;
			QAction *action = prepareAction(new QAction(NULL));

			QFont font = action->font();
			font.setBold(true);
			action->setFont(font);
#ifndef Q_WS_MAC
			QToolButton *m_btn = new QToolButton();
			QWidgetAction *widget = new QWidgetAction(action);
			widget->setDefaultWidget(m_btn);			
			QObject::connect(action,SIGNAL(destroyed()),widget,SLOT(deleteLater()));
			m_btn->setDefaultAction(action);
			m_btn->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
			m_btn->setDown(true); // prevent hover style changes in some styles
			return widget;
#else
			return action;
#endif
		}
		
		virtual ~ProtocolSeparatorActionGenerator()
		{
		}
		
		void ensureVisibility() const
		{
			if (m_action)
				m_action->setVisible(!m_proto->accounts().isEmpty());
		}
	private:
		Protocol *m_proto;
		QToolButton *m_btn;
		mutable QPointer<QAction> m_action;
	};

	SimpleTray::SimpleTray()
	{
		if (!QSystemTrayIcon::isSystemTrayAvailable()) {
			debug() << "No System Tray Available. Tray icon not loaded.";
			return;
		}
		m_iconTimerId = 0;
		m_activeAccount = 0;
		m_isMail = false;
		m_icon = new QSystemTrayIcon(this);
		m_icon->setIcon(m_currentIcon = Icon("qutim"));
		m_mailIcon = Icon("mail-unread-new");
		m_icon->show();
		connect(m_icon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
				this, SLOT(onActivated(QSystemTrayIcon::ActivationReason)));
		connect(ChatLayer::instance(), SIGNAL(sessionCreated(qutim_sdk_0_3::ChatSession*)),
				this, SLOT(onSessionCreated(qutim_sdk_0_3::ChatSession*)));
		QMap<QString, Protocol*> protocols;
		foreach (Protocol *proto, Protocol::all()) {
			protocols.insert(proto->id(), proto);
			connect(proto, SIGNAL(accountCreated(qutim_sdk_0_3::Account*)),
					this, SLOT(onAccountCreated(qutim_sdk_0_3::Account*)));
		}
		m_protocols = protocols.values();

		for (int i = 0; i < m_protocols.size(); i++) {
			ExtensionInfo info = m_protocols.at(i)->property("extensioninfo").value<ExtensionInfo>();
			ProtocolSeparatorActionGenerator *gen =
					new ProtocolSeparatorActionGenerator(m_protocols.at(i), info);
			m_protocolActions.append(gen);
			gen->setPriority(1 - i * 2);
			addAction(gen);
		}

		setMenuOwner(qobject_cast<MenuController*>(ServiceManager::getByName("ContactList")));
		m_icon->setContextMenu(menu());
		qApp->setQuitOnLastWindowClosed(false);
	}

	void SimpleTray::onActivated(QSystemTrayIcon::ActivationReason reason)
	{
		if (reason == QSystemTrayIcon::Trigger) {
			if (m_sessions.isEmpty()) {
				if (QObject *obj = ServiceManager::getByName("ContactList"))
					obj->metaObject()->invokeMethod(obj, "changeVisibility");
			} else {
				m_sessions.first()->activate();
			}
		}
	}

	void SimpleTray::onSessionCreated(qutim_sdk_0_3::ChatSession *session)
	{
		connect(session, SIGNAL(unreadChanged(qutim_sdk_0_3::MessageList)),
				this, SLOT(onUnreadChanged(qutim_sdk_0_3::MessageList)));
		connect(session, SIGNAL(destroyed()), this, SLOT(onSessionDestroyed()));
	}

	void SimpleTray::onSessionDestroyed()
	{
		onUnreadChanged(MessageList());
	}

	void SimpleTray::onUnreadChanged(qutim_sdk_0_3::MessageList unread)
	{
		ChatSession *session = static_cast<ChatSession*>(sender());
		Q_ASSERT(session != NULL);
		MessageList::iterator itr = unread.begin();
		while (itr != unread.end()) {
			if (itr->property("silent", false))
				itr = unread.erase(itr);
			else
				++itr;
		}
		bool empty = m_sessions.isEmpty();
		if (unread.isEmpty()) {
			m_sessions.removeOne(session);
		} else if (!m_sessions.contains(session)) {
			m_sessions.append(session);
		} else {
			return;
		}
		if (empty == m_sessions.isEmpty())
			return;
		if (m_sessions.isEmpty()) {
			if (m_iconTimerId) {
				killTimer(m_iconTimerId);
				m_iconTimerId = 0;
			}
			m_icon->setIcon(m_currentIcon);
			m_isMail = false;
		} else {
			if (!m_iconTimerId)
				m_iconTimerId = startTimer(500);
			m_icon->setIcon(m_mailIcon);
			m_isMail = true;
		}
	}

	void SimpleTray::timerEvent(QTimerEvent *timer)
	{
		if (timer->timerId() != m_iconTimerId) {
			QObject::timerEvent(timer);
		} else {
			m_icon->setIcon(m_isMail ? m_mailIcon : m_currentIcon);
			m_isMail = !m_isMail;
		}
	}

	void StatusAction::onStatusChanged(Status status)
	{
		setIcon(status.icon());
	}

	StatusAction::StatusAction(QObject* parent): QAction(parent)
	{

	}


	class AccountMenuActionGenerator : public ActionGenerator
	{
	public:
		AccountMenuActionGenerator(Account *account) :
				ActionGenerator(QIcon(), account->id(), 0, 0)
		{
			m_account = account;
			setType(-1);
		}

		virtual QObject *generateHelper() const
		{
			StatusAction *action = new StatusAction(0);
			prepareAction(action);
			action->setIcon(m_account->status().icon());
			QMenu *menu = m_account->menu(false);
			QObject::connect(action, SIGNAL(destroyed()), menu, SLOT(deleteLater()));
			QObject::connect(m_account, SIGNAL(statusChanged(qutim_sdk_0_3::Status,qutim_sdk_0_3::Status)),
							 action, SLOT(onStatusChanged(qutim_sdk_0_3::Status)));
			action->setMenu(menu);
			return action;
		}
	private:
		Account *m_account;
	};

	void SimpleTray::onAccountDestroyed(QObject *obj)
	{
		ActionGenerator *gen = m_actions.take(static_cast<Account*>(obj));
		removeAction(gen);
		delete gen;
		validateProtocolActions();
	}

	void SimpleTray::onAccountCreated(qutim_sdk_0_3::Account *account)
	{
		if (m_actions.contains(account))
			return;
		m_accounts << account;
		ActionGenerator *gen = new AccountMenuActionGenerator(account);
		gen->setPriority(- m_protocols.indexOf(account->protocol()) * 2);
		m_actions.insert(account, gen);
		addAction(gen);
		connect(account, SIGNAL(destroyed(QObject*)), this, SLOT(onAccountDestroyed(QObject*)));
		connect(account, SIGNAL(statusChanged(qutim_sdk_0_3::Status,qutim_sdk_0_3::Status)),
				this, SLOT(onStatusChanged(qutim_sdk_0_3::Status)));
		if (!m_activeAccount) {
			if (account->status().type() != Status::Offline)
				m_activeAccount = account;
			m_currentIcon = account->status().icon();
			if (!m_isMail)
				m_icon->setIcon(m_currentIcon);
		}
		validateProtocolActions();
	}

	void SimpleTray::onStatusChanged(const qutim_sdk_0_3::Status &status)
	{
		Account *account = qobject_cast<Account*>(sender());
		if (account == m_activeAccount || !m_activeAccount) {
			m_activeAccount = account;
			if (account->status().type() == Status::Offline) {
				m_activeAccount = 0;
			}
			m_currentIcon = status.icon();
		}
		if (!m_activeAccount) {
			foreach (Account *acc, m_accounts) {
				if (acc->status().type() != Status::Offline) {
					m_activeAccount = acc;
					m_currentIcon = acc->status().icon();
					break;
				}
			}
		}
		if (!m_isMail)
			m_icon->setIcon(m_currentIcon);
	}
	
	void SimpleTray::validateProtocolActions()
	{
		foreach (ProtocolSeparatorActionGenerator *gen, m_protocolActions)
			gen->ensureVisibility();
	}
}
