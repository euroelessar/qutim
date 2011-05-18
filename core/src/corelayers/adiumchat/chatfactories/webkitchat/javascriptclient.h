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

#ifndef JAVASCRIPTCLIENT_H
#define JAVASCRIPTCLIENT_H

#include <QObject>
namespace Core
{
namespace AdiumChat
{
class ChatSessionImpl;

class JavaScriptClient : public QObject
{
	Q_OBJECT
public:
	JavaScriptClient(ChatSessionImpl *session);

public slots:
	void debugLog(const QVariant &text);
	bool zoomImage(const QVariant &text);
	void helperCleared();
	void appendNick(const QVariant &nick);
	void contextMenu(const QVariant &nickVar);
	void appendText(const QVariant &text);

private:
	ChatSessionImpl *m_session;
};
}
}

#endif // JAVASCRIPTCLIENT_H
