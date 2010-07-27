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

class QAction;

namespace qutim_sdk_0_3 {

class ChatSession;

namespace irc {

class IrcProtocolPrivate;
class IrcAccount;

struct IrcCommandAlias
{
	enum Type {
		Disabled     = 0x0000,
		Channel      = 0x0001,
		PrivateChat  = 0x0002,
		Console      = 0x0004,
		All          = Channel | PrivateChat | Console
	};
	Q_DECLARE_FLAGS(Types, Type);
	IrcCommandAlias(const QString &name, const QString &command, Types types = All);
	QString name;
	QString command;
	Types types;
};
Q_DECLARE_OPERATORS_FOR_FLAGS(IrcCommandAlias::Types);

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
	static void registerCommandAlias(const IrcCommandAlias &alias);
	static void removeCommandAlias(const QString &name);
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
