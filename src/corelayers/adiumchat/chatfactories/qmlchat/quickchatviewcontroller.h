/****************************************************************************
 *  quickchatviewcontroller.h
 *
 *  Copyright (c) 2011 by Sidorov Aleksey <sauron@citadelspb.com>
 *
 ***************************************************************************
 *                                                                         *
 *   This library is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************
*****************************************************************************/

#ifndef QUICKCHATVIEWCONTROLLER_H
#define QUICKCHATVIEWCONTROLLER_H

#include <QGraphicsScene>
#include <chatlayer/chatviewfactory.h>
#include <QVariant>

class QDeclarativeEngine;
namespace Core {
namespace AdiumChat {

class QuickChatViewController : public QGraphicsScene, public ChatViewController
{
    Q_OBJECT
	Q_INTERFACES(Core::AdiumChat::ChatViewController)
public:
	QuickChatViewController(/*QDeclarativeEngine *engine*/);
	virtual void setChatSession(ChatSessionImpl *session);
	virtual ChatSessionImpl *getSession() const;
	virtual void appendMessage(const qutim_sdk_0_3::Message &msg);
	virtual void clearChat();
    virtual ~QuickChatViewController();
signals:
	void messageAppended(const QVariant &message);
private:
	ChatSessionImpl *m_session;
	QString m_themeName;
	QDeclarativeEngine *m_engine;
};

} // namespace AdiumChat
} // namespace Core

#endif // QUICKCHATVIEWCONTROLLER_H
