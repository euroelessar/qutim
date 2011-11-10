/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright (C) 2011 Prokhin Alexey <alexey.prokhin@yandex.ru>
**
*****************************************************************************
**
** $QUTIM_BEGIN_LICENSE$
** This program is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation, either version 3 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
** See the GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program.  If not, see http://www.gnu.org/licenses/.
** $QUTIM_END_LICENSE$
**
****************************************************************************/

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
	d->commandAutoDelete = false;
}

IrcActionGenerator::~IrcActionGenerator()
{
	if (d->commandAutoDelete)
		delete d->command;
}

void IrcActionGenerator::enableAutoDeleteOfCommand(bool del)
{
	d->commandAutoDelete = del;
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
		QString tmp = participant->hostMask();
		if (!tmp.isEmpty())
			extParams.insert('m', tmp);
		tmp = participant->hostUser();
		if (!tmp.isEmpty())
			extParams.insert('u', tmp);
		tmp = participant->domain();
		if (!tmp.isEmpty())
			extParams.insert('d', tmp);
		tmp = participant->host();
		if (!tmp.isEmpty())
			extParams.insert('h', tmp);
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
	account->send(command, true, type, QStringList(), extParams);
}

} // namespace irc
} // namespace qutim_sdk_0_3

