/****************************************************************************
 *
 *  This file is part of qutIM
 *
 *  Copyright (c) 2010 by Nigmatullin Ruslan <euroelessar@gmail.com>
 *
 ***************************************************************************
 *                                                                         *
 *   This file is part of free software; you can redistribute it and/or    *
 *   modify it under the terms of the GNU General Public License as        *
 *   published by the Free Software Foundation; either version 2 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 ***************************************************************************
 ****************************************************************************/

#ifndef ABSTRACTCHATWIDGET_H
#define ABSTRACTCHATWIDGET_H

#include <QMainWindow>
#include <QTimer>
#include <QTextCursor>
#include <chatlayer/chatsessionimpl.h>
#include <chatlayer/chatlayerimpl.h>
#include <qutim/actiongenerator.h>
#include "chatlayer_global.h"

namespace qutim_sdk_0_3 {
}

class QPlainTextEdit;
class QListView;
class QModelIndex;
namespace Core
{
namespace AdiumChat
{

using namespace qutim_sdk_0_3;

class ADIUMCHAT_EXPORT AbstractChatWidget : public QMainWindow
{
	Q_OBJECT
public:
	AbstractChatWidget(QWidget *parent = 0);
	virtual void addAction(ActionGenerator *gen) = 0;
	void addActions(const QList<ActionGenerator*> &actions);
	virtual QPlainTextEdit *getInputField() const = 0;
	virtual bool contains(ChatSessionImpl *session) const = 0;
	virtual ~AbstractChatWidget() {}
	virtual ChatSessionImpl *currentSession() const = 0;
	static QString titleForSession(ChatSessionImpl *s);
public slots:
	virtual void addSession(ChatSessionImpl *session) = 0;
	void addSessions(const ChatSessionList &sessions);
	virtual void removeSession(ChatSessionImpl *session) = 0;
	virtual void activate(ChatSessionImpl* session) = 0;
	virtual void loadSettings() = 0;
protected:
	virtual void setTitle(ChatSessionImpl *s);
signals:
	void currentSessionChanged(ChatSessionImpl *now,ChatSessionImpl *before);
protected:

};

}
}
#endif // ABSTRACTCHATWIDGET_H
