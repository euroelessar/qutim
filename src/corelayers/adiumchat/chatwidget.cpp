#include "chatwidget.h"
#include "chatsessionimpl.h"
#include "ui_chatwidget.h"
#include <libqutim/account.h>
#include <libqutim/icon.h>

namespace AdiumChat
{
	
	ChatWidget::ChatWidget(QWidget* parent, Qt::WindowFlags f): QWidget(parent, f), ui(new Ui::AdiumChatForm)
	{
		ui->setupUi(this);
		ui->tabBar->setVisible(false);
		setAttribute(Qt::WA_DeleteOnClose);
		connect(ui->tabBar,SIGNAL(currentChanged(int)),SLOT(currentIndexChanged(int)));
	}

	void ChatWidget::addSession(ChatSessionImpl* session)
	{
		m_sessions.append(session);
		connect(session,SIGNAL(removed(Account*,QString)),SLOT(onSessionRemoved()));
		QString imagePath = session->getAccount()->property("imagepath").toString();
		QIcon icon = imagePath.isEmpty() ? QIcon(":/icons/qutim_64") : QIcon(imagePath);
		ui->tabBar->addTab(icon,session->getId());
		if (ui->tabBar->count() >1)
			ui->tabBar->setVisible(true);
	}
	
	void ChatWidget::currentIndexChanged(int index)
	{
		ui->chatView->setPage(m_sessions.at(index)->getPage());
	}

	void ChatWidget::clear()
	{
		qDeleteAll(m_sessions);
		int count = m_sessions.count();
		m_sessions.clear();
		for (int i = 0;i!=count;i++)
			ui->tabBar->removeTab(i);
	}
	
	void ChatWidget::removeSession(ChatSessionImpl* session)
	{
		int index = m_sessions.indexOf(session);
		if (index != -1)
		{
			ui->tabBar->removeTab(index);
			if (ui->tabBar->count() == 1)
				ui->tabBar->setVisible(false);
		}
		
	}
	
	void ChatWidget::onSessionRemoved()
	{
		ChatSessionImpl *session = qobject_cast<ChatSessionImpl *>(sender());
		if (session)
			removeSession(session);
	}

}