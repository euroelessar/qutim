/****************************************************************************
 *  ircactiongenerator.h
 *
 *  Copyright (c) 2011 by Prokhin Alexey <alexey.prokhin@yandex.ru>
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

#include "ircactiongenerator_p.h"
#include "irccommandalias.h"
#include "ircchannelparticipant.h"
#include "ircchannel.h"
#include "ircaccount.h"
#include <qutim/localizedstring.h>

Q_DECLARE_METATYPE(qutim_sdk_0_3::irc::IrcCommandAlias*)

namespace qutim_sdk_0_3 {
namespace irc {

IrcActionGenerator::IrcActionGenerator(const QIcon &icon,
									   const LocalizedString &text,
									   IrcCommandAlias *command) :
	ActionGenerator(icon, text, IrcActionsManager::instance(), SLOT(onIrcActionTriggered(QAction*,QObject*))),
	d(new IrcActionGeneratorPrivate)
{
	d->command = command;
}

QObject *IrcActionGenerator::generateHelper() const
{
	QAction *action = prepareAction(new QAction(NULL));
	action->setProperty("command", qVariantFromValue(d->command));
	return action;
}

IrcActionsManager::IrcActionsManager()
{
}

IrcActionsManager *IrcActionsManager::instance()
{
	static IrcActionsManager manager;
	return &manager;
}

void IrcActionsManager::onIrcActionTriggered(QAction *action, QObject *controller)
{
	IrcCommandAlias *command = action->property("command").value<IrcCommandAlias*>();
	Q_ASSERT(command);
	IrcCommandAlias::Type type = IrcCommandAlias::Disabled;
	QHash<QChar, QString> extParams;
	IrcAccount *account = 0;
	if (IrcChannelParticipant *participant = qobject_cast<IrcChannelParticipant*>(controller)) {
		extParams.insert('o', participant->name());
		if (participant->channel())
			extParams.insert('n', participant->channel()->id());
		account = participant->account();
		type = IrcCommandAlias::Participant;
	} else if (IrcChannel *channel = qobject_cast<IrcChannel*>(controller)) {
		extParams.insert('n', channel->id());
		account = channel->account();
		type = IrcCommandAlias::Channel;
	}
	Q_ASSERT(account);
	account->send(command->generate(type, QStringList(), extParams));
}

} // namespace irc
} // namespace qutim_sdk_0_3
