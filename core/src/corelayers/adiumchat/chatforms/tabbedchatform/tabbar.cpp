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
#include "tabbar.h"
#include <qutim/adiumchat/chatsessionimpl.h>
#include <qutim/adiumchat/chatlayerimpl.h>
#include <qutim/tooltip.h>
#include <qutim/icon.h>
#include <qutim/mimeobjectdata.h>
#include <qutim/shortcut.h>
#include <QStyle>
#include <QAction>
#include <QDropEvent>
#include <QCoreApplication>
#include <QAbstractButton>
#include <QStyleOption>
#include <QPainter>
#include <qutim/conference.h>

namespace Core
{
namespace AdiumChat
{

struct TabBarPrivate
{
	bool closableActiveTab;
	ChatSessionList sessions;
	QMenu *sessionList;
};

class CloseButton : public QAbstractButton
{
public:
	CloseButton(const QIcon &icon) : m_icon(icon), m_drawClose(false)
	{
		setFocusPolicy(Qt::NoFocus);
		setCursor(Qt::ArrowCursor);
		setToolTip(QCoreApplication::translate("CloseButton", "Close Tab"));
		resize(sizeHint());
	}

	~CloseButton()
	{
	}

	void setIcon(const QIcon &icon)
	{
		m_icon = icon;
		update();
	}

	QSize sizeHint() const
	{
		ensurePolished();
		const int width = style()->pixelMetric(QStyle::PM_ButtonIconSize, 0, this);
		const int height = style()->pixelMetric(QStyle::PM_ButtonIconSize, 0, this);
		return QSize(width, height);
	}

	void enterEvent(QEvent *event)
	{
		if (isEnabled())
			update();
		QAbstractButton::enterEvent(event);
	}

	void leaveEvent(QEvent *event)
	{
		if (isEnabled())
			update();
		QAbstractButton::leaveEvent(event);
	}

	void paintEvent(QPaintEvent *)
	{
		QPainter p(this);
		QStyleOption opt;
		opt.init(this);
		opt.state |= QStyle::State_AutoRaise;
		if (isEnabled() && underMouse() && !isChecked() && !isDown())
			opt.state |= QStyle::State_Raised;
		if (isChecked())
			opt.state |= QStyle::State_On;
		if (isDown())
			opt.state |= QStyle::State_Sunken;

		if (const QTabBar *tb = qobject_cast<const QTabBar *>(parent())) {
			int index = tb->currentIndex();
			QTabBar::ButtonPosition position = (QTabBar::ButtonPosition)style()->styleHint(QStyle::SH_TabBar_CloseButtonPosition, 0, tb);
			if (tb->tabButton(index, position) == this)
				opt.state |= QStyle::State_Selected;
		}

		if (m_drawClose) {
			const int closeWidth = style()->pixelMetric(QStyle::PM_TabCloseIndicatorWidth, 0, this);
			const int closeHeight = style()->pixelMetric(QStyle::PM_TabCloseIndicatorHeight, 0, this);
			const QSize closeSize(closeWidth, closeHeight);
			const QSize realSize = size();
			const QSize delta = (realSize - closeSize) / 2;
			p.translate(delta.width(), delta.height());
			style()->drawPrimitive(QStyle::PE_IndicatorTabClose, &opt, &p, this);
		} else {
			QIcon::Mode mode;
			if (!(opt.state & QStyle::State_Enabled))
				mode = QIcon::Disabled;
			else if (opt.state & QStyle::State_Selected)
				mode = QIcon::Selected;
			else
				mode = QIcon::Normal;
			QIcon::State state = opt.state & QStyle::State_Open ? QIcon::On : QIcon::Off;
			m_icon.paint(&p, opt.rect, Qt::AlignCenter, mode, state);
		}
	}

	void setDrawClose(bool drawClose)
	{
		if (m_drawClose != drawClose) {
			m_drawClose = drawClose;
			update();
		}
	}

private:
	QIcon m_icon;
	bool m_drawClose;
};

static inline QTabBar::ButtonPosition closeButtonSide(TabBar *bar)
{
	const int position = bar->style()->styleHint(QStyle::SH_TabBar_CloseButtonPosition, 0, bar);
	return static_cast<QTabBar::ButtonPosition>(position);
}

TabBar::TabBar(QWidget *parent) : QTabBar(parent), p(new TabBarPrivate())
{
	setContextMenuPolicy(Qt::CustomContextMenu);
	setAcceptDrops(true);
	p->closableActiveTab = false;
	setMouseTracking(true);
	p->sessionList = new QMenu(this);
	setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);
	setMovable(true);
#ifdef Q_OS_MAC
	setTabsClosable(true);
	setDrawBase(false);
#else
	setTabsClosable(true);
#endif

	//init shortcuts
	Shortcut *key = new Shortcut ("chatCloseSession",this);
	connect(key,SIGNAL(activated()),SLOT(closeCurrentTab()));
	key = new Shortcut ("chatNext",this);
	connect(key,SIGNAL(activated()),SLOT(showNextTab()));
	key = new Shortcut ("chatPrevious",this);
	connect(key,SIGNAL(activated()),SLOT(showPreviousTab()));
	key = new Shortcut("moveTabLeft", this);
	connect(key, SIGNAL(activated()), SLOT(onMoveTabLeftActivated()));
	key = new Shortcut("moveTabRight", this);
	connect(key, SIGNAL(activated()), SLOT(onMoveTabRightActivated()));

	connect(this, SIGNAL(currentChanged(int)), SLOT(onCurrentChanged(int)));
	connect(this, SIGNAL(tabCloseRequested(int)), SLOT(onCloseRequested(int)));
	connect(this, SIGNAL(tabMoved(int,int)), SLOT(onTabMoved(int,int)));
	connect(this, SIGNAL(customContextMenuRequested(QPoint)), SLOT(onContextMenu(QPoint)));
	connect(p->sessionList,SIGNAL(triggered(QAction*)),SLOT(onSessionListActionTriggered(QAction*)));
}

TabBar::~TabBar()
{
	blockSignals(true);
	foreach(ChatSessionImpl *s,p->sessions)
		removeSession(s);
}

void TabBar::setClosableActiveTab(bool state)
{
	p->closableActiveTab = state;
	setTabsClosable(tabsClosable() | state);
} 

bool TabBar::closableActiveTab()
{
	return p->closableActiveTab;
}

void TabBar::setTabsClosable(bool closable)
{
	p->closableActiveTab = closable & p->closableActiveTab;
	QTabBar::setTabsClosable(closable);
}

void TabBar::mouseMoveEvent(QMouseEvent *event)
{
#ifdef Q_OS_MAC
	const QTabBar::ButtonPosition position = closeButtonSide(this);
	for (int index = 0; index < count(); ++index) {
		if (CloseButton *button = static_cast<CloseButton*>(tabButton(index, position))) {
			const bool contains = tabRect(index).contains(event->pos());
			button->setDrawClose(contains);
		}
	}
#endif
	return QTabBar::mouseMoveEvent(event);
}

void TabBar::leaveEvent(QEvent *event)
{
#ifdef Q_OS_MAC
	const QTabBar::ButtonPosition position = closeButtonSide(this);
	for (int index = 0; index < count(); ++index) {
		if (CloseButton *button = static_cast<CloseButton*>(tabButton(index, position))) {
			button->setDrawClose(false);
		}
	}
#endif
	return QTabBar::leaveEvent(event);
}

void TabBar::addSession(ChatSessionImpl *session)
{
	p->sessions.append(session);
	ChatUnit *u = session->getUnit();
	QIcon icon = ChatLayerImpl::iconForState(u->chatState(), u);
	p->sessionList->addAction(icon, u->title());
#ifdef Q_OS_MAC
	addTab(u->title());
	const int index = count() - 1;
	CloseButton *button = new CloseButton(icon);
	connect(button, SIGNAL(clicked()), SLOT(onCloseButtonClicked()));
	setTabButton(index, closeButtonSide(this), button);
#else
	addTab(icon, u->title());
#endif

//#if defined(Q_OS_MAC)
//	if (closableActiveTab()) {
//		QTabBar::ButtonPosition closeSide = (QTabBar::ButtonPosition)style()->styleHint(QStyle::SH_TabBar_CloseButtonPosition, 0, this);
//		if (QWidget *button = tabButton(count()-1, closeSide))
//			button->setVisible(false);
//	}
//#endif

	connect(session->getUnit(),SIGNAL(titleChanged(QString,QString)),
			this,SLOT(onTitleChanged(QString)));
	connect(u, SIGNAL(chatStateChanged(qutim_sdk_0_3::ChatUnit::ChatState,qutim_sdk_0_3::ChatUnit::ChatState)),
			this, SLOT(onChatStateChanged(qutim_sdk_0_3::ChatUnit::ChatState,qutim_sdk_0_3::ChatUnit::ChatState)));
	if (Buddy *buddy = qobject_cast<Buddy*>(u))
		connect(buddy,
				SIGNAL(statusChanged(qutim_sdk_0_3::Status,qutim_sdk_0_3::Status)),
				this,
				SLOT(onStatusChanged(qutim_sdk_0_3::Status)));
	connect(session,SIGNAL(destroyed(QObject*)),SLOT(onRemoveSession(QObject*)));
	connect(session,SIGNAL(unreadChanged(qutim_sdk_0_3::MessageList)),
			this,SLOT(onUnreadChanged(qutim_sdk_0_3::MessageList)));
}

void TabBar::removeSession(ChatSessionImpl *session)
{
	removeTab(p->sessions.indexOf(session));
}

ChatSessionImpl *TabBar::session(int index) const
{
	if((index == -1) || (index >= p->sessions.count()))
		return 0;
	return p->sessions.at(index);
}

ChatSessionImpl *TabBar::currentSession() const
{
	int index = currentIndex();
	if(index != -1 && index < p->sessions.count())
		return p->sessions.at(currentIndex());
	return 0;
}

void TabBar::setCurrentSession(ChatSessionImpl *session)
{
	setCurrentIndex(p->sessions.indexOf(session));
}

bool TabBar::contains(ChatSessionImpl *session) const
{
	return p->sessions.contains(session);
}

int TabBar::indexOf(ChatSessionImpl *session) const
{
	return p->sessions.indexOf(session);
}

void TabBar::onCurrentChanged(int index)
{
	if(ChatSessionImpl *s = session(index))
		s->setActive(true);
}

void TabBar::onCloseRequested(int index)
{
	removeTab(index);
}

void TabBar::removeTab(int index)
{
	ChatSessionImpl *s = p->sessions.at(index);
	s->disconnect(this);
	s->removeEventFilter(this);
	s->unit()->disconnect(this);
	onRemoveSession(s);
	emit remove(s);
}

void TabBar::onRemoveSession(QObject *obj)
{
	ChatSessionImpl *s = static_cast<ChatSessionImpl*>(obj);
	int index = p->sessions.indexOf(s);
	p->sessions.removeAll(s);
	p->sessionList->removeAction(p->sessionList->actions().at(index));
	QTabBar::removeTab(index);
}

void TabBar::onTabMoved(int from, int to)
{
	p->sessions.move(from,to);
	//small hack
	QList <QAction *> actions = p->sessionList->actions();
	actions.move(from,to);
	foreach (QAction *a,p->sessionList->actions())
		p->sessionList->removeAction(a);
	p->sessionList->addActions(actions);
}

void TabBar::onTitleChanged(const QString &title)
{
	ChatUnit *u = qobject_cast<ChatUnit*>(sender());
	ChatSessionImpl *s = static_cast<ChatSessionImpl*>(ChatLayer::get(u,false));
	setTabText(indexOf(s),title);
}

void TabBar::onChatStateChanged(qutim_sdk_0_3::ChatUnit::ChatState now, qutim_sdk_0_3::ChatUnit::ChatState)
{
	ChatUnit *unit = qobject_cast<ChatUnit*>(sender());
	Q_ASSERT(unit);
	ChatSessionImpl *s = static_cast<ChatSessionImpl*>(ChatLayer::get(unit,false));
	if(s)
		chatStateChanged(now,s);
}

void TabBar::onStatusChanged(const qutim_sdk_0_3::Status &status)
{
	ChatUnit *unit = qobject_cast<ChatUnit*>(sender());
	Q_ASSERT(unit);
	ChatSessionImpl *s = static_cast<ChatSessionImpl*>(ChatLayer::get(unit,false));
	if(s)
		statusChanged(status,s);
}

void TabBar::onCloseButtonClicked()
{
	QObject *object = sender();
	ButtonPosition position = closeButtonSide(this);
	for (int i = 0; i < count(); ++i) {
		if (tabButton(i, position) == object) {
			emit tabCloseRequested(i);
			return;
		}
	}
}

bool TabBar::event(QEvent *event)
{
	if (event->type() == QEvent::ToolTip) {
		if (QHelpEvent *help = static_cast<QHelpEvent*>(event)) {
			int index = tabAt(help->pos());
			if (index != -1) {
				ChatUnit *unit = session(index)->getUnit();
				ToolTip::instance()->showText(help->globalPos(), unit, this);
				return true;
			}
		}
	} else if (event->type() == QEvent::DragEnter) {
		QDragEnterEvent *dragEvent = static_cast<QDragEnterEvent*>(event);
		if (const MimeObjectData *data = qobject_cast<const MimeObjectData*>(dragEvent->mimeData())) {
			ChatUnit *u = qobject_cast<ChatUnit*>(data->object());
			if (u)
				dragEvent->acceptProposedAction();
		}
		return true;
	} else if (event->type() == QEvent::Drop) {
		QDropEvent *dropEvent = static_cast<QDropEvent*>(event);
		if (const MimeObjectData *mimeData
				= qobject_cast<const MimeObjectData*>(dropEvent->mimeData())) {
			if (ChatUnit *u = qobject_cast<ChatUnit*>(mimeData->object())) {
				ChatLayerImpl::get(u,true)->activate();
				dropEvent->setDropAction(Qt::CopyAction);
				dropEvent->accept();
				return true;
			}
		}
	}
	return QTabBar::event(event);
}

void TabBar::chatStateChanged(ChatUnit::ChatState state, ChatSessionImpl *session)
{
	if(session->unread().count())
		return;
	QIcon icon = ChatLayerImpl::iconForState(state, session->getUnit());
	setSessionIcon(session, icon);
}

void TabBar::statusChanged(const Status &status, ChatSessionImpl *session)
{
	if(session->unread().count())
		return;
	setSessionIcon(session, status.icon());
}

void TabBar::setSessionIcon(ChatSessionImpl *session, const QIcon &icon)
{
#ifdef Q_OS_MAC
	const int index = indexOf(session);
	CloseButton *button = static_cast<CloseButton*>(tabButton(index, closeButtonSide(this)));
	button->setIcon(icon);
#else
	setTabIcon(indexOf(session), icon);
#endif
	p->sessionList->actions().at(indexOf(session))->setIcon(icon);
}

void TabBar::onUnreadChanged(const qutim_sdk_0_3::MessageList &unread)
{
	ChatSessionImpl *session = static_cast<ChatSessionImpl*>(sender());
	int index = indexOf(session);
	QIcon icon;
	QString title = session->getUnit()->title();
	if (unread.isEmpty()) {
		ChatUnit::ChatState state = static_cast<ChatUnit::ChatState>(session->property("currentChatState").toInt());//FIXME remove in future
		icon =  ChatLayerImpl::iconForState(state,session->getUnit());
	} else {
		icon = Icon("mail-unread-new");
	}

	QPalette pal;
	int unreadMessages = 0;
	bool messagesForMe = false;
	QString str = title;
	setTabTextColor(index, pal.color(QPalette::WindowText));
	setSessionIcon(session, icon);
	setTabText(index, title);
	for (int i = 0; i < unread.size(); ++i) {
		qutim_sdk_0_3::Message message = unread.at(i);
		unreadMessages ++;
		messagesForMe |= (message.property("mention") == true || !qobject_cast<Conference*>(message.chatUnit()));
	}
	if (messagesForMe) {
		str.prepend(QLatin1Char('*'));
		setTabTextColor(index, pal.color(QPalette::Highlight));
	}
	if (unreadMessages > 0)
		str.append(" (" + QString::number(unreadMessages) + ")");
	setTabText(index, str);
}

void TabBar::onContextMenu(const QPoint &pos)
{
	int index = tabAt(pos);
	if (index != -1)
		session(index)->getUnit()->showMenu(mapToGlobal(pos));
}

QMenu *TabBar::menu() const
{
	return p->sessionList;
}

void TabBar::onSessionListActionTriggered(QAction *act)
{
	int index = p->sessionList->actions().indexOf(act);
	setCurrentIndex(index);
}

void TabBar::closeCurrentTab()
{
	onCloseRequested(currentIndex());
}

void TabBar::showNextTab()
{
	qDebug() << Q_FUNC_INFO;
	if (count() == 0)
		return;
	int index = (currentIndex() + 1) % count();
	setCurrentIndex(index);
}

void TabBar::showPreviousTab()
{
	qDebug() << Q_FUNC_INFO;
	if (count() == 0)
		return;
	int index = (count() + currentIndex() - 1) % count();
	setCurrentIndex(index);
}

void TabBar::onMoveTabLeftActivated()
{
	qDebug() << Q_FUNC_INFO;
	if (currentIndex() != 0)
	moveTab(currentIndex(), currentIndex() - 1);
}

void TabBar::onMoveTabRightActivated()
{
	qDebug() << Q_FUNC_INFO;
	if (currentIndex() < count())
	moveTab(currentIndex(), currentIndex() + 1);
}

}
}

