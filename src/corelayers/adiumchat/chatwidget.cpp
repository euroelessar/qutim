#include "chatwidget.h"
#include "chatsessionimpl.h"
#include "ui_chatwidget.h"
#include <libqutim/account.h>
#include <libqutim/icon.h>
#include <QWebFrame>
#include <QDebug>

namespace AdiumChat
{
	
	ChatWidget::ChatWidget(ChatFlag chatFlags): ui(new Ui::AdiumChatForm),m_chat_flags(chatFlags)
	{
		ui->setupUi(this);
		ui->tabBar->setVisible(false);
		ui->tabBar->setTabsClosable(true);
		ui->tabBar->setMovable(true);
		setAttribute(Qt::WA_DeleteOnClose);
		connect(ui->tabBar,SIGNAL(currentChanged(int)),SLOT(currentIndexChanged(int)));
		connect(ui->tabBar,SIGNAL(tabMoved(int,int)),SLOT(onTabMoved(int,int)));
		connect(ui->tabBar,SIGNAL(tabCloseRequested(int)),SLOT(onCloseRequested(int)));
	}
	
	ChatWidget::~ChatWidget()
	{
		clear();
	}

	void ChatWidget::addSession(ChatSessionImpl* session)
	{
		if(m_sessions.contains(session))
			return;
		m_sessions.append(session);
		connect(session,SIGNAL(removed(Account*,QString)),SLOT(onSessionRemoved()));
		QString imagePath = session->getAccount()->property("imagepath").toString();
		QIcon icon = imagePath.isEmpty() ? QIcon(":/icons/qutim_64") : QIcon(imagePath);
		ui->tabBar->addTab(icon,session->getId());
		if (ui->tabBar->count() >1)
			ui->tabBar->setVisible(true);
	}
	
	void ChatWidget::addSession(const ChatSessionList &sessions)
	{
		for (int i;i!=sessions.count();i++)
			addSession(sessions.at(i));
	}
	
	void ChatWidget::currentIndexChanged(int index)
	{
		if (index == -1)
			return;
		ui->chatView->setPage(m_sessions.at(index)->getPage());
		//ui->chatView->page()->currentFrame()->toHtml();
	}

	void ChatWidget::clear()
	{
		if (m_chat_flags & RemoveSessionOnClose)
			qDeleteAll(m_sessions);
		int count = m_sessions.count();
		m_sessions.clear();
		for (int i = 0;i!=count;i++)
			ui->tabBar->removeTab(i);
	}
	
	void ChatWidget::removeSession(ChatSessionImpl* session)
	{
		int index = m_sessions.indexOf(session);
		if (index == -1)
			return;
	
		ui->tabBar->removeTab(index);
		m_sessions.removeAt(index);
		if (ui->tabBar->count() == 1)
			ui->tabBar->setVisible(false);
		currentIndexChanged(ui->tabBar->currentIndex());
		if (m_chat_flags & RemoveSessionOnClose)
			session->deleteLater();
	}
	
	void ChatWidget::onSessionRemoved()
	{
		ChatSessionImpl *session = qobject_cast<ChatSessionImpl *>(sender());
		if (session && m_sessions.contains(session))
			removeSession(session);
	}

	ChatSessionList ChatWidget::getSessionList() const
	{
		return m_sessions;
	}
	
	void ChatWidget::onCloseRequested(int index)
	{
		removeSession(m_sessions.at(index));
	}

	void ChatWidget::onTabMoved(int from, int to)
	{
		m_sessions.move(from,to);
	}

}
