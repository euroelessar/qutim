/****************************************************************************
 *  ircprotocol.h
 *
 *  Copyright (c) 2010 by Prokhin Alexey <alexey.prokhin@yandex.ru>
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

#ifndef IRCPROTOCOL_H
#define IRCPROTOCOL_H

#include <qutim/protocol.h>
#include <ircglobal.h>
#include <irccommandalias.h>

class QAction;

namespace qutim_sdk_0_3 {

class ChatSession;

namespace irc {

class IrcProtocolPrivate;
class IrcAccount;

class IrcProtocol: public Protocol
{
	Q_OBJECT
	Q_CLASSINFO("Protocol", "irc")
public:
	IrcProtocol();
	virtual ~IrcProtocol();
	static inline IrcProtocol *instance() { if (!self) qWarning("IrcProtocol isn't created"); return self; }
	virtual QList<Account *> accounts() const;
	virtual Account *account(const QString &id) const;
	virtual QVariant data(DataType type);
	IrcAccount *getAccount(const QString &id, bool create = false);
	ChatSession *activeSession() const;
	static void registerCommandAlias(IrcCommandAlias *alias);
	static void removeCommandAlias(const QString &name);
	static void removeCommandAlias(IrcCommandAlias *alias);
	static QString ircFormatToHtml(const QString &msg, QString *plainText = 0);
	static QString ircFormatToPlainText(const QString &msg);
public slots:
	void updateSettings();
protected:
	void loadAccounts();
private slots:
	void onSessionCreated(qutim_sdk_0_3::ChatSession *session);
	void onSessionActivated(bool active);
private:
	QScopedPointer<IrcProtocolPrivate> d;
	static IrcProtocol *self;
};

} } // namespace qutim_sdk_0_3::irc

#endif // IRCPROTOCOL_H
