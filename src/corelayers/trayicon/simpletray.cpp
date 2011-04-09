#include "simpletray.h"
#include "simpletraysettings.h"
#include <qutim/extensioninfo.h>
#include "qutim/metaobjectbuilder.h"
#include <qutim/servicemanager.h>
#include <QApplication>
#include <QWidgetAction>
#include <QToolButton>
#include <QDateTime>
#include <QPainter>
#include <QPixmap>
#include <QString>
#include <qutim/debug.h>
#include <qutim/settingslayer.h>

namespace Core
{

static QIcon iconForStatus(const Status &status)
{
	return Icon("qutim");
	//TODO create icon
	//if (status.type() == Status::Offline)
	//	return Icon("qutim-offline");
	//else if (status.type() == Status::Connecting)
	//	return Icon("qutim-offline");
	//else
	//	return Icon("qutim-online");
}

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

#ifdef Q_WS_WIN
class ClEventFilter : public QObject
{
	SimpleTray *tray_;
	QWidget *widget_;

public:
	ClEventFilter(SimpleTray *t, QWidget *w)
		: QObject(t)
	{
		tray_   = t;
		widget_ = w;
	}

	bool eventFilter(QObject *obj, QEvent *ev)
	{
		if (QEvent::ActivationChange == ev->type() && obj == widget_) {
			if (!widget_->isActiveWindow())
				tray_->clActivationStateChanged(widget_->isActiveWindow());
		}
		return false;
	}
};
#endif

SimpleTray::SimpleTray()
{
	if (!QSystemTrayIcon::isSystemTrayAvailable()) {
		qDebug() << "No System Tray Available. Tray icon not loaded.";
		return;
	}
	m_activeAccount = 0;
	m_isMail = false;
	m_icon = new QSystemTrayIcon(this);
	m_icon->setIcon(m_currentIcon = Icon("qutim-offline"));
	m_icon->show();
	m_mailIcon = Icon("mail-unread-new");
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

#ifdef Q_WS_WIN
	QWidget *clWindow = ServiceManager::getByName("ContactList")->property("widget").value<QWidget*>();
	clWindow->installEventFilter(new ClEventFilter(this, clWindow));
	activationStateChangedTime = QDateTime::currentMSecsSinceEpoch();
#endif

	SettingsItem *settingsItem = new GeneralSettingsItem<SimpletraySettings>(
				Settings::Plugin, Icon("user-desktop"),
				QT_TRANSLATE_NOOP("Plugin", "Notification Area Icon"));
	Settings::registerItem(settingsItem);
}

void SimpleTray::clActivationStateChanged(bool activated)
{
	activationStateChangedTime = activated ? 0 : QDateTime::currentMSecsSinceEpoch();
}

void SimpleTray::onActivated(QSystemTrayIcon::ActivationReason reason)
{
	if (reason == QSystemTrayIcon::Trigger) {
		if (m_sessions.isEmpty()) {
			if (QObject *obj = ServiceManager::getByName("ContactList")) {
#ifdef Q_WS_WIN
				if (QDateTime::currentMSecsSinceEpoch() - activationStateChangedTime < 200) { // tested - enough
					obj->metaObject()->invokeMethod(obj, "changeVisibility");
					clActivationStateChanged(true);
				}
#endif
				obj->metaObject()->invokeMethod(obj, "changeVisibility");
			}
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

QIcon SimpleTray::unreadIcon()
{
	static QVector<QIcon> icons;
	int number = 0;
	switch (Config(traySettingsFilename).value("showNumber", DontShow))
	{
	default :
	case DontShow : return m_mailIcon;

	case ShowMsgsNumber :
		foreach (ChatSession *s, m_sessions)
			number += s->unread().size();
		break;

	case ShowSessNumber :
		foreach (ChatSession *s, m_sessions)
			if (!s->unread().empty())
				number++;
		break;
	}

	number--;
	if (number >= icons.size())
		icons.resize(number+1);
	if (icons[number].isNull())
		generateIconSizes(m_mailIcon, icons[number], number+1);
	return icons[number];
}

void SimpleTray::generateIconSizes(const QIcon &backing, QIcon &icon, int number)
{
	QFont f = QApplication::font();
	foreach (QSize sz, backing.availableSizes()) {
		QPixmap px(backing.pixmap(sz));
		QPainter p(&px);
		f.setPixelSize(px.height()/1.5);
		p.setFont(f);
		p.drawText(px.rect(), Qt::AlignHCenter | Qt::AlignVCenter, QString::number(number));
		icon.addPixmap(px);
	}
}

void SimpleTray::onUnreadChanged(qutim_sdk_0_3::MessageList unread)
{
	Config cfg(traySettingsFilename);
	ChatSession *session = static_cast<ChatSession*>(sender());
	Q_ASSERT(session != NULL);

	MessageList::iterator itr = unread.begin();
	while (itr != unread.end()) {
		if (itr->property("silent", false))
			itr = unread.erase(itr);
		else
			++itr;
	}

	//bool empty = m_sessions.isEmpty();
	if (unread.isEmpty()) {
		m_sessions.removeOne(session);
	} else if (!m_sessions.contains(session))// {
		m_sessions.append(session);
	/*} else {
   return;
  }
  if (empty == m_sessions.isEmpty())
   return;*/

	if (m_sessions.isEmpty()) {
		if (m_iconTimer.isActive())
			m_iconTimer.stop();
		m_icon->setIcon(m_currentIcon);
		m_isMail = false;
	} else {
		if (!m_iconTimer.isActive() && cfg.value("blink", true) && cfg.value("showIcon", true))
			m_iconTimer.start(500, this);
		if (cfg.value("showIcon", true)) {
			m_generatedIcon = unreadIcon();
			m_icon->setIcon(m_generatedIcon);
			m_isMail = true;
		}
	}
}

void SimpleTray::timerEvent(QTimerEvent *timer)
{
	if (timer->timerId() != m_iconTimer.timerId()) {
		QObject::timerEvent(timer);
	} else {
		m_icon->setIcon(m_isMail ? m_generatedIcon : m_currentIcon);
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
		m_currentIcon = iconForStatus(account->status());
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
		m_currentIcon =iconForStatus(status);
	}
	if (!m_activeAccount) {
		foreach (Account *acc, m_accounts) {
			if (acc->status().type() != Status::Offline) {
				m_activeAccount = acc;
				m_currentIcon = iconForStatus(acc->status());
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
