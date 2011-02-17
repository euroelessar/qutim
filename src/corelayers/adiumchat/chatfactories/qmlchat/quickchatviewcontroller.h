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
#include <chatlayer/chatsessionimpl.h>
#include <QVariant>
#include <QPointer>

class QDeclarativeEngine;
class QDeclarativeItem;
class QDeclarativeContext;
namespace Core {
namespace AdiumChat {

class QuickChatViewController : public QGraphicsScene, public ChatViewController
{
    Q_OBJECT
	Q_INTERFACES(Core::AdiumChat::ChatViewController)
	Q_PROPERTY(QObject* session READ getSession NOTIFY sessionChanged)
public:
	QuickChatViewController(QDeclarativeEngine *engine);
	virtual void setChatSession(ChatSessionImpl *session);
	virtual ChatSessionImpl *getSession() const;
	virtual void appendMessage(const qutim_sdk_0_3::Message &msg);
	virtual void clearChat();
    virtual ~QuickChatViewController();
	QDeclarativeItem *rootItem() const;
	Q_INVOKABLE QString parseEmoticons(const QString &) const;
public slots:
	void loadSettings();
protected slots:
	void loadHistory();
protected:
	bool eventFilter(QObject *, QEvent *);
signals:
	void messageAppended(const QVariant &message);
	void messageDelivered(int mid);
	void clearChatField();
	void sessionChanged(Core::AdiumChat::ChatSessionImpl *);
private:
	ChatSessionImpl *m_session;
	QString m_themeName;
	QDeclarativeEngine *m_engine;
	QDeclarativeContext *m_context;
	QPointer<QDeclarativeItem> m_item;
	bool m_storeServiceMessages;
};

} // namespace AdiumChat
} // namespace Core

#endif // QUICKCHATVIEWCONTROLLER_H
