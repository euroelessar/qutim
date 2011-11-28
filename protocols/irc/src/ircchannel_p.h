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

#ifndef IRCCHANNEL_P_H
#define IRCCHANNEL_P_H

#include "ircchannel.h"

namespace qutim_sdk_0_3 {

namespace irc {

class IrcJoinLeftActionGenerator : public ActionGenerator
{
	Q_GADGET
public:
	IrcJoinLeftActionGenerator(QObject *receiver, const char *member);
protected:
	virtual void showImpl(QAction *action, QObject *obj);
};

typedef QSharedPointer<IrcChannelParticipant> ParticipantPointer;

class IrcChannelPrivate
{
public:
	ParticipantPointer me;
	QString name;
	QHash<QString, ParticipantPointer> users;
	QString topic;
	bool isJoined;
	bool autojoin;
	QString lastPassword;
	QString bookmarkName;
	bool reconnect;
};

}

} // namespace qutim_sdk_0_3::irc

#endif // IRCCHANNEL_P_H

