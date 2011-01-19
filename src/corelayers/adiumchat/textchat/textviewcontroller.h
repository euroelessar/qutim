/****************************************************************************
 *
 *  This file is part of qutIM
 *
 *  Copyright (c) 2011 by Nigmatullin Ruslan <euroelessar@gmail.com>
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

#ifndef TEXTVIEWCONTROLLER_H
#define TEXTVIEWCONTROLLER_H

#include <chatlayer/chatviewfactory.h>
#include <QTextDocument>
#include <QCache>

namespace Core
{
namespace AdiumChat
{
class TextViewController : public QTextDocument, public ChatViewController
{
	Q_OBJECT
	Q_INTERFACES(Core::AdiumChat::ChatViewController)
public:
    TextViewController();
	virtual ~TextViewController();
	virtual void setChatSession(ChatSessionImpl *session);
	virtual ChatSessionImpl *getSession() const;
	virtual void appendMessage(const qutim_sdk_0_3::Message &msg);
	virtual void clear();
private:
	ChatSessionImpl *m_session;
	QCache<int, int> m_cache;
};

#endif // TEXTVIEWCONTROLLER_H
