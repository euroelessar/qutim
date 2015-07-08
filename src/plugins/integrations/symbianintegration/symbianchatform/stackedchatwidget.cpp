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
#include "stackedchatwidget.h"
#include <qutim/servicemanager.h>
#include <qutim/adiumchat/chatviewfactory.h>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <qutim/actiontoolbar.h>
#include <qutim/adiumchat/sessionlistwidget.h>
#include "fingerswipegesture.h"
#include <qutim/adiumchat/chatedit.h>
#include <qutim/adiumchat/conferencecontactsview.h>
#include <QPlainTextEdit>
#include <qutim/debug.h>
#include <qutim/icon.h>
#include <qutim/conference.h>
#include <qutim/config.h>
#include <qutim/qtwin.h>
#include <QAbstractItemModel>
#include <QSplitter>
#include <qutim/shortcut.h>
#include <QToolButton>
#include <slidingstackedwidget.h>
#include <qutim/servicemanager.h>
#include <QApplication>
#include <qutim/actionbox.h>
#include <qutim/servicemanager.h>

namespace Core
{
namespace AdiumChat
{
namespace Symbian
{

StackedChatWidget::StackedChatWidget(QWidget *parent) :
	AbstractChatWidget(parent),
	m_chatInput(new ChatEdit(this)),
	m_receiverList(new QAction(tr("Send to"),this)),
	m_stack(new SlidingStackedWidget(this)),
	m_chatWidget(new QWidget(m_stack)),
	m_menu(new MenuController(this))
{
	QWidget *widget = new QWidget(this);
	QVBoxLayout *l = new QVBoxLayout(widget);
	l->setMargin(0);
	l->addWidget(m_stack);

	setCentralWidget(widget);
	QWidget *view = ChatViewFactory::instance()->createViewWidget();
	view->setParent(m_chatWidget);

	//TODO move to chatform
	//FIXME Create session list and chat when it's realy needed
	if (MenuController *controller = ServiceManager::getByName<MenuController*>("ContactList")) {
		controller->metaObject()->invokeMethod(controller, "widget",Q_RETURN_ARG(QWidget*, m_contactList));
		m_stack->addWidget(m_contactList);
		m_stack->setCurrentWidget(m_contactList);

		ActionGenerator *gen = new ActionGenerator(QIcon(),
												   QT_TRANSLATE_NOOP("ChatWidget", "Toggle fullscreen"),
												   this,
												   SLOT(onToggleFullscreen()));
		controller->addAction(gen);
	}
	m_stack->setWrap(true);
	m_sessionList = new SessionListWidget(this);
	m_confContactView = new ConferenceContactsView(this);

	QWidget *chatInputWidget = new QWidget(m_chatWidget);
	chatInputWidget->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);
	chatInputWidget->setMaximumHeight(m_chatInput->minimumHeight());

	QHBoxLayout *chatInputLayout = new QHBoxLayout(chatInputWidget);
	chatInputLayout->setMargin(0);
	chatInputLayout->setSpacing(0);

	QToolButton *leftBtn = new QToolButton(this);
	leftBtn->setIcon(Icon("preferences-contact-list"));
	leftBtn->setMenu(m_menu->menu());
	leftBtn->setPopupMode(QToolButton::InstantPopup);

	QToolButton *rightBtn = new QToolButton(this);
	rightBtn->setDefaultAction(m_receiverList);

	chatInputLayout->setMargin(0);
	chatInputLayout->addWidget(leftBtn);
	chatInputLayout->addWidget(m_chatInput);
	chatInputLayout->addWidget(rightBtn);

	QVBoxLayout *layout = new QVBoxLayout(m_chatWidget);
	layout->addWidget(view);
	layout->addWidget(chatInputWidget);
	layout->setMargin(0);

	m_view = qobject_cast<ChatViewWidget*>(view);

	loadSettings();
	connect(m_sessionList,SIGNAL(remove(ChatSessionImpl*)),SLOT(removeSession(ChatSessionImpl*)));
	connect(m_stack,SIGNAL(currentChanged(int)),SLOT(onCurrentChanged(int)));

	m_receiverList->setIcon(Icon("view-choose"));

	setAttribute(Qt::WA_AcceptTouchEvents);
	connect(m_stack, SIGNAL(fingerGesture(enum SlidingStackedWidget::SlideDirection)),
			this, SLOT(fingerGesture(enum SlidingStackedWidget::SlideDirection)));
	connect(m_stack, SIGNAL(animationFinished()),
			this, SLOT(animationFinished()));

	m_sendAct = new QAction(tr("Send"),m_chatWidget);
	m_sendAct->setSoftKeyRole(QAction::NegativeSoftKey);
	m_chatWidget->addAction(m_sendAct);
	connect(m_sendAct, SIGNAL(triggered()), m_chatInput, SLOT(send()));

	m_chatWidget->setVisible(false);
	m_sessionList->setVisible(false);

	setWindowIcon(Icon("qutim-trayicon"));
}

void StackedChatWidget::loadSettings()
{
	ConfigGroup cfg = Config("appearance").group("chat/behavior/widget");
	m_chatInput->setSendKey(cfg.value("sendKey", SendEnter));
}

StackedChatWidget::~StackedChatWidget()
{
}

QPlainTextEdit *StackedChatWidget::getInputField() const
{
	return m_chatInput;
}

bool StackedChatWidget::contains(ChatSessionImpl *session) const
{
	return m_sessionList->contains(session);
}

void StackedChatWidget::addAction(ActionGenerator *gen)
{
	m_menu->addAction(gen);
}

void StackedChatWidget::addSession(ChatSessionImpl *session)
{
	if(!m_sessionList->count()) {
		m_sessionList->setVisible(true);
		m_chatWidget->setVisible(true);
		m_stack->addWidget(m_sessionList);
		m_stack->addWidget(m_chatWidget);
	}

	m_sessionList->addSession(session);
	connect(session,SIGNAL(activated(bool)),SLOT(onSessionActivated(bool)));
	connect(session,SIGNAL(unreadChanged(qutim_sdk_0_3::MessageList)),SLOT(onUnreadChanged()));
}

void StackedChatWidget::removeSession(ChatSessionImpl *session)
{
	if(contains(session))
		m_sessionList->removeSession(session);

	if(session == m_currentSession) {
		m_view->setViewController(0);
		m_chatInput->setDocument(0);
		//FIXME Symbian workaround
		if (m_unitActions) {
			m_chatWidget->removeAction(m_unitActions);
			m_unitActions->deleteLater();
		}
	}

	session->setActive(false);
	session->deleteLater();

	if(!m_sessionList->count()) {
		m_stack->slideInIdx(m_stack->indexOf(m_contactList));
	} else {
		m_stack->slideInIdx(m_stack->indexOf(m_sessionList));
	}
}

void StackedChatWidget::onSessionActivated(bool active)
{
	if(!active)
		return;

	ChatSessionImpl *session = qobject_cast<ChatSessionImpl*>(sender());
	Q_ASSERT(session);

	m_sessionList->setCurrentSession(session);
}

void StackedChatWidget::activate(ChatSessionImpl *session)
{
	if(!session->unread().isEmpty())
		session->markRead();

	setTitle(session);

	if(m_currentSession) {
		if(m_currentSession == session) {
			m_stack->slideInIdx(m_stack->indexOf(m_chatWidget));
			return;
		}
		m_currentSession->setActive(false);
	}
	emit currentSessionChanged(session, m_currentSession);
	m_currentSession = session;

	m_sessionList->setCurrentSession(session);
	m_chatInput->setSession(session);
	m_confContactView->setSession(session);
	m_view->setViewController(session->getController());

	if(m_confContactView->isVisible())
		m_stack->addWidget(m_confContactView);
	else
		m_stack->removeWidget(m_confContactView);

	m_receiverList->setMenu(session->menu());

	m_stack->slideInIdx(m_stack->indexOf(m_chatWidget));
}

ChatSessionImpl *StackedChatWidget::currentSession() const
{
	return m_currentSession;
}

bool StackedChatWidget::event(QEvent *event)
{
	return AbstractChatWidget::event(event);
}

void StackedChatWidget::fingerGesture( enum SlidingStackedWidget::SlideDirection direction)
{
	if (direction==SlidingStackedWidget::LeftToRight) {
		m_stack->slideInPrev();
		m_confContactView->blockSignals(true);
	}
	else if (direction==SlidingStackedWidget::RightToLeft) {
		m_stack->slideInNext();
		m_confContactView->blockSignals(true);
	}

}

void StackedChatWidget::activateWindow()
{
	if (m_stack == m_chatWidget && m_currentSession)
		m_currentSession->setActive(isActiveWindow());
	AbstractChatWidget::activateWindow();
}

void StackedChatWidget::onUnreadChanged()
{
	ChatSessionImpl *s = qobject_cast<ChatSessionImpl*>(sender());
	if(s && s == m_sessionList->currentSession())
		setTitle(s);
}

void StackedChatWidget::onCurrentChanged(int index)
{
	if (index != m_stack->indexOf(m_chatWidget)) {
		if (m_currentSession)
			m_currentSession->setActive(false);
	}
	setWindowFilePath(m_stack->currentWidget()->windowTitle());
	//FIXME Symbian workaround
	onAboutToChangeIndex(index);
}

void StackedChatWidget::onAboutToChangeIndex(int index)
{
	if (index != m_stack->indexOf(m_chatWidget)) {
		if (m_unitActions) {
			//FIXME Symbian workaround
			m_chatWidget->removeAction(m_unitActions);
			m_unitActions->deleteLater();
		}
		if (currentSession())
			currentSession()->setActive(false);
	} else  {
		if (!m_unitActions && currentSession()) {
			//FIXME Symbian workaround
			m_unitActions = new QAction (tr("Actions"), m_chatWidget);
			m_unitActions->setMenu(currentSession()->getUnit()->menu());
			m_unitActions->setSoftKeyRole(QAction::PositiveSoftKey);
			m_chatWidget->addAction(m_unitActions);
		}
	}
}

void StackedChatWidget::animationFinished()
{
	m_confContactView->blockSignals(false);
}

void StackedChatWidget::changeEvent(QEvent *ev)
{
	if (ev->type() == QEvent::LanguageChange) {
		if (m_unitActions)
			m_unitActions->setText(tr("Actions"));
		m_sendAct->setText(tr("Send"));
	}
}

void StackedChatWidget::setTitle(ChatSessionImpl *s)
{
	m_chatWidget->setWindowTitle(titleForSession(s));
}

void StackedChatWidget::onToggleFullscreen()
{
	if (isFullScreen()) {
		showMaximized();
	} else {
		setWindowFlags(windowFlags() | Qt::WindowSoftkeysVisibleHint);
		showFullScreen();
	}
}

} // namespace Symbian
} // namespace AdiumChat
} // namespace Core

